# Core library CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# Core library
add_library(traductor_core STATIC
    Config.cpp
    Config.h
    Tokenizer.cpp
    Tokenizer.h
    LRUCache.cpp
    LRUCache.h
    Segmenter.cpp
    Segmenter.h
    Glossary.cpp
    Glossary.h
    PostprocessDA.cpp
    PostprocessDA.h
    PostprocessES.cpp
    PostprocessES.h
    TranslatorEngine.cpp
    TranslatorEngine.h
)

# Target includes
target_include_directories(traductor_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(traductor_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(traductor_core PUBLIC
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# Link SentencePiece if found
if(SENTENCEPIECE_FOUND)
    target_link_libraries(traductor_core PUBLIC sentencepiece::sentencepiece)
    target_compile_definitions(traductor_core PRIVATE HAVE_SENTENCEPIECE)
else()
    message(WARNING "SentencePiece not available - core will use simplified tokenization")
    target_compile_definitions(traductor_core PRIVATE SIMPLIFIED_MODE)
endif()

# Link CTranslate2 if available
if(unofficial-ctranslate2_FOUND)
    target_link_libraries(traductor_core PUBLIC ctranslate2::ctranslate2)
    target_compile_definitions(traductor_core PRIVATE HAVE_CTRANSLATE2)
elseif(ctranslate2_FOUND)
    target_link_libraries(traductor_core PUBLIC ctranslate2)
    target_compile_definitions(traductor_core PRIVATE HAVE_CTRANSLATE2)
else()
    message(WARNING "CTranslate2 not found - using simplified mode")
    target_compile_definitions(traductor_core PRIVATE SIMPLIFIED_MODE)
endif()

# Compiler-specific options
target_compile_features(traductor_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(traductor_core PRIVATE /W4)
else()
    target_compile_options(traductor_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set output directories
set_target_properties(traductor_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
