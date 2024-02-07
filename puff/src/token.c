#include "token.h"

token_t make_token(token_kind_t kind, const char* start, int length)
{
    return (token_t) { .kind = kind, .start = start, .length = length };
}
