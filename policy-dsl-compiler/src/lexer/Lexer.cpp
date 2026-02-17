#include "lexer/Lexer.h"
#include "utils/Error.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& source, const std::string& filename)
    : source_(source), filename_(filename), current_(0), line_(1), column_(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        Token token = scanToken();
        if (token.getType() != TokenType::INVALID) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(makeToken(TokenType::END_OF_FILE, ""));
    return tokens;
}

Token Lexer::scanToken() {
    char c = advance();
    
    // Single character tokens
    switch (c) {
        case '{': return makeToken(TokenType::LBRACE, "{");
        case '}': return makeToken(TokenType::RBRACE, "}");
        case ':': return makeToken(TokenType::COLON, ":");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        case '.': return makeToken(TokenType::DOT, ".");
        case '(': return makeToken(TokenType::LPAREN, "(");
        case ')': return makeToken(TokenType::RPAREN, ")");
        
        // Two character tokens
        case '<':
            if (match('=')) return makeToken(TokenType::LESS_EQUAL, "<=");
            return makeToken(TokenType::LESS_THAN, "<");
        
        case '>':
            if (match('=')) return makeToken(TokenType::GREATER_EQUAL, ">=");
            return makeToken(TokenType::GREATER_THAN, ">");
        
        case '=':
            if (match('=')) return makeToken(TokenType::EQUALS, "==");
            throw LexicalError("Expected '==' but got '='", currentLocation());
        
        case '!':
            if (match('=')) return makeToken(TokenType::NOT_EQUALS, "!=");
            throw LexicalError("Expected '!=' but got '!'", currentLocation());
        
        case '/':
            if (match('/')) {
                skipSingleLineComment();
                return scanToken(); // Continue scanning
            } else if (match('*')) {
                skipMultiLineComment();
                return scanToken();
            }
            throw LexicalError("Unexpected character '/'", currentLocation());
        
        case '"':
            return string();
        
        default:
            if (std::isdigit(c)) {
                current_--; column_--;
                return number();
            }
            if (std::isalpha(c) || c == '_') {
                current_--; column_--;
                return identifier();
            }
            throw LexicalError(std::string("Unexpected character: ") + c, 
                             currentLocation());
    }
}

Token Lexer::identifier() {
    size_t start = current_;
    int startCol = column_;
    
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        advance();
    }
    
    std::string text = source_.substr(start, current_ - start);
    
    // Check if it's a keyword
    if (TokenTypeHelper::isKeyword(text)) {
        return Token(TokenTypeHelper::getKeywordType(text), text, 
                    SourceLocation(filename_, line_, startCol));
    }
    
    return Token(TokenType::IDENTIFIER, text, 
                SourceLocation(filename_, line_, startCol));
}

Token Lexer::number() {
    size_t start = current_;
    int startCol = column_;
    
    while (!isAtEnd() && std::isdigit(peek())) {
        advance();
    }
    
    std::string num = source_.substr(start, current_ - start);
    return Token(TokenType::INTEGER_LITERAL, num, 
                SourceLocation(filename_, line_, startCol));
}

Token Lexer::string() {
    size_t start = current_;
    int startCol = column_ - 1; // Account for opening quote
    
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            line_++;
            column_ = 0;
        }
        advance();
    }
    
    if (isAtEnd()) {
        throw LexicalError("Unterminated string", currentLocation());
    }
    
    advance(); // Closing quote
    
    // Extract string without quotes
    std::string value = source_.substr(start, current_ - start - 1);
    return Token(TokenType::STRING_LITERAL, value, 
                SourceLocation(filename_, line_, startCol));
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line_++;
                column_ = 0;
                advance();
                break;
            default:
                return;
        }
    }
}

void Lexer::skipSingleLineComment() {
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void Lexer::skipMultiLineComment() {
    while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
            advance(); // *
            advance(); // /
            return;
        }
        if (peek() == '\n') {
            line_++;
            column_ = 0;
        }
        advance();
    }
    throw LexicalError("Unterminated multi-line comment", currentLocation());
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

char Lexer::advance() {
    column_++;
    return source_[current_++];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    column_++;
    return true;
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token(type, lexeme, currentLocation());
}

SourceLocation Lexer::currentLocation() const {
    return SourceLocation(filename_, line_, column_);
}