#pragma once

#include "token.h"

typedef enum {
    EXPR_IDENTIFIER,
    EXPR_NUMBER,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_FUNCALL,
} expr_kind_t;

typedef struct {
    expr_kind_t kind;
} expr_t;

typedef struct {
    expr_t __header;
    token_t ident;
} expr_ident_t;

expr_t* expr_ident_make(token_t ident);

typedef struct {
    expr_t __header;
    double number;
} expr_num_t;

expr_t* expr_num_make(double number);

typedef struct {
    expr_t __header;
    char op;
    expr_t* operand;
} expr_unary_t;

expr_t* expr_unary_make(char op, expr_t* expr);

typedef struct {
    expr_t __header;
    expr_t* lhs;
    char op;
    expr_t* rhs;
} expr_binary_t;

expr_t* expr_binary_make(expr_t* lhs, char op, expr_t* rhs);

typedef struct {
    expr_t __header;
    token_t name;
    expr_t* args[10];
    int args_len;
} expr_funcall_t;

expr_t* expr_funcall_make(token_t name);
void expr_funcall_push_arg(expr_funcall_t* funcall, expr_t* arg);

void expr_free(expr_t* expr);

typedef enum {
    STMT_VARDECL,
    STMT_EXPR,
    STMT_RETURN,
    STMT_VARASSIGN,
    STMT_IF,
} stmt_kind_t;

typedef struct {
    stmt_kind_t kind;
} stmt_t;

typedef struct {
    stmt_t __header;
    token_t ident;
    expr_t* expr;
} stmt_vardecl_t;

stmt_t* stmt_vardecl_make(token_t ident, expr_t* expr);

typedef struct {
    stmt_t __header;
    expr_t* expr;
} stmt_expr_t;

stmt_t* stmt_expr_make(expr_t* expr);

typedef struct {
    stmt_t __header;
    expr_t* expr;
} stmt_return_t;

stmt_t* stmt_return_make(expr_t* expr);

typedef struct {
    stmt_t __header;
    token_t ident;
    expr_t* expr;
} stmt_varassign_t;

stmt_t* stmt_varassign_make(token_t ident, expr_t* expr);

typedef struct block_t block_t;

struct block_t {
    stmt_t* stmt;
    block_t* next;
};

block_t* block_make(stmt_t* stmt);
void block_free(block_t* block);

typedef struct {
    stmt_t __header;
    expr_t* condition;
    block_t* true;
    block_t* false;
} stmt_if_t;

stmt_t* stmt_if_make(expr_t* condition, block_t* true, block_t* false);

void stmt_free(stmt_t* stmt);

typedef enum {
    TOPDECL_FUN
} topdecl_kind_t;

typedef struct {
    topdecl_kind_t kind;
} topdecl_t;

typedef struct {
    topdecl_t __header;
    token_t name;
    token_t args[10];
    int args_len;
    block_t* funbody;
} topdecl_fun_t;

topdecl_t* topdecl_fun_make(token_t name);
void topdecl_fun_push_arg(topdecl_fun_t* fun, token_t arg);

void topdecl_free(topdecl_t* topdecl);

typedef struct program_t program_t;

struct program_t {
    topdecl_t* topdecl;
    program_t* next;
};

program_t* program_make(topdecl_t* topdecl);

void program_free(program_t* program);
