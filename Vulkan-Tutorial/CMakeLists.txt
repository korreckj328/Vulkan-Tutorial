cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME VulkanTutorials)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)
add_subdirectory(glm)

OPTION(USE_D2D_WSI "build the project with direct 2 display swapchain" OFF)

find_package(Vulkan REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/glfw/include")

include_directories(${Vulkan_INCLUDE_DIRS})

include_directories("${CMAKE_SOURCE_DIR}/glm")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")

file(GLOB PROJECT_SOURCES *.h *.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw glm)

set (source "${CMAKE_SOURCE_DIR}/Assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/Assets")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} => ${destination}"
)
