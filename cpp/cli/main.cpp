#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include "../core/TranslatorEngine.h"
#include "../core/Config.h"
#include "../core/Glossary.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --direction DIR     Translation direction: es-da or da-es (default: es-da)\n";
    std::cout << "  --formal           Use formal Danish style\n";
    std::cout << "  --max_tokens N     Maximum tokens to generate (default: auto)\n";
    std::cout << "  --in FILE          Input text file (stdin if not specified)\n";
    std::cout << "  --out FILE         Output file (stdout if not specified)\n";
    std::cout << "  --html             HTML mode for email translation\n";
    std::cout << "  --metrics          Show detailed performance metrics\n";
    std::cout << "  --glossary FILE    Load glossary from file (format: term_es=term_da)\n";
    std::cout << "  --config FILE      Load configuration from JSON file\n";
    std::cout << "  --help             Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --direction es-da --in input.txt --out output.txt\n";
    std::cout << "  " << programName << " --direction da-es --formal --html --metrics < email.html\n";
    std::cout << "  echo \"Hola mundo\" | " << programName << " --direction es-da --metrics\n";
}

std::string loadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filepath << std::endl;
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    return content;
}

bool saveFile(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot write to file " << filepath << std::endl;
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

traductor::Glossary::TermMap loadGlossary(const std::string& filepath) {
    traductor::Glossary glossary;
    std::string content = loadFile(filepath);
    
    if (content.empty()) {
        return {};
    }
    
    if (!glossary.loadFromString(content)) {
        std::cerr << "Warning: Failed to load glossary from " << filepath << std::endl;
        return {};
    }
    
    return glossary.getTerms();
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string direction = "es-da";
    bool formal = false;
    int maxTokens = -1;
    std::string inputFile;
    std::string outputFile;
    bool htmlMode = false;
    bool showMetrics = false;
    std::string glossaryFile;
    std::string configFile;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--direction" && i + 1 < argc) {
            direction = argv[++i];
            if (direction != "es-da" && direction != "da-es") {
                std::cerr << "Error: Invalid direction. Use 'es-da' or 'da-es'" << std::endl;
                return 1;
            }
        } else if (arg == "--formal") {
            formal = true;
        } else if (arg == "--max_tokens" && i + 1 < argc) {
            maxTokens = std::atoi(argv[++i]);
            if (maxTokens <= 0) {
                std::cerr << "Error: max_tokens must be positive" << std::endl;
                return 1;
            }
        } else if (arg == "--in" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "--out" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--html") {
            htmlMode = true;
        } else if (arg == "--metrics") {
            showMetrics = true;
        } else if (arg == "--glossary" && i + 1 < argc) {
            glossaryFile = argv[++i];
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Load configuration
    traductor::Config config;
    if (!configFile.empty()) {
        if (!config.loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load config from " << configFile << std::endl;
            return 1;
        }
    }
    
    // Load glossary if specified
    traductor::Glossary::TermMap glossary;
    if (!glossaryFile.empty()) {
        glossary = loadGlossary(glossaryFile);
        if (!glossary.empty()) {
            std::cout << "Loaded glossary with " << glossary.size() << " terms" << std::endl;
        }
    }
    
    // Initialize translator
    traductor::TranslatorEngine translator(config);
    
    std::cout << "Initializing translator..." << std::endl;
    auto startTime = std::chrono::steady_clock::now();
    
    if (!translator.initialize()) {
        auto health = translator.getHealthInfo();
        std::cerr << "Error: Failed to initialize translator" << std::endl;
        std::cerr << "Details: " << health.lastError << std::endl;
        return 1;
    }
    
    auto initTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime);
    std::cout << "Translator ready (" << initTime.count() << "ms)" << std::endl;
    
    // Load input
    std::string input;
    if (inputFile.empty()) {
        // Read from stdin
        std::cout << "Reading from stdin..." << std::endl;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!input.empty()) input += "\n";
            input += line;
        }
    } else {
        input = loadFile(inputFile);
        if (input.empty()) {
            return 1;
        }
    }
    
    if (input.empty()) {
        std::cerr << "Error: No input provided" << std::endl;
        return 1;
    }
    
    // Translate
    std::cout << "Translating..." << std::endl;
    std::string result;
    
    try {
        if (htmlMode) {
            result = translator.translateHtml(input, direction, maxTokens, formal, glossary);
        } else {
            result = translator.translate(input, direction, maxTokens, formal, glossary);
        }
        
        if (result.empty()) {
            std::cerr << "Error: Translation failed" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error during translation: " << e.what() << std::endl;
        return 1;
    }
    
    // Output result
    if (outputFile.empty()) {
        std::cout << result << std::endl;
    } else {
        if (!saveFile(outputFile, result)) {
            return 1;
        }
        std::cout << "Translation saved to " << outputFile << std::endl;
    }
    
    // Show metrics and timing
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime);
    
    std::cout << "Translation completed successfully" << std::endl;
    
    if (showMetrics) {
        auto health = translator.getHealthInfo();
        std::cout << "=== METRICS ===" << std::endl;
        std::cout << "Total time: " << totalTime.count() << "ms" << std::endl;
        std::cout << "Initialization: " << initTime.count() << "ms" << std::endl;
        std::cout << "Average latency: " << std::fixed << std::setprecision(2) 
                  << translator.getAverageLatency() << "ms" << std::endl;
        std::cout << "Total translations: " << translator.getTotalTranslations() << std::endl;
        std::cout << "Cache entries: " << health.cacheSize << std::endl;
        std::cout << "Cache hit rate: " << std::fixed << std::setprecision(1) 
                  << health.cacheHitRate << "%" << std::endl;
        std::cout << "Model status: " << (health.modelLoaded ? "Loaded" : "Simplified mode") << std::endl;
        std::cout << "Tokenizer: " << (health.tokenizerLoaded ? "Ready" : "Not loaded") << std::endl;
    }
    
    return 0;
}
