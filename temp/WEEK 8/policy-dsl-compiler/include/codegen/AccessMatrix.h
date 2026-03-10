#ifndef ACCESS_MATRIX_H
#define ACCESS_MATRIX_H

#include <string>
#include <vector>
#include <unordered_map>

enum class AccessDecision {
    ALLOW,
    DENY
};

struct AccessEntry {
    std::string subject;
    std::string action;
    std::string resource;
    AccessDecision decision;
};

class AccessMatrix {
public:
    void addEntry(const std::string& subject, const std::string& action,
                 const std::string& resource, AccessDecision decision);
    
    AccessDecision query(const std::string& subject, const std::string& action,
                        const std::string& resource) const;
    
    const std::vector<AccessEntry>& getEntries() const { return entries_; }
    
    std::string toText() const;
    std::string toJSON() const;
    
private:
    std::vector<AccessEntry> entries_;
    
    std::string makeKey(const std::string& subject, const std::string& action,
                       const std::string& resource) const;
};

#endif // ACCESS_MATRIX_H