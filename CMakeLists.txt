cmake_minimum_required(VERSION 3.13)

project(
  piper
  VERSION 1.0.0
  DESCRIPTION "Piper neural text to speech system"
  HOMEPAGE_URL "https://github.com/danemadsen/piper.cpp"
  LANGUAGES CXX
)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

if(MSVC)
  # Force compiler to use UTF-8 for IPA constants
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
elseif(NOT APPLE)
  # Linux flags
  string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wl,-rpath,'$ORIGIN'")
  string(APPEND CMAKE_C_FLAGS " -Wall -Wextra")
endif()

# Conditionally set the library type based on the option
if(BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
else()
  set(LIB_TYPE STATIC)
endif()

# Use the LIB_TYPE variable to determine the type of library to build
add_library(
  piper 
  ${LIB_TYPE} 
  src/piper.cpp
  src/phonemize.cpp
  src/phoneme_ids.cpp
)

# ---- fmt ---

if(NOT DEFINED FMT_DIR)
  set(FMT_VERSION "10.0.0")
  set(FMT_DIR "${CMAKE_CURRENT_BINARY_DIR}/fi")

  include(ExternalProject)
  ExternalProject_Add(
    fmt_external
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/f"
    URL "https://github.com/fmtlib/fmt/archive/refs/tags/${FMT_VERSION}.zip"
    CMAKE_ARGS 
      -DCMAKE_INSTALL_PREFIX:PATH=${FMT_DIR}
      -DFMT_TEST:BOOL=OFF  # Don't build all the tests
      -DCMAKE_C_FLAGS="-fPIC"
      -DCMAKE_CXX_FLAGS="-fPIC"
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON
  )
  add_dependencies(piper fmt_external)
endif()

# ---- spdlog ---

if(NOT DEFINED SPDLOG_DIR)
  set(SPDLOG_DIR "${CMAKE_CURRENT_BINARY_DIR}/si")
  set(SPDLOG_VERSION "1.12.0")
  ExternalProject_Add(
    spdlog_external
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/s"
    URL "https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.zip"
    CMAKE_ARGS 
      -DCMAKE_INSTALL_PREFIX:PATH=${SPDLOG_DIR}
      -DCMAKE_C_FLAGS="-fPIC"
      -DCMAKE_CXX_FLAGS="-fPIC"
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON
  )
  add_dependencies(piper spdlog_external)
endif()

# ---- onnxruntime ---

# Look for onnxruntime files in <root>/lib
if(NOT DEFINED ONNXRUNTIME_DIR)
    if(NOT DEFINED ONNXRUNTIME_VERSION)
        set(ONNXRUNTIME_VERSION "1.14.1")
    endif()

    if(WIN32)
        # Windows x86-64
        set(ONNXRUNTIME_PREFIX "onnxruntime-win-x64-${ONNXRUNTIME_VERSION}")
        set(ONNXRUNTIME_EXT "zip")
    elseif (APPLE)
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
            # MacOS x86-64
            set(ONNXRUNTIME_PREFIX "onnxruntime-osx-x86_64-${ONNXRUNTIME_VERSION}")
        elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL arm64)
            # MacOS Apple Silicon
            set(ONNXRUNTIME_PREFIX "onnxruntime-osx-arm64-${ONNXRUNTIME_VERSION}")
        else()
            message(FATAL_ERROR "Unsupported architecture for onnxruntime")
        endif()

        set(ONNXRUNTIME_EXT "tgz")
    else()
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
            # Linux x86-64
            set(ONNXRUNTIME_PREFIX "onnxruntime-linux-x64-${ONNXRUNTIME_VERSION}")
        elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL aarch64)
            # Linux ARM 64-bit
            set(ONNXRUNTIME_PREFIX "onnxruntime-linux-aarch64-${ONNXRUNTIME_VERSION}")
        elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL armv7l)
            # Linux ARM 32-bit
            set(ONNXRUNTIME_PREFIX "onnxruntime-linux-arm32-${ONNXRUNTIME_VERSION}")
            set(ONNXRUNTIME_URL "https://github.com/synesthesiam/prebuilt-apps/releases/download/v1.0/onnxruntime-linux-arm32-${ONNXRUNTIME_VERSION}.tgz")
        else()
            message(FATAL_ERROR "Unsupported architecture for onnxruntime")
        endif()

        set(ONNXRUNTIME_EXT "tgz")
    endif()

    if(NOT DEFINED ONNXRUNTIME_URL)
        set(ONNXRUNTIME_URL "https://github.com/microsoft/onnxruntime/releases/download/v${ONNXRUNTIME_VERSION}/${ONNXRUNTIME_PREFIX}.${ONNXRUNTIME_EXT}")
    endif()

    set(ONNXRUNTIME_FILENAME "${ONNXRUNTIME_PREFIX}.${ONNXRUNTIME_EXT}")
    set(ONNXRUNTIME_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/${ONNXRUNTIME_PREFIX}")

    if(NOT EXISTS "${ONNXRUNTIME_DIR}")
        if(NOT EXISTS "download/${ONNXRUNTIME_FILENAME}")
            # Download onnxruntime release
            message("Downloading ${ONNXRUNTIME_URL}")
            file(DOWNLOAD "${ONNXRUNTIME_URL}" "download/${ONNXRUNTIME_FILENAME}")
        endif()

        # Extract .zip or .tgz to a directory like lib/onnxruntime-linux-x64-1.14.1/
        file(ARCHIVE_EXTRACT INPUT "download/${ONNXRUNTIME_FILENAME}" DESTINATION "${CMAKE_CURRENT_LIST_DIR}/lib")
    endif()
endif()

# ---- espeak-ng ---

if(NOT DEFINED ESPEAK_NG_DIR)
    set(ESPEAK_NG_DIR "${CMAKE_CURRENT_BINARY_DIR}/ei")

    include(ExternalProject)
    ExternalProject_Add(
        espeak_ng_external
        PREFIX "${CMAKE_CURRENT_BINARY_DIR}/e"
        URL "https://github.com/rhasspy/espeak-ng/archive/0f65aa301e0d6bae5e172cc74197d32a6182200f.zip"
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${ESPEAK_NG_DIR}
            -DUSE_ASYNC:BOOL=OFF
            -DBUILD_SHARED_LIBS:BOOL=ON
            -DUSE_MBROLA:BOOL=OFF
            -DUSE_LIBSONIC:BOOL=OFF
            -DUSE_LIBPCAUDIO:BOOL=OFF
            -DUSE_KLATT:BOOL=OFF
            -DUSE_SPEECHPLAYER:BOOL=OFF
            -DEXTRA_cmn:BOOL=ON
            -DEXTRA_ru:BOOL=ON
            -DCMAKE_C_FLAGS="-D_FILE_OFFSET_BITS=64 -fPIC"
            -DCMAKE_CXX_FLAGS="-fPIC"
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    )
    add_dependencies(piper espeak_ng_external)
endif()

# ---- Install ----

if((NOT MSVC) AND (NOT APPLE))
  # Linux flags
  string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wl,-rpath,'$ORIGIN'")
  string(APPEND CMAKE_C_FLAGS " -Wall -Wextra")
endif()

target_link_libraries(piper
  fmt
  spdlog
  espeak-ng
  onnxruntime
)

target_link_directories(piper PUBLIC
  ${FMT_DIR}/lib
  ${SPDLOG_DIR}/lib
  ${ESPEAK_NG_DIR}/lib
  ${ONNXRUNTIME_DIR}/lib
)

target_include_directories(piper PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
  ${FMT_DIR}/include
  ${SPDLOG_DIR}/include
  ${ESPEAK_NG_DIR}/include
  ${ONNXRUNTIME_DIR}/include
)

# ---- Declare install targets ----

include(GNUInstallDirs)

install(
  TARGETS piper
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # for Windows
)

# Dependencies
install(
    DIRECTORY ${ESPEAK_NG_DIR}/
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "*.so*"
)

install(
    DIRECTORY ${ONNXRUNTIME_DIR}/lib/
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "*.so*"
)

install(
    DIRECTORY ${ONNXRUNTIME_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# This line seems to be redundant with the ONNX Runtime library files installation above,
# so consider removing it if ${ONNXRUNTIME_DIR}/lib/ already covers the required files.
# install(
#     DIRECTORY ${ONNXRUNTIME_DIR}/lib/
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     FILES_MATCHING
#     PATTERN "*.lib"
# )