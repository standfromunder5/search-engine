cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_VERSION_MINIMUM 3.14)
project(search-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(search_lib
    src/document-reader.cpp
    src/document.cpp
    src/tokenizer.cpp
)

target_include_directories(search_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(search_lib PRIVATE -Wall -Wextra)

add_executable(search apps/main.cpp)

target_link_libraries(search PRIVATE search_lib)
target_compile_options(search PRIVATE -Wall -Wextra)

# enable_testing()
# add_subdirectory(tests)
include(FetchContent)

FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3)
FetchContent_Declare(rapidcsv GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git GIT_TAG v8.84)
FetchContent_Declare(OpenXLSX GIT_REPOSITORY https://github.com/troldal/OpenXLSX.git GIT_TAG master)

set(OPENXLSX_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(OPENXLSX_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(OPENXLSX_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(json rapidcsv OpenXLSX)

target_link_libraries(search_lib PUBLIC  nlohmann_json::nlohmann_json PRIVATE rapidcsv OpenXLSX::OpenXLSX)