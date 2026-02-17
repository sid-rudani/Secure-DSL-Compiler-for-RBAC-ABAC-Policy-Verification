#include "../../include/utils/JsonOutput.h"
#include <sstream>
#include <iomanip>

std::string JsonOutput::escapeString(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (c < 32) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
                    escaped += buf;
                } else {
                    escaped += c;
                }
        }
    }
    return escaped;
}

std::string JsonOutput::tokenToJson(const std::vector<Token>& tokens) {
    std::stringstream ss;
    ss << "{\n  \"tokens\": [\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        ss << "    {\n";
        ss << "      \"type\": \"" << TokenTypeHelper::toString(token.getType()) << "\",\n";
        ss << "      \"lexeme\": \"" << escapeString(token.getLexeme()) << "\",\n";
        ss << "      \"line\": " << token.getLocation().line << ",\n";
        ss << "      \"column\": " << token.getLocation().column << "\n";
        ss << "    }";
        if (i < tokens.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n}";
    return ss.str();
}

std::string JsonOutput::policyToJson(const PolicyNode& policy) {
    std::stringstream ss;
    ss << "{\n";
    ss << "    \"name\": \"" << policy.name << "\",\n";
    ss << "    \"declarations\": " << policy.declarations.size() << ",\n";
    ss << "    \"rules\": " << policy.rules.size() << "\n";
    ss << "  }";    return ss.str();
}