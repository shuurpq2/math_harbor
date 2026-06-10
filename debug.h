#ifndef MH_DEBUG_H
#define MH_DEBUG_H

#include <stdio.h>

#ifdef MH_DEBUG
    #define MH_DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define MH_DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

#endif