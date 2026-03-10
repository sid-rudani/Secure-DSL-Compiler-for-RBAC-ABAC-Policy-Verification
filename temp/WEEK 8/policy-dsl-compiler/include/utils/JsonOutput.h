#ifndef JSON_OUTPUT_H
#define JSON_OUTPUT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../lexer/Token.h"
#include "../lexer/TokenType.h"
#include "../parser/AST.h"

class JsonOutput {
public:
    static std::string tokenToJson(const std::vector<Token>& tokens);
    static std::string policyToJson(const PolicyNode& policy);
    
private:
    static std::string escapeString(const std::string& str);
};

#endif // JSON_OUTPUT_H
