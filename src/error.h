// src/error.h
#pragma once

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#define ERROR(msg, line) \
    do { \
        fprintf(stderr, "[wscript] error on line %d: %s\n", line, msg); \
        exit(1); \
    } while(0)

#define ERROR_F(line, fmt, ...) \
    do { \
        fprintf(stderr, "[wscript] error on line %d: " fmt "\n", line, ##__VA_ARGS__); \
        exit(1); \
    } while(0)
