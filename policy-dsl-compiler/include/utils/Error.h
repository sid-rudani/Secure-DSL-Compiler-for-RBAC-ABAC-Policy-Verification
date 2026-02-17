#ifndef ERROR_H
#define ERROR_H

#include "SourceLocation.h"
#include <stdexcept>
#include <string>
#include <vector>

enum class ErrorType {
    LEXICAL_ERROR,
    SYNTAX_ERROR,
    SEMANTIC_ERROR,
    TYPE_ERROR,
    CONFLICT_ERROR,
    WARNING
};

class Error {
public:
    ErrorType type;
    std::string message;
    SourceLocation location;
    
    Error(ErrorType type, const std::string& message, const SourceLocation& location);
};

class ErrorHandler {
public:
    void addError(const Error& error);
    void addError(ErrorType type, const std::string& message, const SourceLocation& location);
    bool hasErrors() const;
    const std::vector<Error>& getErrors() const;
    void printErrors() const;
    void clear();
    
private:
    std::vector<Error> errors;
};

// Exception classes
class CompilerError : public std::runtime_error {
public:
    CompilerError(const std::string& message, const SourceLocation& location)
        : std::runtime_error(formatMessage(message, location)), 
          location_(location) {}
    
    const SourceLocation& getLocation() const { return location_; }
    
private:
    SourceLocation location_;
    
    static std::string formatMessage(const std::string& msg, 
                                     const SourceLocation& loc) {
        return "[ERROR] " + loc.toString() + ": " + msg;
    }
};

class LexicalError : public CompilerError {
public:
    using CompilerError::CompilerError;
};

class SyntaxError : public CompilerError {
public:
    using CompilerError::CompilerError;
};

class SemanticError : public CompilerError {
public:
    using CompilerError::CompilerError;
};

#endif // ERROR_H
