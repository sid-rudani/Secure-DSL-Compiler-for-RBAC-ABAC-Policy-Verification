#include "lexer/TokenType.h"

const std::unordered_map<std::string, TokenType> TokenTypeHelper::keywords = {
    {"policy", TokenType::POLICY},
    {"user", TokenType::USER},
    {"role", TokenType::ROLE},
    {"resource", TokenType::RESOURCE},
    {"action", TokenType::ACTION},
    {"attribute", TokenType::ATTRIBUTE},
    {"rule", TokenType::RULE},
    {"effect", TokenType::EFFECT},
    {"allow", TokenType::ALLOW},
    {"deny", TokenType::DENY},
    {"subject", TokenType::SUBJECT},
    {"when", TokenType::WHEN},
    {"int", TokenType::INT_TYPE},
    {"string", TokenType::STRING_TYPE},
    {"bool", TokenType::BOOL_TYPE},
    {"time", TokenType::TIME_TYPE},
    {"AND", TokenType::AND},
    {"OR", TokenType::OR},
    {"NOT", TokenType::NOT},
    {"true", TokenType::TRUE_LITERAL},
    {"false", TokenType::FALSE_LITERAL}
};

bool TokenTypeHelper::isKeyword(const std::string& str) {
    return keywords.find(str) != keywords.end();
}

TokenType TokenTypeHelper::getKeywordType(const std::string& str) {
    auto it = keywords.find(str);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

std::string TokenTypeHelper::toString(TokenType type) {
    switch (type) {
        case TokenType::POLICY: return "POLICY";
        case TokenType::USER: return "USER";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::ROLE: return "ROLE";
        case TokenType::RESOURCE: return "RESOURCE";
        case TokenType::ACTION: return "ACTION";
        case TokenType::ATTRIBUTE: return "ATTRIBUTE";
        case TokenType::RULE: return "RULE";
        case TokenType::EFFECT: return "EFFECT";
        case TokenType::ALLOW: return "ALLOW";
        case TokenType::DENY: return "DENY";
        case TokenType::SUBJECT: return "SUBJECT";
        case TokenType::WHEN: return "WHEN";

        case TokenType::INT_TYPE: return "INT_TYPE";
        case TokenType::STRING_TYPE: return "STRING_TYPE";
        case TokenType::BOOL_TYPE: return "BOOL_TYPE";
        case TokenType::TIME_TYPE: return "TIME_TYPE";

        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::NOT_EQUALS: return "NOT_EQUALS";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";

        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";

        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::DOT: return "DOT";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";

        case TokenType::TRUE_LITERAL: return "TRUE_LITERAL";
        case TokenType::FALSE_LITERAL: return "FALSE_LITERAL";

        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::INVALID: return "INVALID";

        default: return "UNKNOWN";
    }
}