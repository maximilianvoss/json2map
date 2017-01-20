cmake_minimum_required(VERSION 2.8)
project(json2map)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
include("${CMAKE_SOURCE_DIR}/cmake/include.cmake")

set(SOURCE_FILES
        jsmn.c
        jsmn.h
        json2map.c
        json2map.h
        map2json.c
        map2json.h
        stringlib.c
        stringlib.h
        debugging.h
        config.h)

#set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")

add_library(json2map ${SOURCE_FILES})
target_link_libraries(json2map ${CSAFESTRING_LIBRARY})
install(TARGETS json2map DESTINATION /usr/local/lib)
install(FILES json2map.h map2json.h jsmn.h DESTINATION /usr/local/include)

enable_testing()
add_subdirectory(test)
add_subdirectory(example)

