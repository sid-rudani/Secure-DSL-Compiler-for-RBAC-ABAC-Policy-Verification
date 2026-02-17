#include "utils/Error.h"
#include <iostream>

Error::Error(ErrorType type, const std::string& message, const SourceLocation& location)
    : type(type), message(message), location(location) {}

void ErrorHandler::addError(const Error& error) {
    errors.push_back(error);
}

void ErrorHandler::addError(ErrorType type, const std::string& message, const SourceLocation& location) {
    errors.push_back(Error(type, message, location));
}

bool ErrorHandler::hasErrors() const {
    return !errors.empty();
}

const std::vector<Error>& ErrorHandler::getErrors() const {
    return errors;
}

void ErrorHandler::printErrors() const {
    for (const auto& error : errors) {
        std::string errorTypeStr;
        switch (error.type) {
            case ErrorType::LEXICAL_ERROR: errorTypeStr = "LEXICAL ERROR"; break;
            case ErrorType::SYNTAX_ERROR: errorTypeStr = "SYNTAX ERROR"; break;
            case ErrorType::SEMANTIC_ERROR: errorTypeStr = "SEMANTIC ERROR"; break;
            case ErrorType::TYPE_ERROR: errorTypeStr = "TYPE ERROR"; break;
            case ErrorType::CONFLICT_ERROR: errorTypeStr = "CONFLICT ERROR"; break;
            case ErrorType::WARNING: errorTypeStr = "WARNING"; break;
        }
        std::cerr << "[" << errorTypeStr << "] " << error.location.toString() << ": " 
                  << error.message << std::endl;
    }
}

void ErrorHandler::clear() {
    errors.clear();
}