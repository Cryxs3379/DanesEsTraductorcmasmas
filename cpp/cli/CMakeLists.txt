# CLI executable
add_executable(traductor_cli main.cpp)

# Link with core library
target_link_libraries(traductor_cli PRIVATE traductor_core)

# Set output directory
set_target_properties(traductor_cli PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(MSVC)
    target_compile_options(traductor_cli PRIVATE /W4)
else()
    target_compile_options(traductor_cli PRIVATE -Wall -Wextra)
endif()
