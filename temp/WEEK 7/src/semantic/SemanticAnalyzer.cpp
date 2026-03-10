#include "semantic/SemanticAnalyzer.h"
#include <stack>
#include <unordered_set>

SemanticAnalyzer::SemanticAnalyzer() {}

void SemanticAnalyzer::analyze(PolicyNode& root) {
    issues_.clear();
    rules_.clear();
    
    // First pass: build symbol table
    root.accept(*this);
    
    // Second pass: check for cycles in role hierarchy
    checkRoleHierarchyCycles();
}

bool SemanticAnalyzer::hasErrors() const {
    for (const auto& issue : issues_) {
        if (issue.severity == SemanticIssue::Severity::ERROR) {
            return true;
        }
    }
    return false;
}

void SemanticAnalyzer::visit(PolicyNode& node) {
    // Visit all declarations
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
    
    // Visit all rules
    for (auto& rule : node.rules) {
        rule->accept(*this);
    }
}

void SemanticAnalyzer::visit(UserDecl& node) {
    try {
        symbolTable_.addUser(node.identifier, node.location);
    } catch (const std::exception& e) {
        reportError(e.what(), node.location);
    }
}

void SemanticAnalyzer::visit(RoleDecl& node) {
    try {
        symbolTable_.addRole(node.identifier, node.parent, node.location);
        
        // Check if parent exists (if specified)
        if (!node.parent.empty() && !symbolTable_.hasRole(node.parent)) {
            reportError("Parent role '" + node.parent + "' not declared", 
                       node.location);
        }
    } catch (const std::exception& e) {
        reportError(e.what(), node.location);
    }
}

void SemanticAnalyzer::visit(ResourceDecl& node) {
    try {
        symbolTable_.addResource(node.identifier, node.location);
    } catch (const std::exception& e) {
        reportError(e.what(), node.location);
    }
}

void SemanticAnalyzer::visit(ActionDecl& node) {
    try {
        symbolTable_.addAction(node.identifier, node.location);
    } catch (const std::exception& e) {
        reportError(e.what(), node.location);
    }
}

void SemanticAnalyzer::visit(AttributeDecl& node) {
    try {
        symbolTable_.addAttribute(node.namespace_, node.name, 
                                  node.type, node.location);
    } catch (const std::exception& e) {
        reportError(e.what(), node.location);
    }
}

void SemanticAnalyzer::visit(Rule& node) {
    // Store rule for later analysis
    rules_.push_back(&node);
    
    // Validate rule references
    validateRuleReferences(node);
    
    // Check condition type if present
    if (node.condition) {
        checkConditionType(node.condition.get());
    }
}

void SemanticAnalyzer::validateRuleReferences(Rule& rule) {
    // Check subject exists
    if (rule.subjectType == Rule::SubjectType::USER) {
        if (!symbolTable_.hasUser(rule.subject)) {
            reportError("Undefined user: " + rule.subject, rule.location);
        }
    } else { // ROLE
        if (!symbolTable_.hasRole(rule.subject)) {
            reportError("Undefined role: " + rule.subject, rule.location);
        }
    }
    
    // Check action exists
    if (!symbolTable_.hasAction(rule.action)) {
        reportError("Undefined action: " + rule.action, rule.location);
    }
    
    // Check resource exists
    if (!symbolTable_.hasResource(rule.resource)) {
        reportError("Undefined resource: " + rule.resource, rule.location);
    }
}

void SemanticAnalyzer::checkConditionType(Expression* condition) {
    if (!condition) return;
    
    // Condition must be boolean expression
    AttributeType condType = checkExpressionType(*condition);
    if (condType != AttributeType::BOOL) {
        reportError("Condition must be a boolean expression", 
                   condition->location);
    }
}

AttributeType SemanticAnalyzer::checkExpressionType(Expression& expr) {
    // Type checking logic for expressions
    if (auto* binary = dynamic_cast<BinaryExpr*>(&expr)) {
        AttributeType leftType = checkExpressionType(*binary->left);
        AttributeType rightType = checkExpressionType(*binary->right);
        
        switch (binary->op) {
            case BinaryExpr::Operator::AND:
            case BinaryExpr::Operator::OR:
                if (leftType != AttributeType::BOOL || 
                    rightType != AttributeType::BOOL) {
                    reportError("Logical operators require boolean operands", 
                               expr.location);
                }
                return AttributeType::BOOL;
            
            case BinaryExpr::Operator::EQUALS:
            case BinaryExpr::Operator::NOT_EQUALS:
                if (leftType != rightType) {
                    reportError("Type mismatch in comparison", expr.location);
                }
                return AttributeType::BOOL;
            
            case BinaryExpr::Operator::LESS_THAN:
            case BinaryExpr::Operator::GREATER_THAN:
            case BinaryExpr::Operator::LESS_EQUAL:
            case BinaryExpr::Operator::GREATER_EQUAL:
                if (leftType != AttributeType::INT || 
                    rightType != AttributeType::INT) {
                    reportError("Relational operators require integer operands", 
                               expr.location);
                }
                return AttributeType::BOOL;
        }
    }
    else if (auto* unary = dynamic_cast<UnaryExpr*>(&expr)) {
        AttributeType operandType = checkExpressionType(*unary->operand);
        if (unary->op == UnaryExpr::Operator::NOT) {
            if (operandType != AttributeType::BOOL) {
                reportError("NOT operator requires boolean operand", 
                           expr.location);
            }
            return AttributeType::BOOL;
        }
    }
    else if (auto* attr = dynamic_cast<AttributeExpr*>(&expr)) {
        if (symbolTable_.hasAttribute(attr->namespace_, attr->name)) {
            return symbolTable_.getAttribute(attr->namespace_, attr->name).type;
        } else {
            reportError("Undefined attribute: " + attr->namespace_ + 
                       "." + attr->name, expr.location);
            return AttributeType::BOOL; // Default to avoid cascading errors
        }
    }
    else if (auto* lit = dynamic_cast<LiteralExpr*>(&expr)) {
        switch (lit->type) {
            case LiteralExpr::Type::INTEGER: return AttributeType::INT;
            case LiteralExpr::Type::STRING: return AttributeType::STRING;
            case LiteralExpr::Type::BOOLEAN: return AttributeType::BOOL;
        }
    }
    
    return AttributeType::BOOL;
}

void SemanticAnalyzer::checkRoleHierarchyCycles() {
    // Use DFS to detect cycles in role hierarchy
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    for (const auto& [roleName, roleSymbol] : symbolTable_.getRoles()) {
        if (visited.find(roleName) == visited.end()) {
            if (detectCycleDFS(roleName, visited, recursionStack)) {
                reportError("Cyclic role inheritance detected involving role: " + 
                           roleName, roleSymbol.location);
            }
        }
    }
}

bool SemanticAnalyzer::detectCycleDFS(
    const std::string& role,
    std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& recursionStack) {
    
    visited.insert(role);
    recursionStack.insert(role);
    
    if (symbolTable_.hasRole(role)) {
        const auto& roleSymbol = symbolTable_.getRole(role);
        if (!roleSymbol.parent.empty()) {
            if (recursionStack.find(roleSymbol.parent) != recursionStack.end()) {
                return true; // Cycle detected
            }
            
            if (visited.find(roleSymbol.parent) == visited.end()) {
                if (detectCycleDFS(roleSymbol.parent, visited, recursionStack)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(role);
    return false;
}

void SemanticAnalyzer::visit(BinaryExpr& node) {
    // Type checking handled in checkExpressionType
}

void SemanticAnalyzer::visit(UnaryExpr& node) {
    // Type checking handled in checkExpressionType
}

void SemanticAnalyzer::visit(AttributeExpr& node) {
    // Type checking handled in checkExpressionType
}

void SemanticAnalyzer::visit(LiteralExpr& node) {
    // Literals are always valid
}

void SemanticAnalyzer::reportError(const std::string& message, 
                                  const SourceLocation& location) {
    issues_.emplace_back(SemanticIssue::Severity::ERROR, message, location);
}

void SemanticAnalyzer::reportWarning(const std::string& message, 
                                    const SourceLocation& location) {
    issues_.emplace_back(SemanticIssue::Severity::WARNING, message, location);
}