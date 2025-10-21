#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#ifdef HAVE_SENTENCEPIECE
#include "sentencepiece_processor.h"
#endif

namespace traductor {

/**
 * NLLB SentencePiece tokenizer wrapper.
 * Handles tokenization/detokenization and language-specific tokens.
 */
class Tokenizer {
public:
    Tokenizer() = default;
    ~Tokenizer() = default;

    // Initialize from model path
    bool load(const std::string& modelPath);
    
    // Tokenization interface
    std::vector<std::string> tokenize(const std::string& text);
    std::vector<std::vector<std::string>> tokenize(const std::vector<std::string>& texts);
    
    // Detokenization
    std::string detokenize(const std::vector<std::string>& tokens);
    std::vector<std::string> detokenize(const std::vector<std::vector<std::string>>& tokenLists);
    
    // Language token handling
    std::string getLanguageToken(const std::string& languageCode) const;
    bool isValidLanguageCode(const std::string& languageCode) const;
    
    // Encoding for CTranslate2 (returns token IDs)
    std::vector<int> encode(const std::string& text, const std::string& sourceLang = "");
    std::vector<std::vector<int>> encode(const std::vector<std::string>& texts, const std::string& sourceLang = "");
    
    // Decoding from token IDs back to text
    std::string decode(const std::vector<int>& tokenIds, bool skipSpecialTokens = true);
    std::vector<std::string> decode(const std::vector<std::vector<int>>& tokenIdLists, bool skipSpecialTokens = true);
    
    // Convert between tokens and IDs
    std::vector<int> tokensToIds(const std::vector<std::string>& tokens);
    std::vector<std::string> idsToTokens(const std::vector<int>& ids);
    
    // Language code mapping (NLLB supported languages)
    static const std::vector<std::string>& getSupportedLanguages();
    
    // Check if loaded
    bool isLoaded() const;

private:
#ifdef HAVE_SENTENCEPIECE
    std::unique_ptr<sentencepiece::SentencePieceProcessor> processor_;
#else
    void* processor_ = nullptr;  // Placeholder for simplified build
#endif
    
    // NLLB language code to token ID mapping
    std::unordered_map<std::string, int> langCodeToId_;
    std::unordered_map<int, std::string> idToLangCode_;
    
    // Initialize language mappings
    void initializeLanguageMappings();
    
    // Add source language token to beginning if needed
    std::vector<int> addSourceLanguageToken(const std::vector<int>& tokens, const std::string& sourceLang);
};

} // namespace traductor
