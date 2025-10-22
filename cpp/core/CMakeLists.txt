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

# Find required packages
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)

# SentencePiece and CTranslate2 are found in parent CMake

# Link libraries
target_link_libraries(traductor_core PUBLIC
    nlohmann_json::nlohmann_json
    Threads::Threads
    sentencepiece::sentencepiece
    protobuf::libprotobuf
    absl::strings
    absl::flat_hash_map
    absl::hash
    absl::base
    absl::flags
    absl::flags_parse
    absl::flags_usage
)

# Compile definitions - enable SentencePiece and CTranslate2 if found
target_compile_definitions(traductor_core PRIVATE HAVE_SENTENCEPIECE)

if(ctranslate2_FOUND)
    target_link_libraries(traductor_core PUBLIC ctranslate2::ctranslate2)
    target_compile_definitions(traductor_core PRIVATE HAVE_CTRANSLATE2)
    message(STATUS "CTranslate2 found - enabling real translation")
else()
    message(WARNING "CTranslate2 not found - using simplified mode (hardcoded translations)")
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
