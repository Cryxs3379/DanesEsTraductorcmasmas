#include <gtest/gtest.h>
#include "../core/Segmenter.h"

class SegmenterTest : public ::testing::Test {
protected:
    void SetUp() override {
        segmenter_ = std::make_unique<traductor::Segmenter>();
    }
    
    void TearDown() override {
        segmenter_.reset();
    }
    
    std::unique_ptr<traductor::Segmenter> segmenter_;
};

TEST_F(SegmenterTest, BasicSegmentation) {
    // Test basic text segmentation
    std::string longText = "This is a very long text that should be segmented properly to avoid truncation issues during translation.";
    
    auto segments = segmenter_->segment(longText);
    
    // Should return at least one segment
    EXPECT_GE(segments.size(), 1);
    EXPECT_FALSE(segments.empty());
}

TEST_F(SegmenterTest, ShortText) {
    // Test with short text that doesn't need segmentation
    std::string shortText = "Hola mundo";
    
    auto segments = segmenter_->segment(shortText);
    
    // Should return exactly one segment
    EXPECT_EQ(segments.size(), 1);
    EXPECT_EQ(segments[0], shortText);
}
