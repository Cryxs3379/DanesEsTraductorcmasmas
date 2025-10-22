cmake_minimum_required(VERSION 3.20)

project(traductor-danes-espanol 
    VERSION 1.0.0 
    DESCRIPTION "C++ Native Danish-Spanish Translator"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find packages
find_package(Threads REQUIRED)

# vcpkg packages
find_package(nlohmann_json REQUIRED)

# Core ML dependencies
# SentencePiece - find manually since vcpkg doesn't provide CMake config
find_path(SENTENCEPIECE_INCLUDE_DIR sentencepiece_processor.h
    HINTS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES include
)
find_library(SENTENCEPIECE_LIBRARY
    NAMES sentencepiece
    HINTS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES lib
)

if(SENTENCEPIECE_INCLUDE_DIR AND SENTENCEPIECE_LIBRARY)
    set(SENTENCEPIECE_FOUND TRUE CACHE BOOL "SentencePiece found")
    add_library(sentencepiece::sentencepiece UNKNOWN IMPORTED)
    set_target_properties(sentencepiece::sentencepiece PROPERTIES
        IMPORTED_LOCATION ${SENTENCEPIECE_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${SENTENCEPIECE_INCLUDE_DIR}
    )
    message(STATUS "Found SentencePiece: ${SENTENCEPIECE_LIBRARY}")
else()
    message(WARNING "SentencePiece not found - will use simplified mode")
    set(SENTENCEPIECE_FOUND FALSE CACHE BOOL "SentencePiece found")
endif()

# CTranslate2 - try multiple ways to find it
find_package(unofficial-ctranslate2 QUIET)
if(NOT unofficial-ctranslate2_FOUND)
    find_package(ctranslate2 QUIET)
endif()

# Optional GUI dependencies
find_package(Qt6 QUIET COMPONENTS Core Widgets Network)

# Optional REST API dependencies
find_package(Drogon QUIET)

# HTML parsing library (optional)
find_package(Lexbor QUIET)

# Google Test (for testing)
find_package(GTest QUIET)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

# Core library
add_subdirectory(core)

# CLI target
add_subdirectory(cli)

# Qt Desktop GUI target
if(Qt6_FOUND)
    add_subdirectory(desktop_qt)
endif()

# REST API target (optional)
if(Drogon_FOUND)
    add_subdirectory(rest_drogon)
endif()

# Tests (optional)
if(GTest_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()

# Post-build actions: Copy models and assets
add_custom_target(copy_models
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/models"
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_SOURCE_DIR}/../models/nllb-600m-ct2-int8" 
        "${CMAKE_BINARY_DIR}/bin/models/nllb-600m-ct2-int8"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/../models/nllb-600m/sentencepiece.bpe.model"
        "${CMAKE_BINARY_DIR}/bin/models/"
    COMMENT "Copying model files to bin directory"
)

# Set copy_models as dependency for all executables
add_dependencies(traductor_cli copy_models)

if(Qt6_FOUND)
    add_dependencies(traductor_gui copy_models)
endif()

if(Drogon_FOUND)
    add_dependencies(traductor_rest copy_models)
endif()

# Windows-specific settings
if(WIN32)
    # Copy DLLs to bin directory
    if(EXISTS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin")
        add_custom_command(TARGET copy_models POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin"
                "${CMAKE_BINARY_DIR}/bin"
        )
    endif()
    
    # Copy Qt6 platform plugins (needed for GUI)
    if(Qt6_FOUND)
        add_custom_command(TARGET copy_models POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "${CMAKE_BINARY_DIR}/bin/platforms"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/Qt6/plugins/platforms"
                "${CMAKE_BINARY_DIR}/bin/platforms"
            COMMENT "Copying Qt6 platform plugins"
        )
    endif()
    
    # Copy translation models (needed for all executables)
    add_custom_command(TARGET copy_models POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/../models"
            "${CMAKE_BINARY_DIR}/bin/models"
        COMMENT "Copying translation models"
    )
endif()
