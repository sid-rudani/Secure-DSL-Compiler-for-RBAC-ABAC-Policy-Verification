#include "../../include/semantic/TypeChecker.h"

bool TypeChecker::checkType(const std::string& expected, const std::string& actual) {
    return expected == actual;
}

bool TypeChecker::isValidActionType(const std::string& action) {
    // Define valid action types
    return action == "read" || action == "write" || action == "delete" || action == "execute";
}

bool TypeChecker::isValidResourceType(const std::string& resource) {
    // Define valid resource types
    return !resource.empty();
}

bool TypeChecker::isValidRoleType(const std::string& role) {
    // Define valid role types
    return !role.empty();
}
