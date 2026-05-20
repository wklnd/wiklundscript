#pragma once

#include <stddef.h>

typedef enum {
    // Keywords
    TOKEN_VAR, TOKEN_CONST, TOKEN_PRINT,
    TOKEN_IF, TOKEN_ELSE, TOKEN_END,
    TOKEN_REPEAT, TOKEN_TIMES, TOKEN_AS, TOKEN_WHILE,
    TOKEN_FUNC, TOKEN_OBJECT, TOKEN_RETURN,
    TOKEN_TRUE, TOKEN_FALSE,

    // Literals
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // Operators
    TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
    TOKEN_EQ, TOKEN_NEQ, TOKEN_GT, TOKEN_LT, TOKEN_GTE, TOKEN_LTE,

    // Punctuation
    TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_DOT, TOKEN_COMMA,

    // Control
    TOKEN_NEWLINE,
    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType   type;
    char       *value;   // heap allocated, freed by lexer_free
    int         line;
} Token;

typedef struct {
    Token  *tokens;
    size_t  count;
    size_t  capacity;
} TokenList;

TokenList lexer_tokenize(const char *source);
void      lexer_free(TokenList *list);