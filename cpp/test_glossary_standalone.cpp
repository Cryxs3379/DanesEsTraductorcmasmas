#include <iostream>
#include <string>
#include "core/Glossary.h"

int main() {
    std::cout << "=== PRUEBA DEL MÓDULO GLOSARIO CORREGIDO ===" << std::endl;
    
    // Crear instancia del glosario
    traductor::Glossary glossary;
    
    // Cargar términos de prueba
    std::string glossaryText = 
        "hola=hej\n"
        "mundo=verden\n"
        "gracias=tak\n"
        "Por favor=vær venlig\n";  // Test case-insensitive
    
    if (!glossary.loadFromString(glossaryText)) {
        std::cerr << "ERROR: No se pudo cargar el glosario" << std::endl;
        return 1;
    }
    
    std::cout << "Glosario cargado con " << glossary.size() << " términos" << std::endl;
    
    // Texto de prueba que incluye términos, URLs y números
    std::string testText = "Hola mundo, visita https://ejemplo.com y escribe a test@email.com el 123-456-7890";
    std::cout << "\nTexto original: " << testText << std::endl;
    
    // Aplicar pre-procesamiento
    std::string processedText = glossary.applyPreProcessing(testText);
    std::cout << "Después de pre-procesamiento: " << processedText << std::endl;
    
    // Simular traducción (normalmente aquí iría el motor de traducción)
    // Por ahora solo simular que los términos se traducen
    std::string translatedText = processedText;
    // Simular que el motor de traducción tradujo "mundo" pero mantenemos los placeholders
    
    // Aplicar post-procesamiento
    std::string finalText = glossary.applyPostProcessing(translatedText);
    std::cout << "Después de post-procesamiento: " << finalText << std::endl;
    
    // Verificar que las URLs y emails se preservan
    bool hasUrl = finalText.find("https://ejemplo.com") != std::string::npos;
    bool hasEmail = finalText.find("test@email.com") != std::string::npos;
    bool hasNumber = finalText.find("123-456-7890") != std::string::npos;
    
    std::cout << "\n=== RESULTADOS ===" << std::endl;
    std::cout << "URL preservada: " << (hasUrl ? "✅ SÍ" : "❌ NO") << std::endl;
    std::cout << "Email preservado: " << (hasEmail ? "✅ SÍ" : "❌ NO") << std::endl;
    std::cout << "Número preservado: " << (hasNumber ? "✅ SÍ" : "❌ NO") << std::endl;
    
    // Verificar que los términos se marcaron correctamente
    bool hasTermMarkers = processedText.find("[[TERM::") != std::string::npos;
    std::cout << "Marcadores TERM aplicados: " << (hasTermMarkers ? "✅ SÍ" : "❌ NO") << std::endl;
    
    std::cout << "\n=== TEST COMPLETADO ===" << std::endl;
    
    return 0;
}
