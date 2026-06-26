// String native module

#include "native.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static char *dup_empty_string(void) {
    char *result = malloc(1);

    if (!result) {
        return NULL;
    }

    result[0] = '\0';
    return result;
}

// Symbol name expected by interpreter: String_getStringLength
NativeValue String_getStringLength(size_t argc, const char **argv) {
    if (argc == 0) {
        return NATIVE_INT_VALUE(0);
    }

    size_t len = strlen(argv[0]);
    return NATIVE_INT_VALUE((long)len);
}

// Concatenate strings
NativeValue String_doStringConcat(size_t argc, const char **argv) {
    if (argc == 0) {
        return NATIVE_STRING_VALUE(dup_empty_string());
    }

    size_t total_len = 0;

    for (size_t i = 0; i < argc; i++) {
        total_len += strlen(argv[i]);
    }

    char *result = malloc(total_len + 1);

    if (!result) {
        return NATIVE_STRING_VALUE(NULL);
    }

    result[0] = '\0';

    for (size_t i = 0; i < argc; i++) {
        strcat(result, argv[i]);
    }

    return NATIVE_STRING_VALUE(result);
}

// Reverse a string
NativeValue String_doStringReverse(size_t argc, const char **argv) {
    if (argc == 0) {
        return NATIVE_STRING_VALUE(dup_empty_string());
    }

    const char *s = argv[0];
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NATIVE_STRING_VALUE(NULL);
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = s[len - 1 - i];
    }

    result[len] = '\0';

    return NATIVE_STRING_VALUE(result);
}

// Capitalize a string
NativeValue String_doStringCapitalize(size_t argc, const char **argv) {
    if (argc == 0) {
        return NATIVE_STRING_VALUE(dup_empty_string());
    }

    const char *s = argv[0];
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NATIVE_STRING_VALUE(NULL);
    }

    for (size_t i = 0; i < len; i++) {
        if (i == 0) {
            result[i] = (char)toupper((unsigned char)s[i]);
        } else {
            result[i] = (char)tolower((unsigned char)s[i]);
        }
    }

    result[len] = '\0';

    return NATIVE_STRING_VALUE(result);
}

// Make string uppercase
NativeValue String_doStringUppercase(size_t argc, const char **argv) {
    if (argc == 0) {
        return NATIVE_STRING_VALUE(dup_empty_string());
    }

    const char *s = argv[0];
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NATIVE_STRING_VALUE(NULL);
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = (char)toupper((unsigned char)s[i]);
    }

    result[len] = '\0';

    return NATIVE_STRING_VALUE(result);
}

// Make string lowercase
NativeValue String_doStringLowercase(size_t argc, const char **argv) {
    if (argc == 0) {
        return NATIVE_STRING_VALUE(dup_empty_string());
    }

    const char *s = argv[0];
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NATIVE_STRING_VALUE(NULL);
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = (char)tolower((unsigned char)s[i]);
    }

    result[len] = '\0';

    return NATIVE_STRING_VALUE(result);
}

// Check if two strings are equal
NativeValue String_doStringEquals(size_t argc, const char **argv) {
    if (argc < 2) {
        return NATIVE_BOOL_VALUE(0);
    }

    if (strcmp(argv[0], argv[1]) == 0) {
        return NATIVE_BOOL_VALUE(1);
    }

    return NATIVE_BOOL_VALUE(0);
}

// @TODO: Test this function
NativeValue String_doStringContains(size_t argc, const char **argv) {
    if (argc < 2) {
        return NATIVE_BOOL_VALUE(0);
    }

    if (strstr(argv[0], argv[1]) != NULL) {
        return NATIVE_BOOL_VALUE(1);
    }

    return NATIVE_BOOL_VALUE(0);
}