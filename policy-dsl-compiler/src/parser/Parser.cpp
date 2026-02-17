#include "parser/Parser.h"
#include "utils/Error.h"

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens_(tokens), current_(0) {}

std::unique_ptr<PolicyNode> Parser::parse() {
    try {
        return parsePolicy();
    } catch (const SyntaxError& e) {
        // Error already reported
        throw;
    }
}

std::unique_ptr<PolicyNode> Parser::parsePolicy() {
    consume(TokenType::POLICY, "Expected 'policy' keyword");
    
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected policy name");
    auto policy = std::make_unique<PolicyNode>();
    policy->name = nameToken.getLexeme();
    policy->location = nameToken.getLocation();
    
    consume(TokenType::LBRACE, "Expected '{' after policy name");
    
    // Parse declarations and rules
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::RULE)) {
            policy->rules.push_back(parseRule());
        } else {
            policy->declarations.push_back(parseDeclaration());
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after policy body");
    
    return policy;
}

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    if (match(TokenType::USER)) return parseUserDecl();
    if (match(TokenType::ROLE)) return parseRoleDecl();
    if (match(TokenType::RESOURCE)) return parseResourceDecl();
    if (match(TokenType::ACTION)) return parseActionDecl();
    if (match(TokenType::ATTRIBUTE)) return parseAttributeDecl();
    
    throw SyntaxError("Expected declaration", peek().getLocation());
}

std::unique_ptr<UserDecl> Parser::parseUserDecl() {
    Token id = consume(TokenType::IDENTIFIER, "Expected user identifier");
    consume(TokenType::SEMICOLON, "Expected ';' after user declaration");
    
    auto decl = std::make_unique<UserDecl>(id.getLexeme());
    decl->location = id.getLocation();
    return decl;
}

std::unique_ptr<RoleDecl> Parser::parseRoleDecl() {
    Token id = consume(TokenType::IDENTIFIER, "Expected role identifier");
    
    std::string parent = "";
    if (match(TokenType::LESS_THAN)) {
        Token parentToken = consume(TokenType::IDENTIFIER, 
                                   "Expected parent role identifier");
        parent = parentToken.getLexeme();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after role declaration");
    
    auto decl = std::make_unique<RoleDecl>(id.getLexeme(), parent);
    decl->location = id.getLocation();
    return decl;
}

std::unique_ptr<ResourceDecl> Parser::parseResourceDecl() {
    Token id = consume(TokenType::IDENTIFIER, "Expected resource identifier");
    consume(TokenType::SEMICOLON, "Expected ';' after resource declaration");
    
    auto decl = std::make_unique<ResourceDecl>(id.getLexeme());
    decl->location = id.getLocation();
    return decl;
}

std::unique_ptr<ActionDecl> Parser::parseActionDecl() {
    Token id = consume(TokenType::IDENTIFIER, "Expected action identifier");
    consume(TokenType::SEMICOLON, "Expected ';' after action declaration");
    
    auto decl = std::make_unique<ActionDecl>(id.getLexeme());
    decl->location = id.getLocation();
    return decl;
}

std::unique_ptr<AttributeDecl> Parser::parseAttributeDecl() {
    Token namespace_token = consume(TokenType::IDENTIFIER, "Expected namespace");
    consume(TokenType::DOT, "Expected '.' after namespace");
    Token name_token = consume(TokenType::IDENTIFIER, "Expected attribute name");
    consume(TokenType::COLON, "Expected ':' after attribute name");
    
    AttributeType type;
    if (match(TokenType::INT_TYPE)) {
        type = AttributeType::INT;
    } else if (match(TokenType::STRING_TYPE)) {
        type = AttributeType::STRING;
    } else if (match(TokenType::BOOL_TYPE)) {
        type = AttributeType::BOOL;
    } else if (match(TokenType::TIME_TYPE)) {
        type = AttributeType::TIME;
    } else {
        throw SyntaxError("Expected type annotation", peek().getLocation());
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after attribute declaration");
    
    auto decl = std::make_unique<AttributeDecl>(namespace_token.getLexeme(), 
                                                name_token.getLexeme(), type);
    decl->location = name_token.getLocation();
    return decl;
}

std::unique_ptr<Rule> Parser::parseRule() {
    consume(TokenType::RULE, "Expected 'rule' keyword");
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected rule name");
    
    auto rule = std::make_unique<Rule>();
    rule->name = nameToken.getLexeme();
    rule->location = nameToken.getLocation();
    
    consume(TokenType::LBRACE, "Expected '{' after rule name");
    
    // Parse effect
    consume(TokenType::EFFECT, "Expected 'effect' field");
    consume(TokenType::COLON, "Expected ':' after 'effect'");
    if (match(TokenType::ALLOW)) {
        rule->effect = Rule::Effect::ALLOW;
    } else if (match(TokenType::DENY)) {
        rule->effect = Rule::Effect::DENY;
    } else {
        throw SyntaxError("Expected 'allow' or 'deny'", peek().getLocation());
    }
    
    // Parse subject
    consume(TokenType::SUBJECT, "Expected 'subject' field");
    consume(TokenType::COLON, "Expected ':' after 'subject'");
    if (match(TokenType::USER)) {
        rule->subjectType = Rule::SubjectType::USER;
    } else if (match(TokenType::ROLE)) {
        rule->subjectType = Rule::SubjectType::ROLE;
    } else {
        throw SyntaxError("Expected 'user' or 'role'", peek().getLocation());
    }
    Token subjectToken = consume(TokenType::IDENTIFIER, "Expected subject identifier");
    rule->subject = subjectToken.getLexeme();
    
    // Parse action
    consume(TokenType::ACTION, "Expected 'action' field");
    consume(TokenType::COLON, "Expected ':' after 'action'");
    Token actionToken = consume(TokenType::IDENTIFIER, "Expected action identifier");
    rule->action = actionToken.getLexeme();
    
    // Parse resource
    consume(TokenType::RESOURCE, "Expected 'resource' field");
    consume(TokenType::COLON, "Expected ':' after 'resource'");
    Token resourceToken = consume(TokenType::IDENTIFIER, "Expected resource identifier");
    rule->resource = resourceToken.getLexeme();
    
    // Optional condition
    if (match(TokenType::WHEN)) {
        consume(TokenType::COLON, "Expected ':' after 'when'");
        rule->condition = parseExpression();
    }
    
    consume(TokenType::RBRACE, "Expected '}' after rule body");
    
    return rule;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseOrExpression();
}

std::unique_ptr<Expression> Parser::parseOrExpression() {
    auto expr = parseAndExpression();
    
    while (match(TokenType::OR)) {
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = BinaryExpr::Operator::OR;
        binary->right = parseAndExpression();
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseAndExpression() {
    auto expr = parseUnaryExpression();
    
    while (match(TokenType::AND)) {
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = BinaryExpr::Operator::AND;
        binary->right = parseUnaryExpression();
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseUnaryExpression() {
    if (match(TokenType::NOT)) {
        auto unary = std::make_unique<UnaryExpr>();
        unary->op = UnaryExpr::Operator::NOT;
        unary->operand = parseUnaryExpression();
        return unary;
    }
    
    return parseComparisonExpression();
}

std::unique_ptr<Expression> Parser::parseComparisonExpression() {
    auto expr = parsePrimaryExpression();
    
    if (match(TokenType::EQUALS)) {
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = BinaryExpr::Operator::EQUALS;
        binary->right = parsePrimaryExpression();
        return binary;
    } else if (match(TokenType::NOT_EQUALS)) {
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = BinaryExpr::Operator::NOT_EQUALS;
        binary->right = parsePrimaryExpression();
        return binary;
    } else if (match(TokenType::LESS_THAN)) {
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = BinaryExpr::Operator::LESS_THAN;
        binary->right = parsePrimaryExpression();
        return binary;
    } else if (match(TokenType::GREATER_THAN)) {
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = BinaryExpr::Operator::GREATER_THAN;
        binary->right = parsePrimaryExpression();
        return binary;
    } else if (match(TokenType::LESS_EQUAL)) {
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = BinaryExpr::Operator::LESS_EQUAL;
        binary->right = parsePrimaryExpression();
        return binary;
    } else if (match(TokenType::GREATER_EQUAL)) {
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = BinaryExpr::Operator::GREATER_EQUAL;
        binary->right = parsePrimaryExpression();
        return binary;
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parsePrimaryExpression() {
    if (match(TokenType::INTEGER_LITERAL)) {
        auto expr = std::make_unique<LiteralExpr>(LiteralExpr::Type::INTEGER, 
                                                   previous().getLexeme());
        return expr;
    }
    
    if (match(TokenType::STRING_LITERAL)) {
        auto expr = std::make_unique<LiteralExpr>(LiteralExpr::Type::STRING,
                                                   previous().getLexeme());
        return expr;
    }
    
    if (match(TokenType::TRUE_LITERAL)) {
        auto expr = std::make_unique<LiteralExpr>(LiteralExpr::Type::BOOLEAN, "true");
        return expr;
    }
    
    if (match(TokenType::FALSE_LITERAL)) {
        auto expr = std::make_unique<LiteralExpr>(LiteralExpr::Type::BOOLEAN, "false");
        return expr;
    }
    
    if (match(TokenType::IDENTIFIER)) {
        std::string name = previous().getLexeme();
        
        // Check for attribute access (namespace.name)
        if (match(TokenType::DOT)) {
            Token attrName = consume(TokenType::IDENTIFIER, "Expected attribute name");
            auto expr = std::make_unique<AttributeExpr>(name, attrName.getLexeme());
            return expr;
        }
        
        // Just an identifier - treat as attribute in current context
        auto expr = std::make_unique<AttributeExpr>("", name);
        return expr;
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw SyntaxError("Expected expression", peek().getLocation());
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw SyntaxError(message, peek().getLocation());
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().getType() == type;
}

const Token& Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().getType() == TokenType::END_OF_FILE;
}

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().getType() == TokenType::SEMICOLON) return;
        
        switch (peek().getType()) {
            case TokenType::POLICY:
            case TokenType::USER:
            case TokenType::ROLE:
            case TokenType::RESOURCE:
            case TokenType::ACTION:
            case TokenType::RULE:
                return;
            default:
                break;
        }
        
        advance();
    }
}