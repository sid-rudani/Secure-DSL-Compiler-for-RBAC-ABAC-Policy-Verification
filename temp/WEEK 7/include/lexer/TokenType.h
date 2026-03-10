#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <string>
#include <unordered_map>

enum class TokenType {
    // Keywords
    POLICY, USER, ROLE, RESOURCE, ACTION, ATTRIBUTE,
    RULE, EFFECT, ALLOW, DENY, SUBJECT, WHEN,
    
    // Types
    INT_TYPE, STRING_TYPE, BOOL_TYPE, TIME_TYPE,
    
    // Operators
    LESS_THAN,          // <
    EQUALS,             // ==
    NOT_EQUALS,         // !=
    GREATER_THAN,       // >
    LESS_EQUAL,         // <=
    GREATER_EQUAL,      // >=
    
    // Logical
    AND, OR, NOT,
    
    // Delimiters
    LBRACE,             // {
    RBRACE,             // }
    COLON,              // :
    SEMICOLON,          // ;
    DOT,                // .
    LPAREN,             // (
    RPAREN,             // )
    
    // Literals
    IDENTIFIER,
    INTEGER_LITERAL,
    STRING_LITERAL,
    TRUE_LITERAL,
    FALSE_LITERAL,
    
    // Special
    END_OF_FILE,
    INVALID
};

class TokenTypeHelper {
public:
    static std::string toString(TokenType type);
    static bool isKeyword(const std::string& str);
    static TokenType getKeywordType(const std::string& str);
    static const std::unordered_map<std::string, TokenType>& getKeywords();
    
private:
    static const std::unordered_map<std::string, TokenType> keywords;
};

#endif // TOKEN_TYPE_H
