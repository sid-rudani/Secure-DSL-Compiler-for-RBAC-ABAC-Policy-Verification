#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <vector>
#include <string>
#include <memory>

class Lexer {
public:

    explicit Lexer(const std::string& source, const std::string& filename = "");
    std::vector<Token> tokenize();
    
private:
    std::string source_;
    std::string filename_;
    size_t current_;
    int line_;
    int column_;
    
    // Helper methods
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    
    Token scanToken();
    Token identifier();
    Token number();
    Token string();
    
    Token makeToken(TokenType type, const std::string& lexeme);
    SourceLocation currentLocation() const;
};

#endif // LEXER_H