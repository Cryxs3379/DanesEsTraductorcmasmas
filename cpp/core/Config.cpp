#include "Config.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace traductor {

Config::Config() {
    setDefaults();
    loadFromEnvironment();
}

bool Config::loadFromFile(const std::string& configPath) {
    try {
        std::ifstream file(configPath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json config;
        file >> config;
        file.close();
        
        // Parse JSON configuration
        if (config.contains("model_dir")) {
            modelDir_ = config["model_dir"];
        }
        if (config.contains("ct2_dir")) {
            ct2Dir_ = config["ct2_dir"];
        }
        if (config.contains("beam_size")) {
            beamSize_ = config["beam_size"];
        }
        if (config.contains("max_input_tokens")) {
            maxInputTokens_ = config["max_input_tokens"];
        }
        if (config.contains("default_max_new_tokens")) {
            defaultMaxNewTokens_ = config["default_max_new_tokens"];
        }
        if (config.contains("max_max_new_tokens")) {
            maxMaxNewTokens_ = config["max_max_new_tokens"];
        }
        if (config.contains("max_segment_chars")) {
            maxSegmentChars_ = config["max_segment_chars"];
        }
        if (config.contains("ct2_inter_threads")) {
            ct2InterThreads_ = config["ct2_inter_threads"];
        }
        if (config.contains("ct2_intra_threads")) {
            ct2IntraThreads_ = config["ct2_intra_threads"];
        }
        if (config.contains("source_lang")) {
            sourceLang_ = config["source_lang"];
        }
        if (config.contains("target_lang")) {
            targetLang_ = config["target_lang"];
        }
        if (config.contains("formal_da")) {
            formalDa_ = config["formal_da"];
        }
        if (config.contains("host")) {
            host_ = config["host"];
        }
        if (config.contains("port")) {
            port_ = config["port"];
        }
        if (config.contains("cache_size")) {
            cacheSize_ = config["cache_size"];
        }
        if (config.contains("max_batch_size")) {
            maxBatchSize_ = config["max_batch_size"];
        }
        if (config.contains("request_timeout")) {
            requestTimeout_ = config["request_timeout"];
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from file: " << e.what() << std::endl;
        return false;
    }
}

void Config::loadFromEnvironment() {
    // Load from environment variables with same names as Python settings
    if (const char* env = std::getenv("MODEL_DIR")) {
        modelDir_ = env;
    }
    if (const char* env = std::getenv("CT2_DIR")) {
        ct2Dir_ = env;
    }
    if (const char* env = std::getenv("BEAM_SIZE")) {
        beamSize_ = std::atoi(env);
    }
    if (const char* env = std::getenv("MAX_INPUT_TOKENS")) {
        maxInputTokens_ = std::atoi(env);
    }
    if (const char* env = std::getenv("DEFAULT_MAX_NEW_TOKENS")) {
        defaultMaxNewTokens_ = std::atoi(env);
    }
    if (const char* env = std::getenv("MAX_MAX_NEW_TOKENS")) {
        maxMaxNewTokens_ = std::atoi(env);
    }
    if (const char* env = std::getenv("MAX_SEGMENT_CHARS")) {
        maxSegmentChars_ = std::atoi(env);
    }
    if (const char* env = std::getenv("CT2_INTER_THREADS")) {
        ct2InterThreads_ = std::atoi(env);
    }
    if (const char* env = std::getenv("CT2_INTRA_THREADS")) {
        ct2IntraThreads_ = std::atoi(env);
    }
    if (const char* env = std::getenv("SOURCE_LANG")) {
        sourceLang_ = env;
    }
    if (const char* env = std::getenv("TARGET_LANG")) {
        targetLang_ = env;
    }
    if (const char* env = std::getenv("FORMAL_DA")) {
        formalDa_ = (std::string(env) == "true" || std::string(env) == "1");
    }
    if (const char* env = std::getenv("HOST")) {
        host_ = env;
    }
    if (const char* env = std::getenv("PORT")) {
        port_ = std::atoi(env);
    }
    if (const char* env = std::getenv("CACHE_SIZE")) {
        cacheSize_ = std::atoll(env);
    }
    if (const char* env = std::getenv("MAX_BATCH_SIZE")) {
        maxBatchSize_ = std::atoi(env);
    }
    if (const char* env = std::getenv("REQUEST_TIMEOUT")) {
        requestTimeout_ = std::atoi(env);
    }
}

void Config::setDefaults() {
    // Model paths - match Python defaults
    modelDir_ = "./models/nllb-600m";
    ct2Dir_ = "./models/nllb-600m-ct2-int8";
    
    // Translation parameters matching Python settings
    beamSize_ = 4;
    maxInputTokens_ = 4096;
    defaultMaxNewTokens_ = 256;
    maxMaxNewTokens_ = 8192;
    maxSegmentChars_ = 800;
    
    // CTranslate2 threading - conservative defaults
    ct2InterThreads_ = 4;
    ct2IntraThreads_ = 4;
    
    // Languages
    sourceLang_ = "spa_Latn";
    targetLang_ = "dan_Latn";
    formalDa_ = false;
    
    // Server
    host_ = "0.0.0.0";
    port_ = 8000;
    
    // Cache
    cacheSize_ = 1024;
    
    // Limits
    maxBatchSize_ = 16;
    requestTimeout_ = 300;
}

nlohmann::json Config::toJson() const {
    nlohmann::json config;
    config["model_dir"] = modelDir_;
    config["ct2_dir"] = ct2Dir_;
    config["beam_size"] = beamSize_;
    config["max_input_tokens"] = maxInputTokens_;
    config["default_max_new_tokens"] = defaultMaxNewTokens_;
    config["max_max_new_tokens"] = maxMaxNewTokens_;
    config["max_segment_chars"] = maxSegmentChars_;
    config["ct2_inter_threads"] = ct2InterThreads_;
    config["ct2_intra_threads"] = ct2IntraThreads_;
    config["source_lang"] = sourceLang_;
    config["target_lang"] = targetLang_;
    config["formal_da"] = formalDa_;
    config["host"] = host_;
    config["port"] = port_;
    config["cache_size"] = cacheSize_;
    config["max_batch_size"] = maxBatchSize_;
    config["request_timeout"] = requestTimeout_;
    return config;
}

} // namespace traductor
