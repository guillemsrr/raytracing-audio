cmake_minimum_required(VERSION 3.20)
project(raytracing+audio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add SerraEngine as a subdirectory
add_subdirectory(../SerraEngine SerraEngineBuild)

# Find packages
find_package(SDL3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Game sources
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.frag src/*.vert src/*.comp)

add_executable(${PROJECT_NAME}
    ${GAME_SOURCES}
)

# Link to the engine target
target_link_libraries(${PROJECT_NAME} PRIVATE SerraEngine)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

# Include headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../SerraEngine/include
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/shaders DESTINATION ${CMAKE_BINARY_DIR})