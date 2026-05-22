#pragma once

#include <stddef.h>

typedef enum {
    NATIVE_INT,
    NATIVE_FLOAT,
    NATIVE_BOOL,
    NATIVE_STRING,
} NativeType;

typedef struct {
    NativeType type;
    union {
        long integer;
        double floating;
        int boolean;
        char *string;
    } as;
} NativeValue;

#define NATIVE_INT_VALUE(v)    ((NativeValue){ .type = NATIVE_INT,    .as.integer = (v) })
#define NATIVE_FLOAT_VALUE(v)  ((NativeValue){ .type = NATIVE_FLOAT,  .as.floating = (v) })
#define NATIVE_BOOL_VALUE(v)   ((NativeValue){ .type = NATIVE_BOOL,   .as.boolean = (v) })
#define NATIVE_STRING_VALUE(v) ((NativeValue){ .type = NATIVE_STRING, .as.string = (v) })