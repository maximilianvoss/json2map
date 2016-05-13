cmake_minimum_required(VERSION 3.0)
project(json2map)

# Enable this for debug information and testing purposes
add_definitions(-DDEBUG)

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
        example.c
        test.c
        test.h config.h)

add_executable(json2map ${SOURCE_FILES})
