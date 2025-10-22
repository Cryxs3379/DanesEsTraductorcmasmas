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
# SentencePiece - manual detection (vcpkg doesn't provide CMake config)
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
    set(sentencepiece_FOUND TRUE)
    add_library(sentencepiece::sentencepiece UNKNOWN IMPORTED)
    set_target_properties(sentencepiece::sentencepiece PROPERTIES
        IMPORTED_LOCATION ${SENTENCEPIECE_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${SENTENCEPIECE_INCLUDE_DIR}
    )
    message(STATUS "Found SentencePiece: ${SENTENCEPIECE_LIBRARY}")
else()
    message(FATAL_ERROR "SentencePiece not found")
endif()

# CTranslate2 - optional
find_package(ctranslate2 CONFIG QUIET)

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
add_custom_target(copy_models ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/../models"
            "${CMAKE_BINARY_DIR}/bin/models"
    COMMENT "Copying translation models"
)

# Set copy_models as dependency for all executables
add_dependencies(traductor_cli copy_models)

if(Qt6_FOUND)
    add_dependencies(traductor_gui copy_models)
endif()

if(Drogon_FOUND)
    add_dependencies(traductor_rest copy_models)
endif()

# Deploy Qt6 platform plugins (robust path detection)
if(Qt6_FOUND)
    set(_qt_plugins_dir_A "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/plugins/platforms")
    set(_qt_plugins_dir_B "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/Qt6/plugins/platforms")
    
    if(EXISTS "${_qt_plugins_dir_A}")
        set(_qt_src_platforms "${_qt_plugins_dir_A}")
    elseif(EXISTS "${_qt_plugins_dir_B}")
        set(_qt_src_platforms "${_qt_plugins_dir_B}")
    else()
        set(_qt_src_platforms "")
    endif()
    
    if(_qt_src_platforms)
        add_custom_target(deploy_qt ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/platforms"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${_qt_src_platforms}"
                    "${CMAKE_BINARY_DIR}/bin/platforms"
            COMMENT "Copying Qt6 platform plugins"
        )
        add_dependencies(traductor_gui deploy_qt)
    else()
        message(WARNING "Qt6 found but platform plugins path not located in vcpkg; GUI may not start outside build tree.")
    endif()
endif()
