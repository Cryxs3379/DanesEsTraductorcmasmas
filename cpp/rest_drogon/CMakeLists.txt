# REST API server (optional - requires Drogon)
if(Drogon_FOUND)
    add_executable(traductor_rest main.cpp)
    
    target_compile_definitions(traductor_rest PRIVATE DROGON_FOUND)
    
    target_link_libraries(traductor_rest PRIVATE
        traductor_core
        Drogon::Drogon
    )
    
    set_target_properties(traductor_rest PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    if(MSVC)
        target_compile_options(traductor_rest PRIVATE /W4)
    else()
        target_compile_options(traductor_rest PRIVATE -Wall -Wextra)
    endif()
else()
    message(STATUS "Drogon not found - building REST stub implementation")
    add_executable(traductor_rest main.cpp)
    
    target_link_libraries(traductor_rest PRIVATE traductor_core)
    
    set_target_properties(traductor_rest PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    if(MSVC)
        target_compile_options(traductor_rest PRIVATE /W4)
    else()
        target_compile_options(traductor_rest PRIVATE -Wall -Wextra)
    endif()
endif()
