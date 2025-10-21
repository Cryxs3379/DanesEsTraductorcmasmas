#include "LRUCache.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace traductor {

LRUCache::LRUCache(size_t maxSize) : maxSize_(maxSize) {
    cache_.reserve(maxSize);
}

std::string LRUCache::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        // Move to end (most recent)
        accessList_.splice(accessList_.end(), accessList_, it->second);
        hits_++;
        return it->second->second;
    }
    
    misses_++;
    return "";
}

void LRUCache::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        // Update existing entry and move to end
        it->second->second = value;
        accessList_.splice(accessList_.end(), accessList_, it->second);
        return;
    }
    
    // Check if we need to evict
    if (cache_.size() >= maxSize_) {
        // Remove least recently used (first element)
        auto lru = accessList_.begin();
        cache_.erase(lru->first);
        accessList_.erase(lru);
    }
    
    // Add new entry at end
    accessList_.emplace_back(key, value);
    cache_[key] = std::prev(accessList_.end());
}

void LRUCache::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
    accessList_.clear();
    hits_ = 0;
    misses_ = 0;
}

LRUCache::CacheStats LRUCache::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    CacheStats stats;
    stats.size = cache_.size();
    stats.maxSize = maxSize_;
    stats.hits = hits_;
    stats.misses = misses_;
    
    size_t total = hits_ + misses_;
    stats.hitRate = total > 0 ? (static_cast<double>(hits_) / total) * 100.0 : 0.0;
    
    return stats;
}

size_t LRUCache::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cache_.size();
}

double LRUCache::hitRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t total = hits_ + misses_;
    return total > 0 ? (static_cast<double>(hits_) / total) * 100.0 : 0.0;
}

std::string LRUCache::normalizeKey(const std::string& text) const {
    std::string normalized = text;
    
    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    // Normalize whitespace
    std::istringstream iss(normalized);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    // Join with single spaces
    std::ostringstream oss;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) oss << " ";
        oss << tokens[i];
    }
    
    return oss.str();
}

} // namespace traductor
