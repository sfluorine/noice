#pragma once

typedef enum {
    TOK_LET,
    TOK_FUN,
    TOK_RETURN,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LCURLYBRACE,
    TOK_RCURLYBRACE,
    TOK_EQUAL,
    TOK_COMMA,
    TOK_SEMICOLON,
    TOK_EOF,
    TOK_ERROR,
} token_kind_t;

typedef struct {
    token_kind_t kind;
    const char* start;
    int length;
} token_t;

token_t make_token(token_kind_t kind, const char* start, int length);
