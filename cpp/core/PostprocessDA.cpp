#include "PostprocessDA.h"
#include <regex>
#include <algorithm>
#include <map>
#include <cctype>

namespace traductor {

std::string PostprocessDA::process(const std::string& text, bool formal) {
    if (text.empty()) {
        return text;
    }
    
    std::string result = text;
    
    // 1. Normalize numbers
    result = normalizeNumbers(result);
    
    // 2. Normalize dates
    result = normalizeDates(result);
    
    // 3. Apply formal style if requested
    if (formal) {
        result = formalize(result);
    }
    
    // 4. Final cleanup
    result = cleanText(result);
    
    return result;
}

std::string PostprocessDA::normalizeNumbers(const std::string& text) {
    // Danish uses comma as decimal separator, so generally numbers are already correct
    // Just ensure consistency - no major changes needed from Spanish format
    return text;
}

std::string PostprocessDA::normalizeDates(const std::string& text) {
    std::string result = text;
    
    // Convert dd/mm/yyyy or dd-mm-yyyy to dd.mm.yyyy (Danish preferred format)
    // Pattern: day/month/year or day-month-year
    std::regex dateSlashRegex(R"(\b(\d{1,2})/(\d{1,2})/(\d{4})\b)");
    result = std::regex_replace(result, dateSlashRegex, R"($1.$2.$3)");
    
    std::regex dateDashRegex(R"(\b(\d{1,2})-(\d{1,2})-(\d{4})\b)");
    result = std::regex_replace(result, dateDashRegex, R"($1.$2.$3)");
    
    return result;
}

std::string PostprocessDA::formalize(const std::string& text) {
    std::string result = text;
    
    // Formal salutations: "Hej" → "Kære" when followed by name/client reference
    std::regex hejRegex(R"(\bHej\s+([\w\s]+))");
    result = std::regex_replace(result, hejRegex, "Kære $1", std::regex_constants::match_flag_type::format_first_only);
    
    // Formal closings
    std::map<std::string, std::string> formalReplacements = {
        {R"(\bHilsen\b)", "Med venlig hilsen"},
        {R"(\bMvh\b)", "Med venlig hilsen"},
        {R"(\bVenlig hilsen\b)", "Med venlig hilsen"},
        
        // Formal treatment (du → De, dig → Dem, etc.)
        {R"(\bdu\b)", "De"},
        {R"(\bdig\b)", "Dem"},
        {R"(\bdin\b)", "Deres"},
        {R"(\bdine\b)", "Deres"}
    };
    
    for (const auto& [pattern, replacement] : formalReplacements) {
        std::regex regex(pattern, std::regex_constants::icase);
        result = std::regex_replace(result, regex, replacement);
    }
    
    // Capitalize "De/Dem/Deres" at beginning of sentences
    std::regex sentenceStartRegex(R"(\. (de|dem|deres)\b)");
    std::string replacement = ". De"; // Simplified replacement for now
    result = std::regex_replace(result, sentenceStartRegex, replacement);
    
    return result;
}

std::string PostprocessDA::cleanText(const std::string& text) {
    std::string result = text;
    
    // Replace multiple spaces with single space
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // Trim leading and trailing whitespace
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    
    return result;
}

} // namespace traductor
