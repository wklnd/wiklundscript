// String native module
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Symbol name expected by interpreter: String_length
const char *String_getStringLength(size_t argc, const char **argv) {
    if (argc == 0) return "0";
    size_t len = strlen(argv[0]);
    char buf[32];
    snprintf(buf, sizeof(buf), "%zu", len);
    char *copy = malloc(strlen(buf) + 1);
    strcpy(copy, buf);
    return copy;
}


const char *String_doStringConcat(size_t argc, const char **argv) {
    if (argc == 0) return "";
    size_t total_len = 0;
    for (size_t i = 0; i < argc; i++) {
        total_len += strlen(argv[i]);
    }
    char *result = malloc(total_len + 1);
    result[0] = '\0';
    for (size_t i = 0; i < argc; i++) {
        strcat(result, argv[i]);
    }
    return result;
}

// Reverse a string
const char *String_doStringReverse(size_t argc, const char **argv) {
    if (argc == 0) return "";
    const char *s = argv[0];
    size_t len = strlen(s);
    char *result = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        result[i] = s[len - 1 - i];
    }
    result[len] = '\0';
    return result;
}   
