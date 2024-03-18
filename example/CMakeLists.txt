# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.13)

# Set the project name and version
project(piper_cli VERSION 1.0 DESCRIPTION "Example application for the Piper project")

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the piper library
add_subdirectory(.. piper)

# Add the executable
add_executable(piper_cli main.c)

if(WIN32)
    target_link_libraries(piper_cli PRIVATE winmm)
endif()

# Link the executable to the piper library
target_link_libraries(piper_cli PRIVATE piper)

# Set the include directories
target_include_directories(piper_cli PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/../src"
                           )

# Ensure the executable can find the necessary shared libraries at runtime
# Set RPATH to $ORIGIN and its lib directory. $ORIGIN refers to the directory containing the executable.
set_target_properties(piper_cli PROPERTIES
                      INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/../lib64"
                      INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set the installation directory for the example executable
install(TARGETS piper_cli DESTINATION bin)  # Installing executable to `bin` directory under CMAKE_INSTALL_PREFIX
