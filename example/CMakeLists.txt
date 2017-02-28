cmake_minimum_required(VERSION 2.8)

set(SOURCE_FILES
        example.c)

add_executable(json2map_example ${SOURCE_FILES})
target_link_libraries(json2map_example json2map)
