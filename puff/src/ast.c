#include "ast.h"

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static expr_t expr_header_make(expr_kind_t kind)
{
    return (expr_t) { .kind = kind };
}

expr_t* expr_ident_make(token_t ident)
{
    expr_ident_t* expr = malloc(sizeof(*expr));
    expr->__header = expr_header_make(EXPR_IDENTIFIER);
    expr->ident = ident;

    return (expr_t*)expr;
}

expr_t* expr_num_make(expr_num_kind_t kind, token_t number)
{
    expr_num_t* expr = malloc(sizeof(*expr));
    expr->__header = expr_header_make(EXPR_NUMBER);
    expr->kind = kind;
    expr->number = number;

    return (expr_t*)expr;
}

expr_t* expr_unary_make(char op, expr_t* operand)
{
    expr_unary_t* expr = malloc(sizeof(*expr));
    expr->__header = expr_header_make(EXPR_UNARY);
    expr->op = op;
    expr->operand = operand;

    return (expr_t*)expr;
}

expr_t* expr_binary_make(expr_t* lhs, char op, expr_t* rhs)
{
    expr_binary_t* expr = malloc(sizeof(*expr));
    expr->__header = expr_header_make(EXPR_BINARY);
    expr->lhs = lhs;
    expr->op = op;
    expr->rhs = rhs;

    return (expr_t*)expr;
}

expr_t* expr_funcall_make(token_t name)
{
    expr_funcall_t* expr = malloc(sizeof(*expr));
    expr->__header = expr_header_make(EXPR_FUNCALL);
    expr->name = name;
    expr->args_len = 0;

    return (expr_t*)expr;
}

void expr_funcall_push_arg(expr_funcall_t* funcall, expr_t* arg)
{
    if (funcall->args_len >= 10) {
        fprintf(stderr, "ERROR: function call arguments exceeded limit\n");
        exit(1);
    }

    funcall->args[funcall->args_len++] = arg;
}

void expr_free(expr_t* expr)
{
    if (!expr)
        return;

    switch (expr->kind) {
    case EXPR_IDENTIFIER:
    case EXPR_NUMBER:
        free(expr);
        break;
    case EXPR_UNARY: {
        expr_unary_t* e = (expr_unary_t*)expr;
        expr_free(e->operand);
        free(e);
    } break;
    case EXPR_BINARY: {
        expr_binary_t* e = (expr_binary_t*)expr;
        expr_free(e->lhs);
        expr_free(e->rhs);
        free(e);
    } break;
    case EXPR_FUNCALL: {
        expr_funcall_t* e = (expr_funcall_t*)expr;

        for (int i = 0; i < e->args_len; i++) {
            expr_free(e->args[i]);
        }

        free(e);
    } break;
    }
}

static stmt_t stmt_header_make(stmt_kind_t kind)
{
    return (stmt_t) { .kind = kind };
}

stmt_t* stmt_vardecl_make(token_t ident, token_t type, expr_t* expr)
{
    stmt_vardecl_t* stmt = malloc(sizeof(*stmt));
    stmt->__header = stmt_header_make(STMT_VARDECL);
    stmt->ident = ident;
    stmt->type = type;
    stmt->expr = expr;

    return (stmt_t*)stmt;
}

stmt_t* stmt_expr_make(expr_t* expr)
{
    stmt_expr_t* stmt = malloc(sizeof(*stmt));
    stmt->__header = stmt_header_make(STMT_EXPR);
    stmt->expr = expr;

    return (stmt_t*)stmt;
}

stmt_t* stmt_return_make(expr_t* expr)
{
    stmt_return_t* stmt = malloc(sizeof(*stmt));
    stmt->__header = stmt_header_make(STMT_RETURN);
    stmt->expr = expr;

    return (stmt_t*)stmt;
}

stmt_t* stmt_varassign_make(token_t ident, expr_t* expr)
{
    stmt_varassign_t* stmt = malloc(sizeof(*stmt));
    stmt->__header = stmt_header_make(STMT_VARASSIGN);
    stmt->ident = ident;
    stmt->expr = expr;

    return (stmt_t*)stmt;
}

block_t* block_make(stmt_t* stmt)
{
    block_t* block = malloc(sizeof(*block));
    block->stmt = stmt;
    block->next = NULL;

    return block;
}

void block_free(block_t* block)
{
    if (!block)
        return;

    block_free(block->next);
    stmt_free(block->stmt);
    free(block);
}

stmt_t* stmt_if_make(expr_t* condition, block_t* true, block_t* false)
{
    stmt_if_t* stmt = malloc(sizeof(*stmt));
    stmt->__header = stmt_header_make(STMT_IF);
    stmt->condition = condition;
    stmt->true = true;
    stmt->false = false;

    return (stmt_t*)stmt;
}

void stmt_free(stmt_t* stmt)
{
    switch (stmt->kind) {
        case STMT_VARDECL: {
            stmt_vardecl_t* s = (stmt_vardecl_t*)stmt;
            expr_free(s->expr);
            free(s);
        } break;
        case STMT_EXPR: {
            stmt_expr_t* s = (stmt_expr_t*)stmt;
            expr_free(s->expr);
            free(s);
        } break;
        case STMT_RETURN: {
            stmt_return_t* s = (stmt_return_t*)stmt;
            expr_free(s->expr);
            free(s);
        } break;
        case STMT_VARASSIGN: {
            stmt_varassign_t* s = (stmt_varassign_t*)stmt;
            expr_free(s->expr);
            free(s);
        } break;
        case STMT_IF: {
            stmt_if_t* s = (stmt_if_t*)stmt;
            expr_free(s->condition);
            block_free(s->true);
            block_free(s->false);
            free(s);
        } break;
    }
}

static topdecl_t topdecl_make_header(topdecl_kind_t kind)
{
    return (topdecl_t) { .kind = kind };
}

topdecl_t* topdecl_fun_make(token_t name)
{
    topdecl_fun_t* fun = malloc(sizeof(*fun));
    fun->__header = topdecl_make_header(TOPDECL_FUN);
    fun->name = name;
    fun->args_len = 0;
    fun->funbody = NULL;

    return (topdecl_t*)fun;
}

void topdecl_fun_push_arg(topdecl_fun_t* fun, parameter_t arg)
{
    if (fun->args_len >= 10) {
        fprintf(stderr, "ERROR: function arguments exceeded limit\n");
        exit(1);
    }

    fun->args[fun->args_len++] = arg;
}

void topdecl_free(topdecl_t* topdecl)
{
    switch (topdecl->kind) {
        case TOPDECL_FUN: {
            topdecl_fun_t* f = (topdecl_fun_t*)topdecl;
            block_free(f->funbody);
            free(f);
        } break;
    }
}

program_t* program_make(topdecl_t* topdecl)
{
    program_t* program = malloc(sizeof(*program));
    program->topdecl = topdecl;
    program->next = NULL;

    return program;
}

void program_free(program_t* program)
{
    if (!program)
        return;

    program_free(program->next);
    topdecl_free(program->topdecl);
    free(program);
}
