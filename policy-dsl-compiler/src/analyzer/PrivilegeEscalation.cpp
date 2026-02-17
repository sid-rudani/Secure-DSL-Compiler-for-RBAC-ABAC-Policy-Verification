#include "analyzer/PrivilegeEscalation.h"
#include <algorithm>

PrivilegeEscalationDetector::PrivilegeEscalationDetector(
    const SymbolTable& symbolTable)
    : symbolTable_(symbolTable) {}

std::vector<EscalationPath> PrivilegeEscalationDetector::detectEscalation(
    const std::vector<Rule*>& rules) {
    
    std::vector<EscalationPath> escalations;
    
    // Step 1: Build permission maps
    buildPermissionMaps(rules);
    
    // Step 2: Compute inherited permissions for all roles
    computeInheritedPermissions();
    
    // Step 3: Compare direct vs inherited permissions
    for (const auto& [roleName, roleSymbol] : symbolTable_.getRoles()) {
        auto directPerms = directPermissions_[roleName];
        auto inheritedPerms = inheritedPermissions_[roleName];
        
        // Find permissions gained only through inheritance
        std::unordered_set<Permission> escalated;
        for (const auto& perm : inheritedPerms) {
            if (directPerms.find(perm) == directPerms.end()) {
                escalated.insert(perm);
            }
        }
        
        // If there are escalated permissions, create escalation report
        if (!escalated.empty()) {
            // Find which ancestor role(s) grant these permissions
            for (const auto& perm : escalated) {
                // Find the role in hierarchy that grants this permission
                std::string grantingRole;
                for (const auto& [ancestorName, _] : symbolTable_.getRoles()) {
                    if (directPermissions_[ancestorName].count(perm) > 0) {
                        // Check if roleName inherits from ancestorName
                        auto path = findPathToAncestor(roleName, ancestorName);
                        if (!path.empty()) {
                            grantingRole = ancestorName;
                            
                            EscalationPath esc;
                            esc.startRole = roleName;
                            esc.endRole = grantingRole;
                            esc.path = path;
                            esc.escalatedPermissions.push_back(perm);
                            esc.description = "Role '" + roleName + 
                                "' gains permission (" + perm.action + ", " + 
                                perm.resource + ") through inheritance from '" + 
                                grantingRole + "' via path: " + 
                                formatPath(path);
                            
                            escalations.push_back(esc);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return escalations;
}

void PrivilegeEscalationDetector::buildPermissionMaps(
    const std::vector<Rule*>& rules) {
    
    directPermissions_.clear();
    
    for (const Rule* rule : rules) {
        // Only process ALLOW rules for roles
        if (rule->effect == Rule::Effect::ALLOW && 
            rule->subjectType == Rule::SubjectType::ROLE) {
            
            Permission perm{rule->action, rule->resource};
            directPermissions_[rule->subject].insert(perm);
        }
    }
}

void PrivilegeEscalationDetector::computeInheritedPermissions() {
    inheritedPermissions_.clear();
    
    for (const auto& [roleName, _] : symbolTable_.getRoles()) {
        std::unordered_set<std::string> visited;
        std::unordered_set<Permission> perms;
        computeRoleInheritance(roleName, perms, visited);
        inheritedPermissions_[roleName] = perms;
    }
}

void PrivilegeEscalationDetector::computeRoleInheritance(
    const std::string& role,
    std::unordered_set<Permission>& perms,
    std::unordered_set<std::string>& visited) {
    
    if (visited.count(role) > 0) {
        return; // Already visited (cycle protection)
    }
    visited.insert(role);
    
    // Add direct permissions
    if (directPermissions_.count(role) > 0) {
        for (const auto& perm : directPermissions_[role]) {
            perms.insert(perm);
        }
    }
    
    // Recursively add parent permissions
    if (symbolTable_.hasRole(role)) {
        const auto& roleSymbol = symbolTable_.getRole(role);
        if (!roleSymbol.parent.empty()) {
            computeRoleInheritance(roleSymbol.parent, perms, visited);
        }
    }
}

std::vector<std::string> PrivilegeEscalationDetector::findPathToAncestor(
    const std::string& from, const std::string& to) {
    
    std::vector<std::string> path;
    path.push_back(from);
    
    std::string current = from;
    while (current != to) {
        if (!symbolTable_.hasRole(current)) {
            return {}; // No path found
        }
        
        const auto& roleSymbol = symbolTable_.getRole(current);
        if (roleSymbol.parent.empty()) {
            return {}; // No path found
        }
        
        current = roleSymbol.parent;
        path.push_back(current);
        
        if (path.size() > 100) {
            return {}; // Cycle or too deep
        }
    }
    
    return path;
}

std::string PrivilegeEscalationDetector::formatPath(
    const std::vector<std::string>& path) {
    
    std::string result;
    for (size_t i = 0; i < path.size(); i++) {
        result += path[i];
        if (i < path.size() - 1) {
            result += " -> ";
        }
    }
    return result;
}