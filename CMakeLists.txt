cmake_minimum_required(VERSION 4.0)

project(rate_limiter
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

find_package(yaml-cpp CONFIG REQUIRED)

include(FetchContent)
FetchContent_Declare(
    redis-plus-plus
    GIT_REPOSITORY https://github.com/sewenew/redis-plus-plus.git
    GIT_TAG 1.3.15
)
FetchContent_MakeAvailable(redis-plus-plus)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if(APP_SOURCES)
    add_executable(rate_limiter ${APP_SOURCES})
    target_include_directories(rate_limiter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(rate_limiter PRIVATE
        yaml-cpp::yaml-cpp
        redis++::redis++
    )

    target_compile_options(rate_limiter PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
endif()
