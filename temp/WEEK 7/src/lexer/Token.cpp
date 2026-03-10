#include "../../include/lexer/Token.h"

std::string Token::toString() const {
    return "[TOKEN] " + lexeme_;
}
