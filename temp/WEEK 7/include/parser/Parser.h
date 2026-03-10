#ifndef PARSER_H
#define PARSER_H

#include "ASTNodes.h"
#include "../lexer/Token.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    
    std::unique_ptr<PolicyNode> parse();
    
private:
    std::vector<Token> tokens_;
    size_t current_;
    
    // Helper methods
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
    
    void synchronize();
    
    // Grammar rules (recursive descent)
    std::unique_ptr<PolicyNode> parsePolicy();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<UserDecl> parseUserDecl();
    std::unique_ptr<RoleDecl> parseRoleDecl();
    std::unique_ptr<ResourceDecl> parseResourceDecl();
    std::unique_ptr<ActionDecl> parseActionDecl();
    std::unique_ptr<AttributeDecl> parseAttributeDecl();
    std::unique_ptr<Rule> parseRule();
    
    // Expression parsing
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseOrExpression();
    std::unique_ptr<Expression> parseAndExpression();
    std::unique_ptr<Expression> parseUnaryExpression();
    std::unique_ptr<Expression> parseComparisonExpression();
    std::unique_ptr<Expression> parsePrimaryExpression();
};

#endif // PARSER_H