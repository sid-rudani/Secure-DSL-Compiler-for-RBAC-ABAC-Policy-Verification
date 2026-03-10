#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "semantic/SemanticAnalyzer.h"
#include "analyzer/ConflictDetector.h"
#include "analyzer/PrivilegeEscalation.h"
#include "analyzer/RedundancyDetector.h"
#include "codegen/MatrixGenerator.h"
#include "utils/Error.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

struct CompilerOptions {
    std::string inputFile;
    std::string outputFile;
    std::string format = "text"; // text or json
    bool warningsAsErrors = false;
    bool verbose = false;
    std::string analysis = "all"; // all, conflict, priv-esc, redundancy
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <input.pol> [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>              Output file for access matrix\n";
    std::cout << "  --format <fmt>         Output format: text, json (default: text)\n";
    std::cout << "  --warnings-as-errors   Treat warnings as errors\n";
    std::cout << "  --verbose              Enable verbose output\n";
    std::cout << "  --analysis <type>      Analysis type: all, conflict, priv-esc, redundancy\n";
    std::cout << "  --help                 Show this help message\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write to file: " + filename);
    }
    
    file << content;
}

void printIssues(const std::vector<SemanticIssue>& issues) {
    for (const auto& issue : issues) {
        std::string severity = (issue.severity == SemanticIssue::Severity::ERROR) 
                              ? "ERROR" : "WARNING";
        
        std::cout << "[" << severity << "] " 
                  << issue.location.toString() << ": "
                  << issue.message << "\n";
    }
}

void printConflicts(const std::vector<Conflict>& conflicts) {
    if (conflicts.empty()) {
        std::cout << "✓ No conflicts detected\n";
        return;
    }
    
    std::cout << "\n⚠ Conflict Detection:\n";
    std::cout << "Found " << conflicts.size() << " conflict(s):\n\n";
    
    for (const auto& conflict : conflicts) {
        std::cout << "[CONFLICT] " << conflict.reason << "\n";
        std::cout << "  Rule 1: " << conflict.rule1->name 
                  << " at " << conflict.rule1->location.toString() << "\n";
        std::cout << "  Rule 2: " << conflict.rule2->name 
                  << " at " << conflict.rule2->location.toString() << "\n\n";
    }
}

void printEscalations(const std::vector<EscalationPath>& escalations) {
    if (escalations.empty()) {
        std::cout << "✓ No privilege escalation detected\n";
        return;
    }
    
    std::cout << "\n🔓 Privilege Escalation Detection:\n";
    std::cout << "Found " << escalations.size() << " escalation path(s):\n\n";
    
    for (const auto& esc : escalations) {
        std::cout << "[PRIV_ESC] " << esc.description << "\n";
        std::cout << "  Escalated Permissions:\n";
        for (const auto& perm : esc.escalatedPermissions) {
            std::cout << "    - (" << perm.action << ", " 
                      << perm.resource << ")\n";
        }
        std::cout << "\n";
    }
}

void printRedundancy(const std::vector<RedundantRule>& redundant) {
    if (redundant.empty()) {
        std::cout << "✓ No redundant rules detected\n";
        return;
    }
    
    std::cout << "\n📋 Redundancy Detection:\n";
    std::cout << "Found " << redundant.size() << " redundant rule(s):\n\n";
    
    for (const auto& red : redundant) {
        std::cout << "[REDUNDANT] " << red.reason << "\n";
        std::cout << "  at " << red.rule->location.toString() << "\n\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    CompilerOptions options;
    
    // Parse command line arguments
    options.inputFile = argv[1];
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (strcmp(argv[i], "--format") == 0 && i + 1 < argc) {
            options.format = argv[++i];
        } else if (strcmp(argv[i], "--warnings-as-errors") == 0) {
            options.warningsAsErrors = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            options.verbose = true;
        } else if (strcmp(argv[i], "--analysis") == 0 && i + 1 < argc) {
            options.analysis = argv[++i];
        }
    }
    
    try {
        // Read source file
        std::string source = readFile(options.inputFile);
        
        if (options.verbose) {
            std::cout << "Compiling: " << options.inputFile << "\n\n";
        }
        
        // Step 1: Lexical Analysis
        if (options.verbose) std::cout << "Phase 1: Lexical Analysis...\n";
        Lexer lexer(source, options.inputFile);
        std::vector<Token> tokens = lexer.tokenize();
        
        // Step 2: Syntax Analysis
        if (options.verbose) std::cout << "Phase 2: Syntax Analysis...\n";
        Parser parser(tokens);
        auto ast = parser.parse();
        
        // Step 3: Semantic Analysis
        if (options.verbose) std::cout << "Phase 3: Semantic Analysis...\n";
        SemanticAnalyzer semanticAnalyzer;
        semanticAnalyzer.analyze(*ast);
        
        // Print semantic issues
        const auto& issues = semanticAnalyzer.getIssues();
        if (!issues.empty()) {
            printIssues(issues);
        }
        
        // Check for errors
        if (semanticAnalyzer.hasErrors()) {
            std::cerr << "\nCompilation failed with errors.\n";
            return 1;
        }
        
        // Collect all rules
        std::vector<Rule*> rules;
        for (auto& rule : ast->rules) {
            rules.push_back(dynamic_cast<Rule*>(rule.get()));
        }
        
        // Step 4: Security Analysis
        if (options.verbose) std::cout << "Phase 4: Security Analysis...\n\n";
        
        const auto& symbolTable = semanticAnalyzer.getSymbolTable();
        
        bool hasWarnings = false;
        
        // Conflict Detection
        if (options.analysis == "all" || options.analysis == "conflict") {
            ConflictDetector conflictDetector(symbolTable);
            auto conflicts = conflictDetector.detectConflicts(rules);
            printConflicts(conflicts);
            if (!conflicts.empty()) hasWarnings = true;
        }
        
        // Privilege Escalation
        if (options.analysis == "all" || options.analysis == "priv-esc") {
            PrivilegeEscalationDetector privEscDetector(symbolTable);
            auto escalations = privEscDetector.detectEscalation(rules);
            printEscalations(escalations);
            if (!escalations.empty()) hasWarnings = true;
        }
        
        // Redundancy Detection
        if (options.analysis == "all" || options.analysis == "redundancy") {
            RedundancyDetector redundancyDetector(symbolTable);
            auto redundant = redundancyDetector.detectRedundancy(rules);
            printRedundancy(redundant);
            if (!redundant.empty()) hasWarnings = true;
        }
        
        // Check warnings-as-errors
        if (options.warningsAsErrors && hasWarnings) {
            std::cerr << "\nCompilation failed: Warnings treated as errors.\n";
            return 1;
        }
        
        // Step 5: Code Generation
        if (options.verbose) std::cout << "\nPhase 5: Code Generation...\n";
        MatrixGenerator matrixGen(symbolTable);
        AccessMatrix matrix = matrixGen.generate(rules);
        
        // Output
        std::string output;
        if (options.format == "json") {
            output = matrix.toJSON();
        } else {
            output = matrix.toText();
        }
        
        if (!options.outputFile.empty()) {
            writeFile(options.outputFile, output);
            std::cout << "\nAccess matrix written to: " << options.outputFile << "\n";
        } else {
            std::cout << "\n=== Access Matrix ===\n\n";
            std::cout << output;
        }
        
        std::cout << "\n✓ Compilation successful!\n";
        return 0;
        
    } catch (const CompilerError& e) {
        std::cerr << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}