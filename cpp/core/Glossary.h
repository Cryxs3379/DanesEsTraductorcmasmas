#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace traductor {

/**
 * Glossary management for term protection and substitution.
 * Handles pre/post-processing of texts with glossary terms.
 */
class Glossary {
public:
    using TermMap = std::unordered_map<std::string, std::string>;
    
    Glossary() = default;
    ~Glossary() = default;

    // Parse glossary from string format (term_es=term_da)
    bool loadFromString(const std::string& glossaryText);
    
    // Load from map
    void setTerms(const TermMap& terms);
    
    // Get current terms
    const TermMap& getTerms() const { return terms_; }
    
    // Pre-processing: mark terms for protection
    std::string applyPreProcessing(const std::string& text) const;
    
    // Post-processing: replace marked terms with translations
    std::string applyPostProcessing(const std::string& text) const;
    
    // Clear glossary
    void clear() { terms_.clear(); }
    
    // Check if glossary has terms
    bool isEmpty() const { return terms_.empty(); }
    size_t size() const { return terms_.size(); }

private:
    TermMap terms_;
    
    // Protected entities storage during processing
    struct ProtectedEntity {
        std::string placeholder;
        std::string original;
        std::string type; // "URL", "EMAIL", "NUMBER", "TERM"
    };
    
    // Protect entities (URLs, emails, numbers)
    std::pair<std::string, std::vector<ProtectedEntity>> protectEntities(const std::string& text) const;
    
    // Restore protected entities
    std::string restoreEntities(const std::string& text, const std::vector<ProtectedEntity>& entities) const;
    
    // Helper methods
    std::string createPlaceholder(const std::string& type, size_t index) const;
    bool isValidTerm(const std::string& term) const;
};

} // namespace traductor
