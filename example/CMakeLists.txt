cmake_minimum_required(VERSION 2.8)

set(SOURCE_FILES
        example.c)

add_executable(${CMAKE_PROJECT_NAME}_example ${SOURCE_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME}_example json2map)
