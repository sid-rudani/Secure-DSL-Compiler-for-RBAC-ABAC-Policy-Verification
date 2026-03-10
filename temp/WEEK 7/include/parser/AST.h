#ifndef AST_NODES_H
#define AST_NODES_H

#include <string>
#include <vector>
#include <memory>
#include "../utils/SourceLocation.h"

// Forward declarations
class ASTVisitor;

// Base node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    
    SourceLocation location;
};

// Policy (root)
class PolicyNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> declarations;
    std::vector<std::unique_ptr<ASTNode>> rules;
    
    void accept(ASTVisitor& visitor) override;
};

// Declarations
class UserDecl : public ASTNode {
public:
    std::string identifier;
    
    explicit UserDecl(const std::string& id) : identifier(id) {}
    void accept(ASTVisitor& visitor) override;
};

class RoleDecl : public ASTNode {
public:
    std::string identifier;
    std::string parent; // Empty if no parent
    
    RoleDecl(const std::string& id, const std::string& par = "")
        : identifier(id), parent(par) {}
    
    void accept(ASTVisitor& visitor) override;
};

class ResourceDecl : public ASTNode {
public:
    std::string identifier;
    
    explicit ResourceDecl(const std::string& id) : identifier(id) {}
    void accept(ASTVisitor& visitor) override;
};

class ActionDecl : public ASTNode {
public:
    std::string identifier;
    
    explicit ActionDecl(const std::string& id) : identifier(id) {}
    void accept(ASTVisitor& visitor) override;
};

enum class AttributeType {
    INT, STRING, BOOL, TIME
};

class AttributeDecl : public ASTNode {
public:
    std::string namespace_; // user, resource, env
    std::string name;
    AttributeType type;
    
    AttributeDecl(const std::string& ns, const std::string& n, AttributeType t)
        : namespace_(ns), name(n), type(t) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Expressions
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

class BinaryExpr : public Expression {
public:
    enum class Operator {
        EQUALS, NOT_EQUALS,
        LESS_THAN, GREATER_THAN, LESS_EQUAL, GREATER_EQUAL,
        AND, OR
    };
    
    std::unique_ptr<Expression> left;
    Operator op;
    std::unique_ptr<Expression> right;
    
    void accept(ASTVisitor& visitor) override;
};

class UnaryExpr : public Expression {
public:
    enum class Operator { NOT };
    
    Operator op;
    std::unique_ptr<Expression> operand;
    
    void accept(ASTVisitor& visitor) override;
};

class AttributeExpr : public Expression {
public:
    std::string namespace_;
    std::string name;
    
    AttributeExpr(const std::string& ns, const std::string& n)
        : namespace_(ns), name(n) {}
    
    void accept(ASTVisitor& visitor) override;
};

class LiteralExpr : public Expression {
public:
    enum class Type { INTEGER, STRING, BOOLEAN };
    
    Type type;
    std::string value;
    
    LiteralExpr(Type t, const std::string& v) : type(t), value(v) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Rule
class Rule : public ASTNode {
public:
    enum class Effect { ALLOW, DENY };
    enum class SubjectType { USER, ROLE };
    
    std::string name;
    Effect effect;
    SubjectType subjectType;
    std::string subject;
    std::string action;
    std::string resource;
    std::unique_ptr<Expression> condition; // nullable
    
    void accept(ASTVisitor& visitor) override;
};

// Visitor pattern for AST traversal
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(PolicyNode& node) = 0;
    virtual void visit(UserDecl& node) = 0;
    virtual void visit(RoleDecl& node) = 0;
    virtual void visit(ResourceDecl& node) = 0;
    virtual void visit(ActionDecl& node) = 0;
    virtual void visit(AttributeDecl& node) = 0;
    virtual void visit(Rule& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(AttributeExpr& node) = 0;
    virtual void visit(LiteralExpr& node) = 0;
};

#endif // AST_NODES_H