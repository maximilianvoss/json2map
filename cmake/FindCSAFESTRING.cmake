find_path(CSAFESTRING_INCLUDE_DIR csafestring.h
        HINTS
        include
        )

FIND_LIBRARY(CSAFESTRING_LIBRARIES
        NAMES csafestring
        HINTS
        PATH_SUFFIXES lib64 lib
        )