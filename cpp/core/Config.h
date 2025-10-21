#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace traductor {

/**
 * Central configuration management for the translator.
 * Reads from JSON/ENV variables and provides default parameters.
 */
class Config {
public:
    Config();
    ~Config() = default;

    // Model Configuration
    std::string modelDir() const { return modelDir_; }
    std::string ct2Dir() const { return ct2Dir_; }
    
    // Translation Parameters
    int beamSize() const { return beamSize_; }
    int maxInputTokens() const { return maxInputTokens_; }
    int defaultMaxNewTokens() const { return defaultMaxNewTokens_; }
    int maxMaxNewTokens() const { return maxMaxNewTokens_; }
    int maxSegmentChars() const { return maxSegmentChars_; }
    
    // CTranslate2 Performance
    int ct2InterThreads() const { return ct2InterThreads_; }
    int ct2IntraThreads() const { return ct2IntraThreads_; }
    
    // Language Settings
    std::string sourceLang() const { return sourceLang_; }
    std::string targetLang() const { return targetLang_; }
    bool formalDa() const { return formalDa_; }
    
    // Server Settings
    std::string host() const { return host_; }
    int port() const { return port_; }
    
    // Cache Settings
    size_t cacheSize() const { return cacheSize_; }
    
    // Limits
    int maxBatchSize() const { return maxBatchSize_; }
    int requestTimeout() const { return requestTimeout_; }
    
    // Load configuration from JSON file or use environment variables
    bool loadFromFile(const std::string& configPath);
    void loadFromEnvironment();
    
    // Get JSON representation of current config
    nlohmann::json toJson() const;

private:
    // Model paths
    std::string modelDir_;
    std::string ct2Dir_;
    
    // Translation parameters (matching Python defaults)
    int beamSize_ = 4;
    int maxInputTokens_ = 4096;
    int defaultMaxNewTokens_ = 256;
    int maxMaxNewTokens_ = 8192;
    int maxSegmentChars_ = 800;
    
    // CTranslate2 threading
    int ct2InterThreads_ = 4;
    int ct2IntraThreads_ = 4;
    
    // Languages (FLORES-200 codes)
    std::string sourceLang_ = "spa_Latn";
    std::string targetLang_ = "dan_Latn";
    bool formalDa_ = false;
    
    // Server
    std::string host_ = "0.0.0.0";
    int port_ = 8000;
    
    // Cache
    size_t cacheSize_ = 1024;
    
    // Limits
    int maxBatchSize_ = 16;
    int requestTimeout_ = 300;
    
    // Helper to get environment variable or default
    template<typename T>
    T getEnvOrDefault(const std::string& envVar, const T& defaultValue);
    
    void setDefaults();
};

} // namespace traductor
