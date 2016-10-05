#ifndef __JSON2MAP_DEBUGGING_H__
#define __JSON2MAP_DEBUGGING_H__

#ifndef DEBUG
#define DEBUG_TEXT(fmt, ...) printf(fmt, __VA_ARGS__); printf("\n")
#define DEBUG_PUT(fmt) printf(fmt); printf("\n")
#else
#define DEBUG_TEXT(fmt, ...)
#define DEBUG_PUT(fmt)
#endif

#endif
