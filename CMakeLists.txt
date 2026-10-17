
cmake_minimum_required(VERSION 3.20)

project(rate_limiter
    VERSION 0.1.0
    LANGUAGES CXX
)

include(FetchContent)

# Dependencies
find_package(yaml-cpp CONFIG REQUIRED)

FetchContent_Declare(
    redis-plus-plus
    GIT_REPOSITORY https://github.com/sewenew/redis-plus-plus.git
    GIT_TAG 1.3.15
)

FetchContent_MakeAvailable(redis-plus-plus)

# Reusable library
add_library(rate_limiter
    src/rate_limiter.cpp
)

add_library(rate_limiter::rate_limiter ALIAS rate_limiter)

target_compile_features(rate_limiter PUBLIC cxx_std_20)

target_include_directories(rate_limiter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(rate_limiter
    PUBLIC
        yaml-cpp::yaml-cpp
        redis++::redis++
)

target_compile_options(rate_limiter PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
