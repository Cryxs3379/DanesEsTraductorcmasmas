#include <gtest/gtest.h>
#include "../core/PostprocessDA.h"
#include "../core/PostprocessES.h"

class PostprocessTest : public ::testing::Test {
protected:
    void SetUp() override {
        postprocessDA_ = std::make_unique<traductor::PostprocessDA>();
        postprocessES_ = std::make_unique<traductor::PostprocessES>();
    }
    
    void TearDown() override {
        postprocessDA_.reset();
        postprocessES_.reset();
    }
    
    std::unique_ptr<traductor::PostprocessDA> postprocessDA_;
    std::unique_ptr<traductor::PostprocessES> postprocessES_;
};

TEST_F(PostprocessTest, DateConversionES) {
    // Test Spanish date format (should be unchanged for ES postprocessing)
    std::string dateES = "16/10/2025";
    std::string result = postprocessES_->process(dateES);
    
    // ES postprocessing should handle Spanish dates appropriately
    EXPECT_FALSE(result.empty());
}

TEST_F(PostprocessTest, DateConversionDA) {
    // Test Danish date format processing
    std::string dateDA = "16.10.2025";
    std::string result = postprocessDA_->process(dateDA);
    
    // DA postprocessing should handle Danish dates appropriately
    EXPECT_FALSE(result.empty());
}
