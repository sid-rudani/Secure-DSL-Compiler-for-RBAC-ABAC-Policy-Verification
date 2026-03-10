#ifndef CONFLICT_DETECTOR_H
#define CONFLICT_DETECTOR_H

#include "../parser/ASTNodes.h"
#include "../semantic/SymbolTable.h"
#include <vector>
#include <memory>

struct Conflict {
    Rule* rule1;
    Rule* rule2;
    std::string reason;
    
    Conflict(Rule* r1, Rule* r2, const std::string& rsn)
        : rule1(r1), rule2(r2), reason(rsn) {}
};

class ConflictDetector {
public:
    ConflictDetector(const SymbolTable& symbolTable);
    
    std::vector<Conflict> detectConflicts(const std::vector<Rule*>& rules);
    
private:
    const SymbolTable& symbolTable_;
    
    bool rulesOverlap(const Rule& r1, const Rule& r2);
    bool subjectsOverlap(const Rule& r1, const Rule& r2);
    bool conditionsOverlap(Expression* cond1, Expression* cond2);
    
    // Helper to check if one role inherits from another
    bool roleInheritsFrom(const std::string& child, const std::string& parent);
};

#endif // CONFLICT_DETECTOR_H