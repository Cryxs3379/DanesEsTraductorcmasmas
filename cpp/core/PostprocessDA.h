#pragma once

#include <string>

namespace traductor {

/**
 * Post-processing for Danish translation output.
 * Handles date formats, numbers, and formal style transformations.
 */
class PostprocessDA {
public:
    PostprocessDA() = default;
    ~PostprocessDA() = default;

    // Main post-processing function
    static std::string process(const std::string& text, bool formal = false);
    
    // Individual processing functions
    static std::string normalizeNumbers(const std::string& text);
    static std::string normalizeDates(const std::string& text);
    static std::string formalize(const std::string& text);
    
    // Clean up multiple spaces and trim
    static std::string cleanText(const std::string& text);

private:
    // Helper functions for specific transformations
    static std::string convertDateToDanish(const std::string& text);
    static std::string applyFormalStyle(const std::string& text);
};

} // namespace traductor
