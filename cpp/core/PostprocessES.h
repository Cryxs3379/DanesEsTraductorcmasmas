#pragma once

#include <string>

namespace traductor {

/**
 * Post-processing for Spanish translation output.
 * Handles date formats and number normalization.
 */
class PostprocessES {
public:
    PostprocessES() = default;
    ~PostprocessES() = default;

    // Main post-processing function
    static std::string process(const std::string& text);
    
    // Individual processing functions
    static std::string normalizeDates(const std::string& text);
    static std::string normalizeNumbers(const std::string& text);
    
    // Clean up multiple spaces and trim
    static std::string cleanText(const std::string& text);

private:
    // Helper functions
    static std::string convertDateToSpanish(const std::string& text);
};

} // namespace traductor
