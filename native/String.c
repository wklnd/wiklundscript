// String native module

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

static char *dup_string(const char *s) {
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NULL;
    }

    strcpy(result, s);
    return result;
}

// Symbol name expected by interpreter: String_getStringLength
const char *String_getStringLength(size_t argc, const char **argv) {
    if (argc == 0) {
        return dup_string("0");
    }

    size_t len = strlen(argv[0]);

    char buf[32];
    snprintf(buf, sizeof(buf), "%zu", len);

    return dup_string(buf);
}

// Concatenate strings
const char *String_doStringConcat(size_t argc, const char **argv) {
    if (argc == 0) {
        return dup_empty_string();
    }

    size_t total_len = 0;

    for (size_t i = 0; i < argc; i++) {
        total_len += strlen(argv[i]);
    }

    char *result = malloc(total_len + 1);

    if (!result) {
        return NULL;
    }

    result[0] = '\0';

    for (size_t i = 0; i < argc; i++) {
        strcat(result, argv[i]);
    }

    return result;
}

// Reverse a string
const char *String_doStringReverse(size_t argc, const char **argv) {
    if (argc == 0) {
        return dup_empty_string();
    }

    const char *s = argv[0];
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = s[len - 1 - i];
    }

    result[len] = '\0';

    return result;
}

// Capitalize a string
const char *String_doStringCapitalize(size_t argc, const char **argv) {
    if (argc == 0) {
        return dup_empty_string();
    }

    const char *s = argv[0];
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        if (i == 0) {
            result[i] = (char)toupper((unsigned char)s[i]);
        } else {
            result[i] = (char)tolower((unsigned char)s[i]);
        }
    }

    result[len] = '\0';

    return result;
}

// Make string uppercase
const char *String_doStringUppercase(size_t argc, const char **argv) {
    if (argc == 0) {
        return dup_empty_string();
    }

    const char *s = argv[0];
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = (char)toupper((unsigned char)s[i]);
    }

    result[len] = '\0';

    return result;
}

// Make string lowercase
const char *String_doStringLowercase(size_t argc, const char **argv) {
    if (argc == 0) {
        return dup_empty_string();
    }

    const char *s = argv[0];
    size_t len = strlen(s);

    char *result = malloc(len + 1);

    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = (char)tolower((unsigned char)s[i]);
    }

    result[len] = '\0';

    return result;
}

// Check if two strings are equal
const char *String_doStringEquals(size_t argc, const char **argv) {
    if (argc < 2) {
        return dup_string("false");
    }

    if (strcmp(argv[0], argv[1]) == 0) {
        return dup_string("true");
    }

    return dup_string("false");
}