#include "Segmenter.h"
#include <regex>
#include <sstream>

namespace traductor {

Segmenter::Segmenter(size_t maxSegmentChars) : maxSegmentChars_(maxSegmentChars) {}

std::vector<std::string> Segmenter::segment(const std::string& text) const {
    if (text.empty() || !needsSegmentation(text)) {
        return {text};
    }
    
    std::vector<std::string> segments;
    
    // First split by double line breaks (paragraphs)
    auto paragraphs = splitByParagraphs(text);
    
    for (const auto& paragraph : paragraphs) {
        if (paragraph.empty()) {
            continue;
        }
        
        // If paragraph fits within limit, add as-is
        if (paragraph.length() <= maxSegmentChars_) {
            segments.push_back(paragraph);
            continue;
        }
        
        // Split long paragraph by sentences
        auto sentences = splitBySentences(paragraph);
        
        std::string currentSegment;
        for (const auto& sentence : sentences) {
            if (sentence.empty()) {
                continue;
            }
            
            // If adding this sentence would exceed limit, start new segment
            if (!currentSegment.empty() && 
                currentSegment.length() + sentence.length() + 1 > maxSegmentChars_) {
                segments.push_back(currentSegment);
                currentSegment = sentence;
            } else {
                if (!currentSegment.empty()) {
                    currentSegment += " " + sentence;
                } else {
                    currentSegment = sentence;
                }
            }
        }
        
        // Add remaining segment
        if (!currentSegment.empty()) {
            segments.push_back(currentSegment);
        }
    }
    
    return segments.empty() ? std::vector<std::string>{text} : segments;
}

std::vector<std::vector<std::string>> Segmenter::segment(const std::vector<std::string>& texts) const {
    std::vector<std::vector<std::string>> results;
    results.reserve(texts.size());
    
    for (const auto& text : texts) {
        results.push_back(segment(text));
    }
    
    return results;
}

std::string Segmenter::rejoinSegments(const std::vector<std::string>& segments) const {
    if (segments.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < segments.size(); ++i) {
        if (i > 0) {
            oss << " ";
        }
        oss << segments[i];
    }
    
    return oss.str();
}

std::vector<std::string> Segmenter::splitByParagraphs(const std::string& text) const {
    std::vector<std::string> paragraphs;
    
    // Split by double line breaks
    std::regex paraRegex(R"(\n\s*\n)");
    std::sregex_token_iterator iter(text.begin(), text.end(), paraRegex, -1);
    std::sregex_token_iterator end;
    
    for (; iter != end; ++iter) {
        std::string paragraph = *iter;
        // Trim whitespace
        paragraph = std::regex_replace(paragraph, std::regex(R"(^\s+|\s+$)"), "");
        if (!paragraph.empty()) {
            paragraphs.push_back(paragraph);
        }
    }
    
    // If no paragraphs found, treat whole text as one paragraph
    if (paragraphs.empty() && !text.empty()) {
        paragraphs.push_back(std::regex_replace(text, std::regex(R"(^\s+|\s+$)"), ""));
    }
    
    return paragraphs;
}

std::vector<std::string> Segmenter::splitBySentences(const std::string& paragraph) const {
    std::vector<std::string> sentences;
    
    // Sentence-ending punctuation followed by space and capital letter
    // This regex handles Spanish punctuation patterns
    std::regex sentenceRegex(R"(([.!?]+\s+)(?=[A-ZÁÉÍÓÚÑ¿¡]))");
    
    std::sregex_token_iterator iter(paragraph.begin(), paragraph.end(), sentenceRegex, -1);
    std::sregex_token_iterator end;
    
    std::vector<std::string> parts;
    for (; iter != end; ++iter) {
        parts.push_back(*iter);
    }
    
    // Reconstruct sentences with their punctuation
    std::string currentSentence;
    for (size_t i = 0; i < parts.size(); ++i) {
        currentSentence += parts[i];
        
        // Check if this part ends with sentence punctuation
        if (i < parts.size() - 1 && 
            std::regex_search(parts[i], std::regex(R"([.!?]+\s*$)"))) {
            // This is likely the end of a sentence
            std::string trimmed = currentSentence;
            trimmed = std::regex_replace(trimmed, std::regex(R"(^\s+|\s+$)"), "");
            if (!trimmed.empty()) {
                sentences.push_back(trimmed);
            }
            currentSentence.clear();
        }
    }
    
    // Add remaining sentence if any
    if (!currentSentence.empty()) {
        std::string trimmed = currentSentence;
        trimmed = std::regex_replace(trimmed, std::regex(R"(^\s+|\s+$)"), "");
        if (!trimmed.empty()) {
            sentences.push_back(trimmed);
        }
    }
    
    // Fallback: if no sentences detected, return the paragraph as single sentence
    if (sentences.empty() && !paragraph.empty()) {
        sentences.push_back(std::regex_replace(paragraph, std::regex(R"(^\s+|\s+$)"), ""));
    }
    
    return sentences;
}

bool Segmenter::needsSegmentation(const std::string& text) const {
    return text.length() > maxSegmentChars_;
}

} // namespace traductor
