#include "Glossary.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace traductor {

bool Glossary::loadFromString(const std::string& glossaryText) {
    if (glossaryText.empty()) {
        return true;
    }
    
    terms_.clear();
    
    std::istringstream iss(glossaryText);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Skip empty lines and comments
        line = std::regex_replace(line, std::regex(R"(^\s+|\s+$)"), ""); // trim
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse format: term_es=term_da
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string termEs = line.substr(0, eqPos);
            std::string termDa = line.substr(eqPos + 1);
            
            // Trim both terms
            termEs = std::regex_replace(termEs, std::regex(R"(^\s+|\s+$)"), "");
            termDa = std::regex_replace(termDa, std::regex(R"(^\s+|\s+$)"), "");
            
            if (!termEs.empty() && !termDa.empty()) {
                terms_[termEs] = termDa;
            }
        }
    }
    
    return true;
}

void Glossary::setTerms(const TermMap& terms) {
    terms_ = terms;
}

std::string Glossary::applyPreProcessing(const std::string& text) const {
    if (text.empty() || terms_.empty()) {
        return text;
    }
    
    // First protect entities (URLs, emails, numbers)
    auto [protectedText, entities] = protectEntities(text);
    
    std::string result = protectedText;
    
    // Sort terms by length (longest first) to avoid partial matches
    std::vector<std::pair<std::string, std::string>> sortedTerms(terms_.begin(), terms_.end());
    std::sort(sortedTerms.begin(), sortedTerms.end(), 
              [](const auto& a, const auto& b) {
                  return a.first.length() > b.first.length();
              });
    
    // Apply term marking
    for (const auto& [termEs, termDa] : sortedTerms) {
        if (termEs.empty()) {
            continue;
        }
        
        // Create regex pattern with word boundaries for exact match (case-insensitive)
        std::string escapedTerm = std::regex_replace(termEs, std::regex(R"([.^$*+?()\[\]\\|{}])"), R"(\$&)");
        std::string pattern = "\\b(" + escapedTerm + ")\\b";
        
        std::regex termRegex(pattern, std::regex_constants::icase);
        
        // Replace with marker preserving original case
        result = std::regex_replace(result, termRegex, "[[TERM::$1]]");
    }
    
    // Mark protected entities with KEEP markers
    for (const auto& entity : entities) {
        result = std::regex_replace(result, std::regex(std::regex_replace(entity.placeholder, 
            std::regex(R"([.^$*+?()\[\]\\|{}])"), R"(\$&)")), 
            "[[KEEP::" + entity.original + "]]");
    }
    
    return result;
}

std::string Glossary::applyPostProcessing(const std::string& text) const {
    if (text.empty()) {
        return text;
    }
    
    std::string result = text;
    
    // Create case-insensitive lookup for terms
    std::unordered_map<std::string, std::string> termsLower;
    for (const auto& [termEs, termDa] : terms_) {
        std::string termLower = termEs;
        std::transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);
        termsLower[termLower] = termDa;
    }
    
    // Process TERM markers
    std::regex termPattern(R"(\[\[TERM::(.*?)\]\]");
    result = std::regex_replace(result, termPattern, 
        [&termsLower](const std::smatch& match) -> std::string {
            std::string originalTerm = match[1].str();
            std::string termLower = originalTerm;
            std::transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);
            
            auto it = termsLower.find(termLower);
            if (it != termsLower.end()) {
                return it->second;
            }
            
            return originalTerm; // Fallback: return original if not found
        });
    
    // Process KEEP markers (restore protected entities)
    std::regex keepPattern(R"(\[\[KEEP::(.*?)\]\]");
    result = std::regex_replace(result, keepPattern, "$1");
    
    // Clean up any residual markers as safety measure
    result = std::regex_replace(result, std::regex("\\[\\[TERM::(.*?)\\]\\]"), "$1");
    result = std::regex_replace(result, std::regex("\\[\\[KEEP::(.*?)\\]\\]"), "$1");
    
    return result;
}

std::pair<std::string, std::vector<Glossary::ProtectedEntity>> Glossary::protectEntities(const std::string& text) const {
    std::vector<ProtectedEntity> entities;
    std::string result = text;
    
    // Protect URLs (http://, https://, www.)
    {
        std::regex urlRegex(R"((https?://[^\s]+|www\.[^\s]+))");
        std::sregex_iterator iter(text.begin(), text.end(), urlRegex);
        std::sregex_iterator end;
        
        size_t urlIndex = 0;
        for (std::sregex_iterator i = iter; i != end; ++i, ++urlIndex) {
            ProtectedEntity entity;
            entity.type = "URL";
            entity.original = (*i)[1].str();
            entity.placeholder = createPlaceholder("URL", urlIndex);
            entities.push_back(entity);
            
            // Replace first occurrence
            size_t pos = result.find(entity.original);
            if (pos != std::string::npos) {
                result.replace(pos, entity.original.length(), entity.placeholder);
            }
        }
    }
    
    // Protect emails
    {
        std::regex emailRegex(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b)");
        std::sregex_iterator iter(text.begin(), text.end(), emailRegex);
        std::sregex_iterator end;
        
        size_t emailIndex = 0;
        for (std::sregex_iterator i = iter; i != end; ++i, ++emailIndex) {
            ProtectedEntity entity;
            entity.type = "EMAIL";
            entity.original = (*i)[0].str();
            entity.placeholder = createPlaceholder("EMAIL", emailIndex);
            entities.push_back(entity);
            
            // Replace first occurrence
            size_t pos = result.find(entity.original);
            if (pos != std::string::npos) {
                result.replace(pos, entity.original.length(), entity.placeholder);
            }
        }
    }
    
    // Protect numbers (integers, decimals, with separators)
    {
        std::regex numberRegex(R"(\b\d+(?:[.,]\d+)*\b)");
        std::sregex_iterator iter(text.begin(), text.end(), numberRegex);
        std::sregex_iterator end;
        
        size_t numberIndex = 0;
        for (std::sregex_iterator i = iter; i != end; ++i, ++numberIndex) {
            ProtectedEntity entity;
            entity.type = "NUMBER";
            entity.original = (*i)[0].str();
            entity.placeholder = createPlaceholder("NUM", numberIndex);
            entities.push_back(entity);
            
            // Replace first occurrence
            size_t pos = result.find(entity.original);
            if (pos != std::string::npos) {
                result.replace(pos, entity.original.length(), entity.placeholder);
            }
        }
    }
    
    return std::make_pair(result, entities);
}

std::string Glossary::restoreEntities(const std::string& text, const std::vector<ProtectedEntity>& entities) const {
    std::string result = text;
    
    for (const auto& entity : entities) {
        size_t pos = result.find(entity.placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, entity.placeholder.length(), entity.original);
            pos = result.find(entity.placeholder, pos + entity.original.length());
        }
    }
    
    return result;
}

std::string Glossary::createPlaceholder(const std::string& type, size_t index) const {
    return "__" + type + "_" + std::to_string(index) + "__";
}

bool Glossary::isValidTerm(const std::string& term) const {
    return !term.empty() && term.find('=') == std::string::npos && term.length() < 100;
}

} // namespace traductor
