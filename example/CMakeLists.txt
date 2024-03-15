# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.13)

# Set the project name and version
project(piper_example VERSION 1.0 DESCRIPTION "Example application for the Piper project")

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add the piper library
add_subdirectory(.. piper)

# Add the executable
add_executable(piper_example main.cpp)

# Link the executable to the piper library
target_link_libraries(piper_example piper)

# Set the include directories
target_include_directories(piper_example PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/../src"
                           )

# Set the installation directory for the example executable
install(TARGETS piper_example DESTINATION ${CMAKE_INSTALL_PREFIX})