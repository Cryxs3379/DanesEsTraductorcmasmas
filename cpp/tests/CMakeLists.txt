# Tests for Traductor Danés-Español

if(GTest_FOUND)
    # Test executable
    add_executable(traductor_tests
        test_translator_engine.cpp
        test_glossary.cpp
        test_postprocess.cpp
        test_segmenter.cpp
        test_lru_cache.cpp
    )
    
    # Link with core library and GTest
    target_link_libraries(traductor_tests PRIVATE
        traductor_core
        GTest::gtest
        GTest::gtest_main
    )
    
    # Include directories
    target_include_directories(traductor_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/core
    )
    
    # Run tests
    enable_testing()
    add_test(NAME TraductorTests COMMAND traductor_tests)
    
    message(STATUS "Tests configured - use 'ctest' to run")
else()
    message(STATUS "Google Test not found - tests will be skipped")
endif()
