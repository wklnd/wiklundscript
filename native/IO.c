// IO native module
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Symbol name expected by interpreter: IO_readline
const char *IO_readline(size_t argc, const char **argv) {
    const char *prompt = (argc > 0 && argv[0]) ? argv[0] : "";
    
    if (prompt && *prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }
    
    char buf[1024];
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        char *empty = malloc(1);
        empty[0] = '\0';
        return empty;
    }
    
    // Remove trailing newline
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
    
    char *result = malloc(strlen(buf) + 1);
    strcpy(result, buf);
    return result;
}
