#ifndef PRIVILEGE_ESCALATION_H
#define PRIVILEGE_ESCALATION_H

#include "../parser/ASTNodes.h"
#include "../semantic/SymbolTable.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct Permission {
    std::string action;
    std::string resource;
    
    bool operator==(const Permission& other) const {
        return action == other.action && resource == other.resource;
    }
};

// Hash function for Permission
namespace std {
    template<>
    struct hash<Permission> {
        size_t operator()(const Permission& p) const {
            return hash<string>()(p.action) ^ (hash<string>()(p.resource) << 1);
        }
    };
}

struct EscalationPath {
    std::string startRole;
    std::string endRole;
    std::vector<std::string> path; // Role chain
    std::vector<Permission> escalatedPermissions;
    std::string description;
};

class PrivilegeEscalationDetector {
public:
    PrivilegeEscalationDetector(const SymbolTable& symbolTable);
    
    std::vector<EscalationPath> detectEscalation(const std::vector<Rule*>& rules);
    
private:
    const SymbolTable& symbolTable_;
    std::unordered_map<std::string, std::unordered_set<Permission>> 
        directPermissions_;
    std::unordered_map<std::string, std::unordered_set<Permission>> 
        inheritedPermissions_;
    
    void buildPermissionMaps(const std::vector<Rule*>& rules);
    void computeInheritedPermissions();
    void computeRoleInheritance(const std::string& role,
                                std::unordered_set<Permission>& perms,
                                std::unordered_set<std::string>& visited);
    
    std::vector<std::string> findPathToAncestor(const std::string& from,
                                                const std::string& to);
    std::string formatPath(const std::vector<std::string>& path);
};

#endif // PRIVILEGE_ESCALATION_H