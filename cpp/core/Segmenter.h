#pragma once

#include <vector>
#include <string>

namespace traductor {

/**
 * Text segmentation for anti-truncation.
 * Splits long texts into manageable chunks while preserving structure.
 */
class Segmenter {
public:
    explicit Segmenter(size_t maxSegmentChars = 800);
    ~Segmenter() = default;

    // Segment a single text
    std::vector<std::string> segment(const std::string& text) const;
    
    // Batch segmentation
    std::vector<std::vector<std::string>> segment(const std::vector<std::string>& texts) const;
    
    // Rejoin segments back to original text structure
    std::string rejoinSegments(const std::vector<std::string>& segments) const;
    
    // Configure segment size
    void setMaxSegmentChars(size_t maxChars) { maxSegmentChars_ = maxChars; }
    size_t getMaxSegmentChars() const { return maxSegmentChars_; }

private:
    size_t maxSegmentChars_;
    
    // Helper methods
    std::vector<std::string> splitByParagraphs(const std::string& text) const;
    std::vector<std::string> splitBySentences(const std::string& paragraph) const;
    bool needsSegmentation(const std::string& text) const;
};

} // namespace traductor
