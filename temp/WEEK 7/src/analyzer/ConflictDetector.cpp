#include "analyzer/ConflictDetector.h"

ConflictDetector::ConflictDetector(const SymbolTable& symbolTable)
    : symbolTable_(symbolTable) {}

std::vector<Conflict> ConflictDetector::detectConflicts(
    const std::vector<Rule*>& rules) {
    
    std::vector<Conflict> conflicts;
    
    // O(n²) pairwise comparison
    for (size_t i = 0; i < rules.size(); i++) {
        for (size_t j = i + 1; j < rules.size(); j++) {
            Rule* r1 = rules[i];
            Rule* r2 = rules[j];
            
            // Check if rules overlap and have different effects
            if (rulesOverlap(*r1, *r2)) {
                if (r1->effect != r2->effect) {
                    std::string reason = "Conflicting allow/deny rules for (" +
                        r1->subject + ", " + r1->action + ", " + r1->resource + ")";
                    conflicts.emplace_back(r1, r2, reason);
                }
            }
        }
    }
    
    return conflicts;
}

bool ConflictDetector::rulesOverlap(const Rule& r1, const Rule& r2) {
    // Check if subjects overlap
    if (!subjectsOverlap(r1, r2)) {
        return false;
    }
    
    // Check if actions match
    if (r1.action != r2.action) {
        return false;
    }
    
    // Check if resources match
    if (r1.resource != r2.resource) {
        return false;
    }
    
    // Check if conditions overlap
    if (!conditionsOverlap(r1.condition.get(), r2.condition.get())) {
        return false;
    }
    
    return true;
}

bool ConflictDetector::subjectsOverlap(const Rule& r1, const Rule& r2) {
    // Same subject type and same identifier
    if (r1.subjectType == r2.subjectType && r1.subject == r2.subject) {
        return true;
    }
    
    // Both are roles - check inheritance
    if (r1.subjectType == Rule::SubjectType::ROLE && 
        r2.subjectType == Rule::SubjectType::ROLE) {
        
        // Check if one role inherits from the other
        if (roleInheritsFrom(r1.subject, r2.subject) ||
            roleInheritsFrom(r2.subject, r1.subject)) {
            return true;
        }
    }
    
    return false;
}

bool ConflictDetector::roleInheritsFrom(const std::string& child,
                                       const std::string& parent) {
    if (child == parent) return true;
    
    if (!symbolTable_.hasRole(child)) return false;
    
    const auto& roleSymbol = symbolTable_.getRole(child);
    if (roleSymbol.parent.empty()) return false;
    
    // Recursively check parent chain
    return roleInheritsFrom(roleSymbol.parent, parent);
}

bool ConflictDetector::conditionsOverlap(Expression* cond1, Expression* cond2) {
    // If both conditions are null, they overlap (both always apply)
    if (!cond1 && !cond2) {
        return true;
    }
    
    // If one has condition and other doesn't, the unconditioned one
    // is more general and they overlap
    if (!cond1 || !cond2) {
        return true;
    }
    
    // TODO: Implement sophisticated condition overlap analysis
    // For now, conservatively assume they might overlap
    // A complete implementation would use SMT solvers or constraint analysis
    return true;
}