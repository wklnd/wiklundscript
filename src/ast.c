// src/ast.c
#include "ast.h"

#include <stdlib.h>
#include <string.h>

// ─── Expression Cleanup ───────────────────────────────────────────────────────

void expr_free(Expr *expr) {
    if (!expr) return;

    switch (expr->kind) {
        case EXPR_LITERAL:
            free(expr->literal.value);
            break;

        case EXPR_VARIABLE:
            free(expr->variable.name);
            break;

        case EXPR_MEMBER:
            free(expr->member.object);
            free(expr->member.field);
            break;

        case EXPR_BINARY:
            expr_free(expr->binary.left);
            expr_free(expr->binary.right);
            free(expr->binary.op);
            break;

        case EXPR_CALL:
            free(expr->call.name);
            for (size_t i = 0; i < expr->call.arg_count; i++) {
                expr_free(expr->call.args[i]);
            }
            free(expr->call.args);
            break;
    }

    free(expr);
}

// ─── Statement Cleanup ───────────────────────────────────────────────────────

void stmt_free(Stmt *stmt) {
    if (!stmt) return;

    switch (stmt->kind) {

        case STMT_VAR_DECL:
            free(stmt->var_decl.name);
            expr_free(stmt->var_decl.value);
            break;

        case STMT_ASSIGN:
            free(stmt->assign.target);
            expr_free(stmt->assign.value);
            break;

        case STMT_MEMBER_ASSIGN:
            free(stmt->member_assign.object);
            free(stmt->member_assign.field);
            expr_free(stmt->member_assign.value);
            break;

        case STMT_PRINT:
            expr_free(stmt->print.value);
            break;

        case STMT_IF:
            expr_free(stmt->if_stmt.condition);
            for (size_t i = 0; i < stmt->if_stmt.body_count; i++) {
                stmt_free(stmt->if_stmt.body[i]);
            }
            free(stmt->if_stmt.body);
            for (size_t i = 0; i < stmt->if_stmt.else_count; i++) {
                stmt_free(stmt->if_stmt.else_body[i]);
            }
            free(stmt->if_stmt.else_body);
            break;

        case STMT_REPEAT_TIMES:
            expr_free(stmt->repeat_times.count);
            free(stmt->repeat_times.iterator);
            for (size_t i = 0; i < stmt->repeat_times.body_count; i++) {
                stmt_free(stmt->repeat_times.body[i]);
            }
            free(stmt->repeat_times.body);
            break;

        case STMT_REPEAT_WHILE:
            expr_free(stmt->repeat_while.condition);
            for (size_t i = 0; i < stmt->repeat_while.body_count; i++) {
                stmt_free(stmt->repeat_while.body[i]);
            }
            free(stmt->repeat_while.body);
            break;

        case STMT_FUNC_DECL:
            free(stmt->func_decl.name);
            for (size_t i = 0; i < stmt->func_decl.param_count; i++) {
                free(stmt->func_decl.params[i]);
            }
            free(stmt->func_decl.params);
            for (size_t i = 0; i < stmt->func_decl.body_count; i++) {
                stmt_free(stmt->func_decl.body[i]);
            }
            free(stmt->func_decl.body);
            break;

        case STMT_FUNC_CALL:
            free(stmt->func_call.name);
            for (size_t i = 0; i < stmt->func_call.arg_count; i++) {
                expr_free(stmt->func_call.args[i]);
            }
            free(stmt->func_call.args);
            break;

        case STMT_OBJECT_DECL:
            free(stmt->object_decl.name);
            for (size_t i = 0; i < stmt->object_decl.field_count; i++) {
                free(stmt->object_decl.fields[i].name);
            }
            free(stmt->object_decl.fields);
            break;

        case STMT_RETURN:
            expr_free(stmt->ret.value);
            break;
    }

    free(stmt);
}

// ─── Program Cleanup ─────────────────────────────────────────────────────────

void program_free(Program *program) {
    if (!program) return;
    for (size_t i = 0; i < program->count; i++) {
        stmt_free(program->stmts[i]);
    }
    free(program->stmts);
    program->stmts = NULL;
    program->count = 0;
}