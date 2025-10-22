#include "Glossary.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>

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
    
    // Marcar entidades protegidas con [[KEEP::...]]
    for (const auto& entity : entities) {
        // Escapar el placeholder para regex literal
        std::string escaped = std::regex_replace(
            entity.placeholder,
            std::regex(R"([.^$*+?()\[\]\\|{}])"),
            R"(\$&)"
        );

        result = std::regex_replace(
            result,
            std::regex(escaped),
            "[[KEEP::" + entity.original + "]]"
        );
    }
    
    return result;
}

std::string Glossary::applyPostProcessing(const std::string& text) const {
    if (text.empty()) return text;

    std::string result = text;

    // 1) Construir mapa case-insensitive para términos de glosario
    std::unordered_map<std::string, std::string> termsLower;
    termsLower.reserve(terms_.size());
    for (const auto& [termEs, termDa] : terms_) {
        std::string key = termEs;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        termsLower.emplace(std::move(key), termDa);
    }

    // 2) Reemplazar marcadores [[TERM::...]] de forma manual (sin lambda)
    {
        const std::regex termPattern(R"(\[\[TERM::(.*?)\]\])");
        std::string rebuilt;
        rebuilt.reserve(result.size() + 64);

        std::sregex_iterator it(result.begin(), result.end(), termPattern);
        std::sregex_iterator end;
        std::size_t last_pos = 0;

        for (; it != end; ++it) {
            const auto& m = *it;
            // Copiar texto previo
            rebuilt.append(result, last_pos, static_cast<std::size_t>(m.position()) - last_pos);

            // Resolver el término
            std::string originalTerm = m[1].str();
            std::string key = originalTerm;
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);

            if (auto f = termsLower.find(key); f != termsLower.end()) {
                rebuilt.append(f->second); // traducción del glosario
            } else {
                rebuilt.append(originalTerm); // fallback
            }

            last_pos = static_cast<std::size_t>(m.position() + m.length());
        }
        rebuilt.append(result, last_pos, std::string::npos);
        result.swap(rebuilt);
    }

    // 3) Restaurar marcadores [[KEEP::...]]
    {
        const std::regex keepPattern(R"(\[\[KEEP::(.*?)\]\])");
        result = std::regex_replace(result, keepPattern, "$1");
    }

    // 4) Limpiezas por si quedara algún residuo
    result = std::regex_replace(result, std::regex(R"(\[\[TERM::(.*?)\]\])"), "$1");
    result = std::regex_replace(result, std::regex(R"(\[\[KEEP::(.*?)\]\])"), "$1");

    return result;
}

std::pair<std::string, std::vector<Glossary::ProtectedEntity>> Glossary::protectEntities(const std::string& text) const {
    std::vector<ProtectedEntity> entities;
    std::string result = text;

    // Protect URLs (http://, https://, www.)
    {
        const std::regex urlRegex(R"((https?://[^\s]+|www\.[^\s]+))");
        std::sregex_iterator it(result.begin(), result.end(), urlRegex), end;
        size_t idx = 0;
        for (; it != end; ++it, ++idx) {
            const std::string matched = (*it)[0].str();
            ProtectedEntity e{ createPlaceholder("URL", idx), matched, "URL" };
            entities.push_back(e);
            size_t pos = result.find(matched);
            if (pos != std::string::npos) result.replace(pos, matched.size(), e.placeholder);
        }
    }

    // Protect emails
    {
        const std::regex emailRegex(R"(\b[A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}\b)");
        std::sregex_iterator it(result.begin(), result.end(), emailRegex), end;
        size_t idx = 0;
        for (; it != end; ++it, ++idx) {
            const std::string matched = (*it)[0].str();
            ProtectedEntity e{ createPlaceholder("EMAIL", idx), matched, "EMAIL" };
            entities.push_back(e);
            size_t pos = result.find(matched);
            if (pos != std::string::npos) result.replace(pos, matched.size(), e.placeholder);
        }
    }

    // Protect numbers (integers, decimals, with separators)
    {
        const std::regex numberRegex(R"(\b\d{1,3}(?:[.]\d{3})*(?:[.,]\d+)?\b)");
        std::sregex_iterator it(result.begin(), result.end(), numberRegex), end;
        size_t idx = 0;
        for (; it != end; ++it, ++idx) {
            const std::string matched = (*it)[0].str();
            ProtectedEntity e{ createPlaceholder("NUM", idx), matched, "NUMBER" };
            entities.push_back(e);
            size_t pos = result.find(matched);
            if (pos != std::string::npos) result.replace(pos, matched.size(), e.placeholder);
        }
    }

    return { result, entities };
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
