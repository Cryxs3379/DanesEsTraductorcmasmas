# Qt Desktop GUI
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# GUI executable
add_executable(traductor_gui
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

# Link Qt libraries
target_link_libraries(traductor_gui PRIVATE
    traductor_core
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

# Set output directory
set_target_properties(traductor_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(MSVC)
    target_compile_options(traductor_gui PRIVATE /W4)
else()
    target_compile_options(traductor_gui PRIVATE -Wall -Wextra)
endif()
