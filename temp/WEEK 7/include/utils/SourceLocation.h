#ifndef SOURCE_LOCATION_H
#define SOURCE_LOCATION_H

#include <string>

struct SourceLocation {
    std::string filename;
    int line;
    int column;
    
    SourceLocation(const std::string& file = "", int ln = 1, int col = 1)
        : filename(file), line(ln), column(col) {}
    
    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

#endif // SOURCE_LOCATION_H