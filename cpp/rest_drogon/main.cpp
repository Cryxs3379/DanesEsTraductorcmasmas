#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include "../core/TranslatorEngine.h"
#include "../core/Config.h"
#include "../core/Glossary.h"
#include <nlohmann/json.hpp>

#ifdef DROGON_FOUND
#include <drogon/drogon.h>
using namespace drogon;
#endif

namespace traductor {
    namespace rest {
        
        class RestServer {
        public:
            RestServer(const Config& config) : translator_(config) {}
            
            bool initialize() {
                std::cout << "Initializing REST server..." << std::endl;
                return translator_.initialize();
            }
            
            // Endpoint: POST /translate
            nlohmann::json handleTranslate(const nlohmann::json& request) {
                try {
                    std::vector<std::string> texts;
                    if (request.contains("text")) {
                        if (request["text"].is_string()) {
                            texts.push_back(request["text"]);
                        } else if (request["text"].is_array()) {
                            for (const auto& item : request["text"]) {
                                texts.push_back(item.get<std::string>());
                            }
                        }
                    }
                    
                    std::string direction = request.value("direction", "es-da");
                    int maxTokens = request.value("max_new_tokens", -1);
                    bool formal = request.value("formal", false);
                    
                    // Load glossary if provided
                    Glossary::TermMap glossary;
                    if (request.contains("glossary") && request["glossary"].is_object()) {
                        for (const auto& [key, value] : request["glossary"].items()) {
                            if (value.is_string()) {
                                glossary[key] = value.get<std::string>();
                            }
                        }
                    }
                    
                    auto result = translator_.translate(texts, direction, maxTokens, formal, glossary);
                    
                    nlohmann::json response;
                    response["provider"] = "nllb-ct2-int8";
                    response["direction"] = result.direction;
                    response["source"] = result.sourceLang;
                    response["target"] = result.targetLang;
                    response["translations"] = result.translations;
                    
                    return response;
                    
                } catch (const std::exception& e) {
                    nlohmann::json error;
                    error["error"] = e.what();
                    return error;
                }
            }
            
            // Endpoint: POST /translate/html
            nlohmann::json handleTranslateHtml(const nlohmann::json& request) {
                try {
                    std::string html = request.value("html", "");
                    std::string direction = request.value("direction", "es-da");
                    int maxTokens = request.value("max_new_tokens", -1);
                    bool formal = request.value("formal", false);
                    
                    // Load glossary
                    Glossary::TermMap glossary;
                    if (request.contains("glossary") && request["glossary"].is_object()) {
                        for (const auto& [key, value] : request["glossary"].items()) {
                            if (value.is_string()) {
                                glossary[key] = value.get<std::string>();
                            }
                        }
                    }
                    
                    std::string result = translator_.translateHtml(html, direction, maxTokens, formal, glossary);
                    
                    nlohmann::json response;
                    response["provider"] = "nllb-ct2-int8";
                    response["direction"] = direction;
                    response["source"] = getLanguageCode(direction, true);
                    response["target"] = getLanguageCode(direction, false);
                    response["html"] = result;
                    
                    return response;
                    
                } catch (const std::exception& e) {
                    nlohmann::json error;
                    error["error"] = e.what();
                    return error;
                }
            }
            
            // Endpoint: GET /health
            nlohmann::json handleHealth() {
                auto health = translator_.getHealthInfo();
                
                nlohmann::json response;
                response["status"] = health.modelLoaded ? "healthy" : "unhealthy";
                response["model_loaded"] = health.modelLoaded;
                response["ready_for_translation"] = health.modelLoaded && health.tokenizerLoaded;
                response["last_error"] = health.lastError;
                response["cache"] = {
                    {"size", health.cacheSize},
                    {"hit_rate", health.cacheHitRate}
                };
                
                return response;
            }
            
        private:
            TranslatorEngine translator_;
            
            std::string getLanguageCode(const std::string& direction, bool isSource) {
                if (direction == "es-da") {
                    return isSource ? "spa_Latn" : "dan_Latn";
                } else {
                    return isSource ? "dan_Latn" : "spa_Latn";
                }
            }
        };
        
    } // namespace rest
} // namespace traductor

#ifdef DROGON_FOUND
// Global translator instance (solo cuando Drogon está disponible)
static std::unique_ptr<traductor::TranslatorEngine> g_translator;

// Health endpoint
void healthHandler(const HttpRequestPtr& /*req*/, std::function<void(const HttpResponsePtr&)>&& callback) {
    auto health = g_translator->getHealthInfo();
    
    Json::Value response;
    response["status"] = health.modelLoaded ? "healthy" : "unhealthy";
    response["model_loaded"] = health.modelLoaded;
    response["ready_for_translation"] = health.modelLoaded && health.tokenizerLoaded;
    response["last_error"] = health.lastError;
    response["cache"]["size"] = static_cast<int>(health.cacheSize);
    response["cache"]["hit_rate"] = health.cacheHitRate;
    
    auto resp = HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

// Translate endpoint
void translateHandler(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        auto json = req->getJsonObject();
        if (!json) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Invalid JSON");
            callback(resp);
            return;
        }
        
        std::vector<std::string> texts;
        if (json->isMember("text")) {
            const auto& textValue = (*json)["text"];
            if (textValue.isString()) {
                texts.push_back(textValue.asString());
            } else if (textValue.isArray()) {
                for (const auto& item : textValue) {
                    texts.push_back(item.asString());
                }
            }
        }
        
        std::string direction = json->get("direction", "es-da").asString();
        int maxTokens = json->get("max_new_tokens", -1).asInt();
        bool formal = json->get("formal", false).asBool();
        
        // Load glossary if provided
        traductor::Glossary::TermMap glossary;
        if (json->isMember("glossary") && (*json)["glossary"].isObject()) {
            const auto& glossObj = (*json)["glossary"];
            for (const auto& key : glossObj.getMemberNames()) {
                glossary[key] = glossObj[key].asString();
            }
        }
        
        auto result = g_translator->translate(texts, direction, maxTokens, formal, glossary);
        
        Json::Value response;
        response["provider"] = "nllb-ct2-int8";
        response["direction"] = result.direction;
        response["source"] = result.sourceLang;
        response["target"] = result.targetLang;
        response["latency_ms"] = result.latency_ms;
        response["used_cache"] = result.usedCache;
        
        Json::Value translations(Json::arrayValue);
        for (const auto& translation : result.translations) {
            translations.append(translation);
        }
        response["translations"] = translations;
        
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        
    } catch (const std::exception& e) {
        Json::Value error;
        error["error"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

// HTML translate endpoint
void translateHtmlHandler(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        auto json = req->getJsonObject();
        if (!json) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Invalid JSON");
            callback(resp);
            return;
        }
        
        std::string html = json->get("html", "").asString();
        std::string direction = json->get("direction", "es-da").asString();
        int maxTokens = json->get("max_new_tokens", -1).asInt();
        bool formal = json->get("formal", false).asBool();
        
        // Load glossary
        traductor::Glossary::TermMap glossary;
        if (json->isMember("glossary") && (*json)["glossary"].isObject()) {
            const auto& glossObj = (*json)["glossary"];
            for (const auto& key : glossObj.getMemberNames()) {
                glossary[key] = glossObj[key].asString();
            }
        }
        
        std::string result = g_translator->translateHtml(html, direction, maxTokens, formal, glossary);
        
        Json::Value response;
        response["provider"] = "nllb-ct2-int8";
        response["direction"] = direction;
        response["html"] = result;
        
        auto resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
        
    } catch (const std::exception& e) {
        Json::Value error;
        error["error"] = e.what();
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}
#endif

int main(int argc, char* argv[]) {
#ifdef DROGON_FOUND
    std::cout << "Starting REST API server..." << std::endl;
    
    // Load configuration
    traductor::Config config;
    
    // Initialize translator
    g_translator = std::make_unique<traductor::TranslatorEngine>(config);
    
    if (!g_translator->initialize()) {
        auto health = g_translator->getHealthInfo();
        std::cerr << "Failed to initialize translator: " << health.lastError << std::endl;
        return 1;
    }
    
    std::cout << "Translator initialized successfully" << std::endl;
    
    // Configure Drogon
    app().setLogLevel(trantor::Logger::kInfo);
    
    // Set up routes
    app().registerHandler("/health", &healthHandler, {Get});
    app().registerHandler("/translate", &translateHandler, {Post});
    app().registerHandler("/translate/html", &translateHtmlHandler, {Post});
    
    // Set server address and port from config
    app()
        .setClientMaxBodySize(1024 * 1024)  // 1MB max request size
        .addListener(config.host(), config.port())
        .run();
#else
    std::cout << "REST Server - Drogon not found, using stub implementation" << std::endl;
    
    traductor::Config config;
    traductor::rest::RestServer server(config);
    
    if (!server.initialize()) {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }
    
    std::cout << "Server stub initialized successfully" << std::endl;
    std::cout << "Note: Install Drogon for full REST server functionality." << std::endl;
#endif
    
    return 0;
}
