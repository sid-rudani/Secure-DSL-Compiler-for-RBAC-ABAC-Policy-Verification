#include "semantic/SymbolTable.h"
#include <stdexcept>

void SymbolTable::addUser(const std::string& name, const SourceLocation& loc) {
    if (users_.find(name) != users_.end()) {
        throw std::runtime_error("Duplicate user declaration: " + name);
    }
    users_[name] = UserSymbol{name, loc};
}

void SymbolTable::addRole(const std::string& name, const std::string& parent,
                         const SourceLocation& loc) {
    if (roles_.find(name) != roles_.end()) {
        throw std::runtime_error("Duplicate role declaration: " + name);
    }
    roles_[name] = RoleSymbol{name, parent, loc};
}

void SymbolTable::addResource(const std::string& name, const SourceLocation& loc) {
    if (resources_.find(name) != resources_.end()) {
        throw std::runtime_error("Duplicate resource declaration: " + name);
    }
    resources_[name] = ResourceSymbol{name, loc};
}

void SymbolTable::addAction(const std::string& name, const SourceLocation& loc) {
    if (actions_.find(name) != actions_.end()) {
        throw std::runtime_error("Duplicate action declaration: " + name);
    }
    actions_[name] = ActionSymbol{name, loc};
}

void SymbolTable::addAttribute(const std::string& ns, const std::string& name,
                               AttributeType type, const SourceLocation& loc) {
    std::string key = ns + "." + name;
    if (attributes_.find(key) != attributes_.end()) {
        throw std::runtime_error("Duplicate attribute declaration: " + key);
    }
    attributes_[key] = AttributeSymbol{ns, name, type, loc};
}

bool SymbolTable::hasUser(const std::string& name) const {
    return users_.find(name) != users_.end();
}

bool SymbolTable::hasRole(const std::string& name) const {
    return roles_.find(name) != roles_.end();
}

bool SymbolTable::hasResource(const std::string& name) const {
    return resources_.find(name) != resources_.end();
}

bool SymbolTable::hasAction(const std::string& name) const {
    return actions_.find(name) != actions_.end();
}

bool SymbolTable::hasAttribute(const std::string& ns, const std::string& name) const {
    std::string key = ns + "." + name;
    return attributes_.find(key) != attributes_.end();
}

const RoleSymbol& SymbolTable::getRole(const std::string& name) const {
    auto it = roles_.find(name);
    if (it == roles_.end()) {
        throw std::runtime_error("Role not found: " + name);
    }
    return it->second;
}

const AttributeSymbol& SymbolTable::getAttribute(const std::string& ns,
                                                 const std::string& name) const {
    std::string key = ns + "." + name;
    auto it = attributes_.find(key);
    if (it == attributes_.end()) {
        throw std::runtime_error("Attribute not found: " + key);
    }
    return it->second;
}