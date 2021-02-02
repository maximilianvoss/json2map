find_path(JSON2MAP_INCLUDE_DIR json2map.h
        HINTS
        include
        )

FIND_LIBRARY(JSON2MAP_LIBRARIES
        NAMES json2map
        HINTS
        PATH_SUFFIXES lib64 lib
        )