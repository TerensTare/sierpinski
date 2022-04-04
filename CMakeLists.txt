cmake_minimum_required(VERSION 3.16)

project(
    sierpinski
    VERSION 0.1.0
    LANGUAGES CXX
)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "CMake should not be used to build in a directory that has a CMakeLists.txt file!!\n
        Please consider making a \"build\" subdirectory and run cmake .. from the \"build\" subdirectory.")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/StaticAnalyzers.cmake)

########################
# Find the dependencies #
########################

find_package(imgui CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# The engine SDK

add_executable(${PROJECT_NAME} main.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

set_project_warnings(${PROJECT_NAME})
enable_sanitizers(${PROJECT_NAME})

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    CXX_EXTENSIONS OFF
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(
    ${PROJECT_NAME}
    LINK_PUBLIC
    SDL2::SDL2 SDL2::SDL2main
    imgui::imgui
)