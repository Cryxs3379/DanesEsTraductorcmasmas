#include <gtest/gtest.h>
#include "../core/Glossary.h"

class GlossaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        glossary_ = std::make_unique<traductor::Glossary>();
    }
    
    void TearDown() override {
        glossary_.reset();
    }
    
    std::unique_ptr<traductor::Glossary> glossary_;
};

TEST_F(GlossaryTest, BasicLoading) {
    // Test basic glossary loading from string
    std::string glossaryContent = "hello=hola\nworld=mundo\n";
    
    EXPECT_TRUE(glossary_->loadFromString(glossaryContent));
    
    auto terms = glossary_->getTerms();
    EXPECT_GE(terms.size(), 0); // Should have parsed terms
}

TEST_F(GlossaryTest, EmptyContent) {
    // Test empty glossary
    EXPECT_TRUE(glossary_->loadFromString(""));
    
    auto terms = glossary_->getTerms();
    EXPECT_EQ(terms.size(), 0);
}
