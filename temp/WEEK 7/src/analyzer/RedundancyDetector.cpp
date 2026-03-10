#include "analyzer/RedundancyDetector.h"

RedundancyDetector::RedundancyDetector(const SymbolTable& symbolTable)
    : symbolTable_(symbolTable) {}

std::vector<RedundantRule> RedundancyDetector::detectRedundancy(
    const std::vector<Rule*>& rules) {
    
    std::vector<RedundantRule> redundant;
    
    // Check each rule against all others
    for (size_t i = 0; i < rules.size(); i++) {
        for (size_t j = 0; j < rules.size(); j++) {
            if (i == j) continue;
            
            Rule* r1 = rules[i];
            Rule* r2 = rules[j];
            
            // Check if r1 is subsumed by r2
            if (isSubsumedBy(*r1, *r2)) {
                std::string reason = "Rule '" + r1->name + 
                    "' is subsumed by rule '" + r2->name + "'";
                redundant.emplace_back(r1, reason);
                break; // Don't check other rules for this one
            }
        }
    }
    
    return redundant;
}

bool RedundancyDetector::isSubsumedBy(const Rule& r1, const Rule& r2) {
    // A rule is subsumed if another rule covers all its cases
    
    // Must have same effect
    if (r1.effect != r2.effect) {
        return false;
    }
    
    // Must have same action and resource
    if (r1.action != r2.action || r1.resource != r2.resource) {
        return false;
    }
    
    // r2's subject must be broader or equal to r1's subject
    if (!subjectSubsumes(r2, r1)) {
        return false;
    }
    
    // r2's condition must be broader or equal to r1's condition
    if (!conditionSubsumes(r2.condition.get(), r1.condition.get())) {
        return false;
    }
    
    return true;
}

bool RedundancyDetector::subjectSubsumes(const Rule& broader, 
                                        const Rule& narrower) {
    // Same subject
    if (broader.subjectType == narrower.subjectType && 
        broader.subject == narrower.subject) {
        return true;
    }
    
    // Broader is a parent role of narrower
    if (broader.subjectType == Rule::SubjectType::ROLE && 
        narrower.subjectType == Rule::SubjectType::ROLE) {
        
        // Check if narrower's role inherits from broader's role
        std::string current = narrower.subject;
        while (!current.empty()) {
            if (current == broader.subject) {
                return true;
            }
            
            if (symbolTable_.hasRole(current)) {
                const auto& roleSymbol = symbolTable_.getRole(current);
                current = roleSymbol.parent;
            } else {
                break;
            }
        }
    }
    
    return false;
}

bool RedundancyDetector::conditionSubsumes(Expression* broader, 
                                          Expression* narrower) {
    // No condition (always true) subsumes any condition
    if (!broader) {
        return true;
    }
    
    // If broader has condition but narrower doesn't, no subsumption
    if (!narrower) {
        return false;
    }
    
    // TODO: Implement sophisticated condition subsumption analysis
    // For now, only consider exact matches
    // A complete implementation would use logical implication checking
    
    return false; // Conservative: assume no subsumption
}