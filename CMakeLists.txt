cmake_minimum_required(VERSION 2.8)
project(json2map)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(CSAFESTRING REQUIRED)
include_directories(${CSAFESTRING_INCLUDE_DIR})

set(SOURCE_FILES
        jsmn.c
        jsmn.h
        json2map.c
        json2map.h
        map2json.c
        map2json.h
        stringlib.c
        stringlib.h
        logging.h
        config.h)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")

if (NOT TARGET json2map)
    add_library(json2map ${SOURCE_FILES})
    target_link_libraries(json2map ${CSAFESTRING_LIBRARIES})
    install(TARGETS json2map DESTINATION lib)
    install(FILES json2map.h map2json.h jsmn.h DESTINATION include)
endif ()

enable_testing()
add_subdirectory(test)
add_subdirectory(example)

