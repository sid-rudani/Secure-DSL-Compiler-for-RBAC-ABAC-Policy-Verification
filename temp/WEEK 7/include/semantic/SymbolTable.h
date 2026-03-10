#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "../parser/ASTNodes.h"

struct UserSymbol {
    std::string name;
    SourceLocation location;
};

struct RoleSymbol {
    std::string name;
    std::string parent;
    SourceLocation location;
};

struct ResourceSymbol {
    std::string name;
    SourceLocation location;
};

struct ActionSymbol {
    std::string name;
    SourceLocation location;
};

struct AttributeSymbol {
    std::string namespace_;
    std::string name;
    AttributeType type;
    SourceLocation location;
};

class SymbolTable {
public:
    void addUser(const std::string& name, const SourceLocation& loc);
    void addRole(const std::string& name, const std::string& parent, 
                 const SourceLocation& loc);
    void addResource(const std::string& name, const SourceLocation& loc);
    void addAction(const std::string& name, const SourceLocation& loc);
    void addAttribute(const std::string& ns, const std::string& name, 
                     AttributeType type, const SourceLocation& loc);
    
    bool hasUser(const std::string& name) const;
    bool hasRole(const std::string& name) const;
    bool hasResource(const std::string& name) const;
    bool hasAction(const std::string& name) const;
    bool hasAttribute(const std::string& ns, const std::string& name) const;
    
    const RoleSymbol& getRole(const std::string& name) const;
    const AttributeSymbol& getAttribute(const std::string& ns, 
                                       const std::string& name) const;
    
    const auto& getUsers() const { return users_; }
    const auto& getRoles() const { return roles_; }
    const auto& getResources() const { return resources_; }
    const auto& getActions() const { return actions_; }
    
private:
    std::unordered_map<std::string, UserSymbol> users_;
    std::unordered_map<std::string, RoleSymbol> roles_;
    std::unordered_map<std::string, ResourceSymbol> resources_;
    std::unordered_map<std::string, ActionSymbol> actions_;
    std::unordered_map<std::string, AttributeSymbol> attributes_;
};

#endif // SYMBOL_TABLE_H