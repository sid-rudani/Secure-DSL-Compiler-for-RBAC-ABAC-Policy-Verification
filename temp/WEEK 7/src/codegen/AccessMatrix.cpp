#include "codegen/AccessMatrix.h"
#include <sstream>
#include <iomanip>

void AccessMatrix::addEntry(const std::string& subject, 
                           const std::string& action,
                           const std::string& resource, 
                           AccessDecision decision) {
    entries_.push_back({subject, action, resource, decision});
}

AccessDecision AccessMatrix::query(const std::string& subject,
                                  const std::string& action,
                                  const std::string& resource) const {
    std::string key = makeKey(subject, action, resource);
    
    for (const auto& entry : entries_) {
        if (makeKey(entry.subject, entry.action, entry.resource) == key) {
            return entry.decision;
        }
    }
    
    return AccessDecision::DENY; // Default deny
}

std::string AccessMatrix::toText() const {
    std::ostringstream oss;
    
    // Header
    oss << std::left 
        << std::setw(20) << "Subject" << " | "
        << std::setw(15) << "Action" << " | "
        << std::setw(20) << "Resource" << " | "
        << "Decision" << "\n";
    
    oss << std::string(80, '-') << "\n";
    
    // Entries
    for (const auto& entry : entries_) {
        oss << std::setw(20) << entry.subject << " | "
            << std::setw(15) << entry.action << " | "
            << std::setw(20) << entry.resource << " | "
            << (entry.decision == AccessDecision::ALLOW ? "ALLOW" : "DENY")
            << "\n";
    }
    
    return oss.str();
}

std::string AccessMatrix::toJSON() const {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"access_matrix\": [\n";
    
    for (size_t i = 0; i < entries_.size(); i++) {
        const auto& entry = entries_[i];
        
        oss << "    {\n";
        oss << "      \"subject\": \"" << entry.subject << "\",\n";
        oss << "      \"action\": \"" << entry.action << "\",\n";
        oss << "      \"resource\": \"" << entry.resource << "\",\n";
        oss << "      \"decision\": \"" 
            << (entry.decision == AccessDecision::ALLOW ? "ALLOW" : "DENY")
            << "\"\n";
        oss << "    }";
        
        if (i < entries_.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}\n";
    
    return oss.str();
}

std::string AccessMatrix::makeKey(const std::string& subject,
                                 const std::string& action,
                                 const std::string& resource) const {
    return subject + "|" + action + "|" + resource;
}