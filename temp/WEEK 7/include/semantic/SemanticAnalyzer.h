#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "SymbolTable.h"
#include "../parser/ASTNodes.h"
#include <memory>
#include <vector>
#include <unordered_set>

struct SemanticIssue {
    enum class Severity { ERROR, WARNING };
    
    Severity severity;
    std::string message;
    SourceLocation location;
    
    SemanticIssue(Severity sev, const std::string& msg, const SourceLocation& loc)
        : severity(sev), message(msg), location(loc) {}
};

class SemanticAnalyzer : public ASTVisitor {
public:
    SemanticAnalyzer();
    
    void analyze(PolicyNode& root);
    
    const SymbolTable& getSymbolTable() const { return symbolTable_; }
    const std::vector<SemanticIssue>& getIssues() const { return issues_; }
    bool hasErrors() const;
    
    // Visitor methods
    void visit(PolicyNode& node) override;
    void visit(UserDecl& node) override;
    void visit(RoleDecl& node) override;
    void visit(ResourceDecl& node) override;
    void visit(ActionDecl& node) override;
    void visit(AttributeDecl& node) override;
    void visit(Rule& node) override;
    void visit(BinaryExpr& node) override;
    void visit(UnaryExpr& node) override;
    void visit(AttributeExpr& node) override;
    void visit(LiteralExpr& node) override;
    
private:
    SymbolTable symbolTable_;
    std::vector<SemanticIssue> issues_;
    std::vector<Rule*> rules_; // Store rules for later analysis
    
    void reportError(const std::string& message, const SourceLocation& location);
    void reportWarning(const std::string& message, const SourceLocation& location);
    
    // Semantic checks
    void checkRoleHierarchyCycles();
    bool detectCycleDFS(const std::string& role,
                       std::unordered_set<std::string>& visited,
                       std::unordered_set<std::string>& recursionStack);
    void validateRuleReferences(Rule& rule);
    AttributeType checkExpressionType(Expression& expr);
    void checkConditionType(Expression* condition);
};

#endif // SEMANTIC_ANALYZER_H