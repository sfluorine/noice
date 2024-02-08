#include "lexer.h"

#include <assert.h>
#include <ctype.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* puff_source = NULL;

void lexer_init(const char* source)
{
    puff_source = source;
}

static char current()
{
    return *puff_source;
}

static void advance()
{
    if (!current())
        return;

    puff_source++;
}

static void skip_whitespaces()
{
    while (isspace(current()))
        advance();
}

token_t get_token()
{
    assert(puff_source);

    skip_whitespaces();

    const char* start = puff_source;

    if (!current())
        return make_token(TOK_EOF, start, 1);

    switch (current()) {
        case '+':
            advance();
            return make_token(TOK_PLUS, start, 1);
        case '-':
            advance();
            return make_token(TOK_MINUS, start, 1);
        case '*':
            advance();
            return make_token(TOK_STAR, start, 1);
        case '/':
            advance();
            return make_token(TOK_SLASH, start, 1);
        case '(':
            advance();
            return make_token(TOK_LPAREN, start, 1);
        case ')':
            advance();
            return make_token(TOK_RPAREN, start, 1);
        case '!':
            advance();

            if (current() != '=') {
                advance();
                return make_token(TOK_ERROR, start, 2);
            }

            advance();
            return make_token(TOK_NOTEQUAL, start, 2);
        case '=':
            advance();

            if (current() != '=') {
                return make_token(TOK_EQUAL, start, 1);
            }

            advance();
            return make_token(TOK_EQUALEQUAL, start, 2);
        case ':':
            advance();
            return make_token(TOK_COLON, start, 1);
        case ';':
            advance();
            return make_token(TOK_SEMICOLON, start, 1);
        case ',':
            advance();
            return make_token(TOK_COMMA, start, 1);
        case '{':
            advance();
            return make_token(TOK_LCURLYBRACE, start, 1);
        case '}':
            advance();
            return make_token(TOK_RCURLYBRACE, start, 1);
    }

    if (isdigit(current())) {
        int length = 0;
        int mantissa_length = 0;

        do {
            length++;
            advance();
        } while (current() && isdigit(current()));

        if (current() == '.') {
            length++;
            advance();

            while (current() && isdigit(current())) {
                mantissa_length++;
                advance();
            }

            if (mantissa_length == 0) {
                return make_token(TOK_ERROR, start, length);
            }

            return make_token(TOK_DOUBLELITERAL, start, length + mantissa_length);
        }

        return make_token(TOK_INTLITERAL, start, length);
    }

    if (isalpha(current()) || current() == '_') {
        int length = 0;
        do {
            length++;
            advance();
        } while (current() && (isalnum(current()) || current() == '_'));

        if (strncmp(start, "int", length) == 0)
            return make_token(TOK_INT, start, length);

        if (strncmp(start, "double", length) == 0)
            return make_token(TOK_DOUBLE, start, length);

        if (strncmp(start, "void", length) == 0)
            return make_token(TOK_VOID, start, length);

        if (strncmp(start, "let", length) == 0)
            return make_token(TOK_LET, start, length);

        if (strncmp(start, "set", length) == 0)
            return make_token(TOK_SET, start, length);

        if (strncmp(start, "fun", length) == 0)
            return make_token(TOK_FUN, start, length);

        if (strncmp(start, "return", length) == 0)
            return make_token(TOK_RETURN, start, length);

        if (strncmp(start, "if", length) == 0)
            return make_token(TOK_IF, start, length);

        return make_token(TOK_IDENTIFIER, start, length);
    }

    advance();
    return make_token(TOK_ERROR, start, 1);
}
