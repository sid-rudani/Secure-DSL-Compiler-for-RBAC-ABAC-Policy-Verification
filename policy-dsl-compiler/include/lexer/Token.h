#ifndef TOKEN_H
#define TOKEN_H

#include "TokenType.h"
#include "../utils/SourceLocation.h"
#include <string>
#include <memory>

class Token {
public:
    Token(TokenType type, const std::string& lexeme, 
          const SourceLocation& location)
        : type_(type), lexeme_(lexeme), location_(location) {}
    
    TokenType getType() const { return type_; }
    const std::string& getLexeme() const { return lexeme_; }
    const SourceLocation& getLocation() const { return location_; }
    
    std::string toString() const;
    
private:
    TokenType type_;
    std::string lexeme_;
    SourceLocation location_;
};

#endif // TOKEN_H