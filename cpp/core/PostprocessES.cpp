#include "PostprocessES.h"
#include <regex>

namespace traductor {

std::string PostprocessES::process(const std::string& text) {
    if (text.empty()) {
        return text;
    }
    
    std::string result = text;
    
    // 1. Normalize dates
    result = normalizeDates(result);
    
    // 2. Normalize numbers  
    result = normalizeNumbers(result);
    
    // 3. Final cleanup
    result = cleanText(result);
    
    return result;
}

std::string PostprocessES::normalizeDates(const std::string& text) {
    std::string result = text;
    
    // Convert dd.mm.yyyy (Danish format) to dd/mm/yyyy (Spanish format)
    std::regex dateDotRegex(R"(\b(\d{1,2})\.(\d{1,2})\.(\d{4})\b)");
    result = std::regex_replace(result, dateDotRegex, R"($1/$2/$3)");
    
    return result;
}

std::string PostprocessES::normalizeNumbers(const std::string& text) {
    // Spanish number formatting is similar to Danish (comma decimal, dot thousands)
    // Generally no changes needed, just return as-is
    return text;
}

std::string PostprocessES::cleanText(const std::string& text) {
    std::string result = text;
    
    // Replace multiple spaces with single space
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // Trim leading and trailing whitespace
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    
    return result;
}

} // namespace traductor
