// src/main.c
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "0.1.1a"
#define REPL_BUFFER 1024

//  File Reading 

static char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "[wscript] could not open file '%s'\n", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

//  Run Source 

static void run_source(const char *source) {
    TokenList tokens = lexer_tokenize(source);
    Program   program = parser_parse(tokens);
    interpreter_run(&program);
    program_free(&program);
    lexer_free(&tokens);
}

//  REPL 

static void run_repl(void) {
    printf("wscript v%s\n", VERSION);
    printf("Type 'exit' to quit.\n\n");

    char line[REPL_BUFFER];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        // Strip newline
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "exit") == 0) break;
        if (strlen(line) == 0)        continue;

        run_source(line);
    }
}

//  Entry Point 

int main(int argc, char **argv) {
    // No arguments -- start REPL
    if (argc == 1) {
        run_repl();
        return 0;
    }

    // Flags
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        printf("wscript v%s\n", VERSION);
        return 0;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("Usage:\n");
        printf("  wscript              start interactive REPL\n");
        printf("  wscript <file.ws>    run a .ws script file\n");
        printf("  wscript --version    print version\n");
        printf("  wscript --help       show this help\n");
        return 0;
    }
    if (strcmp(argv[1], "--repl") == 0) {
        run_repl();
        return 0;
    }

        if (strcmp(argv[1], "--hello") == 0) {
        printf("Hello, World!\n");
        return 0;
    }

    // Run file
    const char *path = argv[1];
    char *source = read_file(path);
    run_source(source);
    free(source);
    return 0;
}