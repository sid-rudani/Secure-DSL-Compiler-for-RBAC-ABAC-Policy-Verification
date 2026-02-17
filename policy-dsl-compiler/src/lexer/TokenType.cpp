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
        // ... add all cases
        default: return "UNKNOWN";
    }
}