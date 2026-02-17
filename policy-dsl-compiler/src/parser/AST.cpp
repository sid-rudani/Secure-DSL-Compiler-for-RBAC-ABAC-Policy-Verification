#include "../../include/parser/AST.h"

void PolicyNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UserDecl::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void RoleDecl::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ResourceDecl::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ActionDecl::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void AttributeDecl::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void Rule::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void BinaryExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void AttributeExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void LiteralExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);}