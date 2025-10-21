#pragma once

#include <unordered_map>
#include <string>
#include <list>
#include <mutex>

namespace traductor {

/**
 * Thread-safe LRU cache implementation for translation results.
 * Uses direction||text as key and stores translated results.
 */
class LRUCache {
public:
    struct CacheStats {
        size_t size = 0;
        size_t maxSize = 0;
        size_t hits = 0;
        size_t misses = 0;
        double hitRate = 0.0;
    };

    explicit LRUCache(size_t maxSize = 1024);
    ~LRUCache() = default;

    // Cache operations
    std::string get(const std::string& key);
    void put(const std::string& key, const std::string& value);
    void clear();
    
    // Statistics
    CacheStats getStats() const;
    size_t size() const;
    double hitRate() const;

private:
    mutable std::mutex mutex_;
    size_t maxSize_;
    
    // Cache storage: key -> iterator in access list
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> cache_;
    
    // Access order: most recent at end
    std::list<std::pair<std::string, std::string>> accessList_;
    
    // Statistics
    mutable size_t hits_ = 0;
    mutable size_t misses_ = 0;
    
    // Helper to normalize cache key
    std::string normalizeKey(const std::string& text) const;
};

} // namespace traductor
