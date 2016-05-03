#ifndef __DEBUGGING_H__
#define __DEBUGGING_H__

#ifdef DEBUG
#define DEBUG_TEXT(fmt, ...) printf(fmt, __VA_ARGS__)
#define DEBUG_PUT(fmt) printf(fmt)
#else
#define DEBUG_TEXT(fmt, ...)
#define DEBUG_PUT(fmt)
#endif
