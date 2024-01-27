#pragma once

// avaliable type slots: 0xfffa000000000000 - 0xfffe000000000000

#include <stdint.h>

typedef enum {
    VAL_DOUBLE,
    VAL_INT,
    VAL_UNKNOWN,
} value_kind_t;

typedef int64_t value_t;

#define NANISH          0xffff000000000000
#define NAN_PAYLOAD     0x0000ffffffffffff

#define INT_MASK        0xfffa000000000000

#define IS_DOUBLE(v)    ((v & NANISH) != NANISH)
#define IS_INT(v)       ((v & INT_MASK) == INT_MASK)

value_kind_t value_get_type(value_t value);

value_t value_from_double(double value);
value_t value_from_int(int32_t value);

double value_as_double(value_t value);
int32_t value_as_int(value_t value);
