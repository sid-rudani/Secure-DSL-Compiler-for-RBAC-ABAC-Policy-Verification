#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "../parser/ASTNodes.h"
#include "../utils/Error.h"
#include <string>

class TypeChecker {
public:
    bool checkType(const std::string& expected, const std::string& actual);
    bool isValidActionType(const std::string& action);
    bool isValidResourceType(const std::string& resource);
    bool isValidRoleType(const std::string& role);
    
private:
    ErrorHandler errorHandler;
};

#endif // TYPE_CHECKER_H
