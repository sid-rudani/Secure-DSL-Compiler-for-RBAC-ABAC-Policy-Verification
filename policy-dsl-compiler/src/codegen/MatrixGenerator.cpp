#include "codegen/MatrixGenerator.h"

MatrixGenerator::MatrixGenerator(const SymbolTable& symbolTable)
    : symbolTable_(symbolTable) {}

AccessMatrix MatrixGenerator::generate(const std::vector<Rule*>& rules) {
    AccessMatrix matrix;
    
    // Get all subjects (users and roles)
    std::vector<std::string> subjects = getAllSubjects();
    
    // For each combination of (subject, action, resource)
    for (const auto& subject : subjects) {
        for (const auto& [actionName, _] : symbolTable_.getActions()) {
            for (const auto& [resourceName, __] : symbolTable_.getResources()) {
                AccessDecision decision = evaluateAccess(subject, actionName,
                                                        resourceName, rules);
                matrix.addEntry(subject, actionName, resourceName, decision);
            }
        }
    }
    
    return matrix;
}

std::vector<std::string> MatrixGenerator::getAllSubjects() {
    std::vector<std::string> subjects;
    
    // Add all users
    for (const auto& [userName, _] : symbolTable_.getUsers()) {
        subjects.push_back("user:" + userName);
    }
    
    // Add all roles
    for (const auto& [roleName, _] : symbolTable_.getRoles()) {
        subjects.push_back("role:" + roleName);
    }
    
    return subjects;
}

AccessDecision MatrixGenerator::evaluateAccess(
    const std::string& subject,
    const std::string& action,
    const std::string& resource,
    const std::vector<Rule*>& rules) {
    
    bool hasAllow = false;
    bool hasDeny = false;
    
    // Check all matching rules
    for (const Rule* rule : rules) {
        if (ruleMatches(*rule, subject, action, resource)) {
            if (rule->effect == Rule::Effect::ALLOW) {
                hasAllow = true;
            } else {
                hasDeny = true;
            }
        }
    }
    
    // Explicit deny overrides allow
    if (hasDeny) {
        return AccessDecision::DENY;
    }
    
    // Explicit allow
    if (hasAllow) {
        return AccessDecision::ALLOW;
    }
    
    // Default deny
    return AccessDecision::DENY;
}

bool MatrixGenerator::ruleMatches(const Rule& rule, 
                                 const std::string& subject,
                                 const std::string& action,
                                 const std::string& resource) {
    // Check action
    if (rule.action != action) {
        return false;
    }
    
    // Check resource
    if (rule.resource != resource) {
        return false;
    }
    
    // Check subject
    if (!subjectMatches(rule, subject)) {
        return false;
    }
    
    // TODO: Evaluate condition if present
    // For now, assume condition is always satisfied
    
    return true;
}

bool MatrixGenerator::subjectMatches(const Rule& rule, 
                                    const std::string& subject) {
    // Parse subject (format: "user:alice" or "role:admin")
    size_t colonPos = subject.find(':');
    if (colonPos == std::string::npos) {
        return false;
    }
    
    std::string subjectType = subject.substr(0, colonPos);
    std::string subjectName = subject.substr(colonPos + 1);
    
    if (rule.subjectType == Rule::SubjectType::USER) {
        return subjectType == "user" && rule.subject == subjectName;
    } else { // ROLE
        if (subjectType == "role") {
            // Direct match or inheritance
            return isRoleOrInheritsFrom(subjectName, rule.subject);
        }
    }
    
    return false;
}

bool MatrixGenerator::isRoleOrInheritsFrom(const std::string& subject,
                                          const std::string& targetRole) {
    if (subject == targetRole) {
        return true;
    }
    
    // Check inheritance chain
    std::string current = subject;
    while (!current.empty()) {
        if (current == targetRole) {
            return true;
        }
        
        if (symbolTable_.hasRole(current)) {
            const auto& roleSymbol = symbolTable_.getRole(current);
            current = roleSymbol.parent;
        } else {
            break;
        }
    }
    
    return false;
}