#ifndef REDUNDANCY_DETECTOR_H
#define REDUNDANCY_DETECTOR_H

#include "../parser/ASTNodes.h"
#include "../semantic/SymbolTable.h"
#include <vector>

struct RedundantRule {
    Rule* rule;
    std::string reason;
    
    RedundantRule(Rule* r, const std::string& rsn)
        : rule(r), reason(rsn) {}
};

class RedundancyDetector {
public:
    RedundancyDetector(const SymbolTable& symbolTable);
    
    std::vector<RedundantRule> detectRedundancy(const std::vector<Rule*>& rules);
    
private:
    const SymbolTable& symbolTable_;
    
    bool isSubsumedBy(const Rule& r1, const Rule& r2);
    bool subjectSubsumes(const Rule& broader, const Rule& narrower);
    bool conditionSubsumes(Expression* broader, Expression* narrower);
};

#endif // REDUNDANCY_DETECTOR_H