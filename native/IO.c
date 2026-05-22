// IO native module
#include "native.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Symbol name expected by interpreter: IO_readline
NativeValue IO_readline(size_t argc, const char **argv) {
    const char *prompt = (argc > 0 && argv[0]) ? argv[0] : "";
    
    if (prompt && *prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }
    
    char buf[1024];
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        char *empty = malloc(1);
        empty[0] = '\0';
        return NATIVE_STRING_VALUE(empty);
    }
    
    // Remove trailing newline
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
    
    char *result = malloc(strlen(buf) + 1);
    strcpy(result, buf);
    return NATIVE_STRING_VALUE(result);
}


NativeValue IO_parseInt(size_t argc, const char **argv) {
    if (argc == 0 || !argv[0]) {
        return NATIVE_INT_VALUE(0);
    }
    long value = strtol(argv[0], NULL, 10);
    return NATIVE_INT_VALUE(value);
}