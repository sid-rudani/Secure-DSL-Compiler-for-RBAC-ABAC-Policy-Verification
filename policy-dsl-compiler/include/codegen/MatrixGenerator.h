#ifndef MATRIX_GENERATOR_H
#define MATRIX_GENERATOR_H

#include "AccessMatrix.h"
#include "../parser/ASTNodes.h"
#include "../semantic/SymbolTable.h"
#include <memory>

class MatrixGenerator {
public:
    MatrixGenerator(const SymbolTable& symbolTable);
    
    AccessMatrix generate(const std::vector<Rule*>& rules);
    
private:
    const SymbolTable& symbolTable_;
    
    std::vector<std::string> getAllSubjects();
    AccessDecision evaluateAccess(const std::string& subject,
                                  const std::string& action,
                                  const std::string& resource,
                                  const std::vector<Rule*>& rules);
    
    bool ruleMatches(const Rule& rule, const std::string& subject,
                    const std::string& action, const std::string& resource);
    bool subjectMatches(const Rule& rule, const std::string& subject);
    bool isRoleOrInheritsFrom(const std::string& subject, 
                             const std::string& targetRole);
};

#endif // MATRIX_GENERATOR_H