#include <gtest/gtest.h>
#include "../core/LRUCache.h"

class LRUCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache_ = std::make_unique<traductor::LRUCache<std::string, std::string>>(2);
    }
    
    void TearDown() override {
        cache_.reset();
    }
    
    std::unique_ptr<traductor::LRUCache<std::string, std::string>> cache_;
};

TEST_F(LRUCacheTest, BasicOperations) {
    // Test basic cache operations
    cache_->put("key1", "value1");
    
    auto result = cache_->get("key1");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "value1");
}

TEST_F(LRUCacheTest, CacheMiss) {
    // Test cache miss
    auto result = cache_->get("nonexistent");
    EXPECT_FALSE(result.has_value());
}

TEST_F(LRUCacheTest, LRUEviction) {
    // Test LRU eviction behavior
    cache_->put("key1", "value1");
    cache_->put("key2", "value2");
    cache_->put("key3", "value3"); // Should evict key1
    
    // key1 should be evicted
    EXPECT_FALSE(cache_->get("key1").has_value());
    
    // key2 and key3 should still be present
    EXPECT_TRUE(cache_->get("key2").has_value());
    EXPECT_TRUE(cache_->get("key3").has_value());
}
