cmake_minimum_required(VERSION 3.24)
set(PROJECT_NAME "OpenGLRenderer")

project("${PROJECT_NAME}" VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(SOURCE_PATH "${PROJECT_SOURCE_DIR}/source" )
set(INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include")
set(EXTERNAL_PATH "${PROJECT_SOURCE_DIR}/extern")

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false "${SOURCE_PATH}/*.c*" "${SOURCE_PATH}/*.h*" "${INCLUDE_PATH}/*.cpp" "${INCLUDE_PATH}/*.h*" "${INCLUDE_PATH}/*.hpp" "${EXTERNAL_PATH}/*.cpp" "${EXTERNAL_PATH}/*.h*" "${EXTERNAL_PATH}/*.hpp")

add_executable("${PROJECT_NAME}" ${SOURCE_FILES})
set_target_properties("${PROJECT_NAME}" PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING;"
        "_CRT_SECURE_NO_WARNINGS;"
        "NOMINMAX;"
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTERNAL_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib)
target_precompile_headers(${PROJECT_NAME} PRIVATE include/PCH.hpp)

#ifdef DEBUG

# vcpkg dependencies
#toml++
#GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE glm::glm)
#GLFW3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE glfw)

find_package(glad CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE glad::glad)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE nlohmann_json::nlohmann_json)

find_package(Stb REQUIRED)
target_include_directories("${PROJECT_NAME}" PRIVATE ${Stb_INCLUDE_DIR})