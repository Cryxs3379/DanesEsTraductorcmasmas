#include <iostream>
#include <string>
#include <regex>

// Simular la función translateSegmentSimple para probar
std::string translateSegmentSimple(const std::string& segment, 
                                const std::string& direction, 
                                bool formal) {
    std::string result = segment;
    
    if (direction == "es-da") {
        result = std::regex_replace(result, std::regex("Hola", std::regex_constants::icase), "Hej");
        result = std::regex_replace(result, std::regex("mundo", std::regex_constants::icase), "verden");
        result = std::regex_replace(result, std::regex("buenos", std::regex_constants::icase), "gode");
        result = std::regex_replace(result, std::regex("días", std::regex_constants::icase), "dage");
        result = std::regex_replace(result, std::regex("gracias", std::regex_constants::icase), "tak");
        result = std::regex_replace(result, std::regex("por", std::regex_constants::icase), "for");
        result = std::regex_replace(result, std::regex("tu", std::regex_constants::icase), "din");
        result = std::regex_replace(result, std::regex("ayuda", std::regex_constants::icase), "hjælp");
        result = std::regex_replace(result, std::regex("que", std::regex_constants::icase), "hvad");
        result = std::regex_replace(result, std::regex("tal", std::regex_constants::icase), "sådan");
    }
    
    return result;
}

int main() {
    std::cout << "=== TEST DE TRADUCCIÓN ===" << std::endl;
    
    std::vector<std::string> tests = {
        "Hola mundo",
        "Buenos días", 
        "Gracias por tu ayuda",
        "¿Qué tal?",
        "Hola mundo que tal"
    };
    
    for (const auto& test : tests) {
        std::string result = translateSegmentSimple(test, "es-da", false);
        std::cout << "Input:  \"" << test << "\"" << std::endl;
        std::cout << "Output: \"" << result << "\"" << std::endl;
        std::cout << "---" << std::endl;
    }
    
    return 0;
}
