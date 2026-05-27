#pragma once

#include <stddef.h>

//  Types 

typedef enum {
    TYPE_STRING,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_UNKNOWN,
} TypeName;

//  Expressions 

typedef enum {
    EXPR_LITERAL,
    EXPR_VARIABLE,
    EXPR_MEMBER,
    EXPR_INDEX,
    EXPR_ARRAY,
    EXPR_BINARY,
    EXPR_CALL,
} ExprKind;

typedef struct Expr Expr;

typedef struct {
    char    *value;     // raw string representation
    TypeName type;
} LiteralExpr;

typedef struct {
    char *name;
} VariableExpr;

typedef struct {
    char *object;
    char *field;
} MemberExpr;

typedef struct {
    Expr *array;
    Expr *index;
} IndexExpr;

typedef struct {
    Expr **items;
    size_t count;
} ArrayExpr;

typedef struct {
    Expr *left;
    char *op;
    Expr *right;
} BinaryExpr;

typedef struct {
    char    *name;
    Expr   **args;
    size_t   arg_count;
} CallExpr;

struct Expr {
    ExprKind kind;
    union {
        LiteralExpr  literal;
        VariableExpr variable;
        MemberExpr   member;
        IndexExpr    index;
        ArrayExpr    array;
        BinaryExpr   binary;
        CallExpr     call;
    };
};

//  Statements 

typedef enum {
    STMT_VAR_DECL,
    STMT_ASSIGN,
    STMT_MEMBER_ASSIGN,
    STMT_PRINT,
    STMT_IF,
    STMT_REPEAT_TIMES,
    STMT_REPEAT_WHILE,
    STMT_FUNC_DECL,
    STMT_FUNC_CALL,
    STMT_OBJECT_DECL,
    STMT_IMPORT,
    STMT_RETURN,
} StmtKind;

typedef struct Stmt Stmt;

// Object field definition
typedef struct {
    char    *name;
    TypeName type;
} FieldDef;

struct Stmt {
    StmtKind kind;
    int      line;

    union {
        // var / const declaration
        struct {
            char *name;
            Expr *value;
            int   is_mutable;  // 1 = var, 0 = const
        } var_decl;

        // name = expr
        struct {
            char *target;
            Expr *value;
        } assign;

        // name.field = expr
        struct {
            char *object;
            char *field;
            Expr *value;
        } member_assign;

        // print(expr)
        struct {
            Expr *value;
        } print;

        // if / else / end
        struct {
            Expr   *condition;
            Stmt  **body;
            size_t  body_count;
            Stmt  **else_body;
            size_t  else_count;
        } if_stmt;

        // repeat N times as i
        struct {
            Expr   *count;
            char   *iterator;
            Stmt  **body;
            size_t  body_count;
        } repeat_times;

        // repeat while condition
        struct {
            Expr   *condition;
            Stmt  **body;
            size_t  body_count;
        } repeat_while;

        // func name params / end
        struct {
            char    *name;
            char   **params;
            size_t   param_count;
            Stmt   **body;
            size_t   body_count;
        } func_decl;

        // name(args)
        struct {
            char    *name;
            Expr   **args;
            size_t   arg_count;
        } func_call;

        // object Name { fields }
        struct {
            char      *name;
            FieldDef  *fields;
            size_t     field_count;
        } object_decl;

        // import Module.field or Module.{a,b} or Module.*
        struct {
            char *module;
            char **fields;     // NULL for wildcard
            size_t field_count;
            int is_wildcard;   // 1 if Module.*
        } import_stmt;

        // return expr
        struct {
            Expr *value;    // NULL = bare return
        } ret;
    };
};

//  Program 

typedef struct {
    Stmt  **stmts;
    size_t  count;
} Program;

//  Memory 

void expr_free(Expr *expr);
void stmt_free(Stmt *stmt);
void program_free(Program *program);