#include "parser.h"
#include "ast.h"
#include "lexer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int initialized = 0;
static token_t current;

void parser_init(const char* source)
{
    lexer_init(source);

    current = get_token();
    initialized = 1;
}

static void advance()
{
    if (current.kind != TOK_EOF)
        current = get_token();
}

static int expect(token_kind_t kind)
{
    return current.kind == kind;
}

static void match(token_kind_t kind)
{
    if (expect(TOK_EOF)) {
        fprintf(stderr, "ERROR: unexpected end of file\n");
        exit(1);
    } else if (!expect(kind)) {
        fprintf(stderr, "ERROR: unexpected token: '%.*s'\n", current.length, current.start);
        exit(1);
    }

    advance();
}

expr_t* parse_expression();

static expr_t* parse_primary()
{
    if (expect(TOK_LPAREN)) {
        advance();

        expr_t* expr = parse_expression();

        match(TOK_RPAREN);

        return expr;
    } else if (expect(TOK_NUMBER)) {
        token_t number = current;
        advance();

        return expr_num_make(strtod(number.start, NULL));
    } else if (expect(TOK_IDENTIFIER)) {
        token_t ident = current;
        advance();

        if (expect(TOK_LPAREN)) {
            advance();

            expr_funcall_t* funcall = (expr_funcall_t*)expr_funcall_make(ident);

            int first = 1;
            while (funcall->args_len < 10 && !expect(TOK_RPAREN)) {
                if (!first) {
                    match(TOK_COMMA);
                }

                expr_funcall_push_arg(funcall, parse_expression());
                first = 0;
            }

            match(TOK_RPAREN);

            return (expr_t*)funcall;
        }

        return expr_ident_make(ident);
    } else if (expect(TOK_MINUS)) {
        advance();

        return expr_unary_make('-', parse_expression());
    }

    fprintf(stderr, "ERROR: expected primary expression\n");
    exit(1);
}

static expr_t* parse_factor()
{
    expr_t* lhs = parse_primary();

    while (expect(TOK_STAR) || expect(TOK_SLASH)) {
        char op = expect(TOK_STAR) ? '*' : '/';
        advance();


        expr_t* rhs = parse_primary();
        lhs = expr_binary_make(lhs, op, rhs);
    }

    return lhs;
}

static expr_t* parse_term()
{
    expr_t* lhs = parse_factor();

    while (expect(TOK_PLUS) || expect(TOK_MINUS)) {
        char op = expect(TOK_PLUS) ? '+' : '-';
        advance();


        expr_t* rhs = parse_factor();
        lhs = expr_binary_make(lhs, op, rhs);
    }

    return lhs;
}

expr_t* parse_expression()
{
    assert(initialized);

    expr_t* lhs = parse_term();

    while (expect(TOK_EQUALEQUAL) || expect(TOK_NOTEQUAL)) {
        char op = expect(TOK_EQUALEQUAL) ? '=' : '!';
        advance();

        expr_t* rhs = parse_term();
        lhs = expr_binary_make(lhs, op, rhs);
    }

    return lhs;
}

static block_t* parse_block()
{
    match(TOK_LCURLYBRACE);

    block_t* block = NULL;
    block_t* ptr = NULL;

    while (!expect(TOK_RCURLYBRACE)) {
        if (!block) {
            block = block_make(parse_statement());
            ptr = block;
        } else {
            ptr->next = block_make(parse_statement());
            ptr = ptr->next;
        }
    }

    match(TOK_RCURLYBRACE);

    return block;
}

stmt_t* parse_statement()
{
    if (expect(TOK_LET)) {
        advance();

        token_t ident = current;
        match(TOK_IDENTIFIER);

        match(TOK_EQUAL);

        expr_t* expr = parse_expression();

        return stmt_vardecl_make(ident, expr);
    } else if (expect(TOK_SET)) {
        advance();

        token_t ident = current;
        match(TOK_IDENTIFIER);

        match(TOK_EQUAL);

        expr_t* expr = parse_expression();

        return stmt_varassign_make(ident, expr);
    } else if (expect(TOK_RETURN)) {
        advance();

        return stmt_return_make(parse_expression());
    } else if (expect(TOK_IF)) {
        advance();

        match(TOK_LPAREN);
        expr_t* condition = parse_expression();
        match(TOK_RPAREN);

        block_t* true = parse_block();

        return stmt_if_make(condition, true, NULL);
    }

    return stmt_expr_make(parse_expression());
}

topdecl_t* parse_topdecl()
{
    if (expect(TOK_FUN)) {
        advance();

        token_t name = current;
        match(TOK_IDENTIFIER);

        match(TOK_LPAREN);

        topdecl_fun_t* fun = (topdecl_fun_t*)topdecl_fun_make(name);

        int first = 1;
        while (fun->args_len < 10 && !expect(TOK_RPAREN)) {
            if (!first)
                match(TOK_COMMA);

            token_t arg = current;
            match(TOK_IDENTIFIER);

            topdecl_fun_push_arg(fun, arg);
            first = 0;
        }

        match(TOK_RPAREN);

        fun->funbody = parse_block();

        return (topdecl_t*)fun;
    }

    fprintf(stderr, "ERROR: expected top level declarations\n");
    exit(1);
}

program_t* parse_program()
{
    program_t* program = NULL;
    program_t* ptr = NULL;

    while (!expect(TOK_EOF)) {
        if (!program) {
            program = program_make(parse_topdecl());
            ptr = program;
        } else {
            ptr->next = program_make(parse_topdecl());
            ptr = ptr->next;
        }
    }

    return program;
}
