#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <mutex>

// Forward declarations
#ifdef HAVE_CTRANSLATE2
#include <ctranslate2/translator.h>
#include <ctranslate2/translation_options.h>
#endif

namespace traductor {

class Tokenizer;
class LRUCache;
class Segmenter;
class Glossary;
class Config;
class PostprocessDA;
class PostprocessES;

// Type alias for Glossary terms
using TermMap = std::unordered_map<std::string, std::string>;

/**
 * Main translation engine that orchestrates the entire translation pipeline.
 * Uses CTranslate2 for inference with SentencePiece tokenization.
 */
class TranslatorEngine {
public:
    struct TranslationResult {
        std::vector<std::string> translations;
        double latency_ms = 0.0;
        bool usedCache = false;
        std::string sourceLang;
        std::string targetLang;
        std::string direction;
    };

    explicit TranslatorEngine(const Config& config);
    ~TranslatorEngine();

    // Initialize the engine (load model and tokenizer)
    bool initialize();
    
    // Check if engine is ready
    bool isReady() const { return isReady_; }
    
    // Main translation interface
    TranslationResult translate(
        const std::vector<std::string>& texts,
        const std::string& direction = "es-da",
        int maxNewTokens = -1,  // -1 = auto-calculate
        bool formal = false,
        const TermMap& glossary = {}
    );
    
    // Single text translation (convenience)
    std::string translate(
        const std::string& text,
        const std::string& direction = "es-da",
        int maxNewTokens = -1,
        bool formal = false,
        const TermMap& glossary = {}
    );
    
    // HTML translation (preserves structure)
    std::string translateHtml(
        const std::string& html,
        const std::string& direction = "es-da", 
        int maxNewTokens = -1,
        bool formal = false,
        const TermMap& glossary = {}
    );
    
    // Health check and diagnostics
    struct HealthInfo {
        bool modelLoaded = false;
        bool tokenizerLoaded = false;
        std::string lastError;
        std::chrono::milliseconds loadTime{0};
        size_t cacheSize = 0;
        double cacheHitRate = 0.0;
    };
    
    HealthInfo getHealthInfo() const;
    
    // Performance metrics
    double getAverageLatency() const { return avgLatency_; }
    size_t getTotalTranslations() const { return totalTranslations_; }

private:
    const Config& config_;
    
    // Core components
#ifdef HAVE_CTRANSLATE2
    std::unique_ptr<ctranslate2::Translator> translator_;
#endif
    std::unique_ptr<Tokenizer> tokenizer_;
    std::unique_ptr<LRUCache> cache_;
    std::unique_ptr<Segmenter> segmenter_;
    
    // State
    bool isReady_ = false;
    std::string lastError_;
    std::chrono::steady_clock::time_point loadStartTime_;
    std::chrono::milliseconds loadTime_{0};
    
    // Performance tracking
    double avgLatency_ = 0.0;
    size_t totalTranslations_ = 0;
    
    // Internal helpers
    bool loadModel();
    bool loadTokenizer();
    
    // Translation pipeline
    std::vector<std::string> preprocessTexts(
        const std::vector<std::string>& texts,
        const TermMap& glossary
    ) const;
    
    std::vector<std::string> postprocessTexts(
        const std::vector<std::string>& texts,
        const std::string& direction,
        bool formal,
        const TermMap& glossary
    ) const;
    
    // CTranslate2 integration
    std::vector<std::vector<std::string>> runInference(
        const std::vector<std::vector<std::string>>& sourceTokens,
        const std::string& targetLang,
        int maxNewTokens,
        int beamSize
    );
    
    // Token management
    std::vector<std::vector<std::string>> prepareSourceTokens(
        const std::vector<std::string>& texts,
        const std::string& sourceLang
    );
    
    std::vector<std::string> prepareTargetPrefix(
        size_t batchSize,
        const std::string& targetLang
    );
    
    // Utility functions
    int calculateMaxNewTokens(const std::vector<std::vector<std::string>>& sourceTokens) const;
    std::string getLanguageCode(const std::string& direction, bool isSource) const;
    bool validateDirection(const std::string& direction) const;
    bool isMostlyLatin(const std::string& text) const;
    
    // Translation segment processing
    std::string translateSegment(const std::string& segment, const std::string& direction, 
                                int maxNewTokens, bool formal);
    std::string translateSegmentSimple(const std::string& segment, const std::string& direction, 
                                      bool formal);
    std::string postprocessTranslation(const std::string& text, const std::string& direction, 
                                      bool formal) const;
    
    // Thread safety
    mutable std::mutex translateMutex_;
    
    // Cache operations
    std::string makeCacheKey(const std::string& text, const std::string& direction) const;
};

} // namespace traductor
