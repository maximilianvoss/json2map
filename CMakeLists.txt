cmake_minimum_required(VERSION 3.6)
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

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fstack-protector-strong -Wformat -Werror=format-security")

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(SOURCE_FILES ${SOURCE_FILES} test.c test.h example.c)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG")
    add_executable(json2map ${SOURCE_FILES})
    target_link_libraries(json2map ${CSAFESTRING_LIBRARY})
else (${CMAKE_BUILD_TYPE} STREQUAL Release)
    add_library(json2map ${SOURCE_FILES})
    target_link_libraries(json2map ${CSAFESTRING_LIBRARY})
    install(TARGETS json2map DESTINATION /usr/local/lib)
    install(FILES json2map.h map2json.h jsmn.h DESTINATION /usr/local/include)
endif (${CMAKE_BUILD_TYPE} STREQUAL Debug)

