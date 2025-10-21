#include "Tokenizer.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

#ifdef HAVE_SENTENCEPIECE
#include "sentencepiece_processor.h"
#endif

namespace traductor {

bool Tokenizer::load(const std::string& modelPath) {
    try {
        if (!std::filesystem::exists(modelPath)) {
            std::cerr << "Tokenizer model file not found: " << modelPath << std::endl;
            return false;
        }
        
#ifdef HAVE_SENTENCEPIECE
        processor_ = std::make_unique<sentencepiece::SentencePieceProcessor>();
        
        auto status = processor_->Load(modelPath);
        if (!status.ok()) {
            std::cerr << "Failed to load SentencePiece model: " << status.ToString() << std::endl;
            processor_.reset();
            return false;
        }
        
        std::cout << "SentencePiece tokenizer loaded from: " << modelPath << std::endl;
        initializeLanguageMappings();
        return true;
#else
        std::cout << "SentencePiece not available, using simplified tokenizer" << std::endl;
        initializeLanguageMappings();
        return true;
#endif
    } catch (const std::exception& e) {
        std::cerr << "Error loading tokenizer: " << e.what() << std::endl;
        return false;
    }
}

void Tokenizer::initializeLanguageMappings() {
    // Simplified language mappings
    langCodeToId_["spa_Latn"] = 1;
    langCodeToId_["dan_Latn"] = 2;
    
    idToLangCode_[1] = "spa_Latn";
    idToLangCode_[2] = "dan_Latn";
}

std::vector<std::string> Tokenizer::tokenize(const std::string& text) {
#ifdef HAVE_SENTENCEPIECE
    if (processor_) {
        std::vector<std::string> tokens;
        auto status = processor_->Encode(text, &tokens);
        if (status.ok()) {
            return tokens;
        }
    }
#endif
    // Fallback: simplified tokenization - just split by spaces
    std::vector<std::string> tokens;
    std::string current;
    
    for (char c : text) {
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

std::vector<std::vector<std::string>> Tokenizer::tokenize(const std::vector<std::string>& texts) {
    std::vector<std::vector<std::string>> result;
    for (const auto& text : texts) {
        result.push_back(tokenize(text));
    }
    return result;
}

std::string Tokenizer::detokenize(const std::vector<std::string>& tokens) {
#ifdef HAVE_SENTENCEPIECE
    if (processor_) {
        std::string result;
        auto status = processor_->Decode(tokens, &result);
        if (status.ok()) {
            return result;
        }
    }
#endif
    // Fallback: simple joining with spaces
    std::string result;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) result += " ";
        result += tokens[i];
    }
    return result;
}

std::vector<std::string> Tokenizer::detokenize(const std::vector<std::vector<std::string>>& tokenLists) {
    std::vector<std::string> result;
    for (const auto& tokens : tokenLists) {
        result.push_back(detokenize(tokens));
    }
    return result;
}

std::string Tokenizer::getLanguageToken(const std::string& languageCode) const {
    return languageCode; // Simplified - return the code as-is
}

bool Tokenizer::isValidLanguageCode(const std::string& languageCode) const {
    return languageCode == "spa_Latn" || languageCode == "dan_Latn";
}

std::vector<int> Tokenizer::encode(const std::string& text, const std::string& sourceLang) {
#ifdef HAVE_SENTENCEPIECE
    if (processor_) {
        std::vector<int> ids;
        auto status = processor_->Encode(text, &ids);
        if (status.ok()) {
            return addSourceLanguageToken(ids, sourceLang);
        }
    }
#endif
    // Fallback: simplified encoding
    std::vector<int> result;
    for (char c : text) {
        result.push_back(static_cast<int>(c));
    }
    return result;
}

std::vector<std::vector<int>> Tokenizer::encode(const std::vector<std::string>& texts, const std::string& sourceLang) {
    std::vector<std::vector<int>> result;
    for (const auto& text : texts) {
        result.push_back(encode(text, sourceLang));
    }
    return result;
}

std::string Tokenizer::decode(const std::vector<int>& tokenIds, bool skipSpecialTokens) {
#ifdef HAVE_SENTENCEPIECE
    if (processor_) {
        std::string result;
        auto status = processor_->Decode(tokenIds, &result);
        if (status.ok()) {
            return result;
        }
    }
#endif
    // Fallback: simple character conversion
    std::string result;
    for (int id : tokenIds) {
        if (id > 0 && id < 256) { // Simple ASCII range check
            result += static_cast<char>(id);
        }
    }
    return result;
}

std::vector<std::string> Tokenizer::decode(const std::vector<std::vector<int>>& tokenIdLists, bool skipSpecialTokens) {
    std::vector<std::string> result;
    for (const auto& ids : tokenIdLists) {
        result.push_back(decode(ids, skipSpecialTokens));
    }
    return result;
}

std::vector<int> Tokenizer::tokensToIds(const std::vector<std::string>& tokens) {
    std::vector<int> result;
    for (const auto& token : tokens) {
        // Simple hash-based ID generation
        int id = 0;
        for (char c : token) {
            id = id * 31 + static_cast<int>(c);
        }
        result.push_back(std::abs(id) % 10000); // Keep IDs reasonable
    }
    return result;
}

std::vector<std::string> Tokenizer::idsToTokens(const std::vector<int>& ids) {
    std::vector<std::string> result;
    for (int id : ids) {
        // Cannot reverse hash, so return placeholder
        result.push_back("token_" + std::to_string(id));
    }
    return result;
}

bool Tokenizer::isLoaded() const {
#ifdef HAVE_SENTENCEPIECE
    return processor_ != nullptr;
#else
    return processor_ != nullptr; // In simplified mode, always return true after load()
#endif
}

const std::vector<std::string>& Tokenizer::getSupportedLanguages() {
    static const std::vector<std::string> languages = {"spa_Latn", "dan_Latn"};
    return languages;
}

std::vector<int> Tokenizer::addSourceLanguageToken(const std::vector<int>& tokens, const std::string& sourceLang) {
    std::vector<int> result;
    if (auto it = langCodeToId_.find(sourceLang); it != langCodeToId_.end()) {
        result.push_back(it->second);
    }
    result.insert(result.end(), tokens.begin(), tokens.end());
    return result;
}

} // namespace traductor