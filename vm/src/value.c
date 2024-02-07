#include "value.h"
#include <assert.h>

#define IS_DOUBLE(v)    ((v & NANISH) != NANISH)
#define IS_INT(v)       ((v & INT_MASK) == INT_MASK)

value_kind_t value_get_type(value_t value)
{
    if (IS_INT(value)) {
        return VAL_INT;
    } else if (IS_DOUBLE(value)) {
        return VAL_DOUBLE;
    } else {
        return VAL_UNKNOWN;
    }
}

value_t value_from_double(double value)
{
    return *(value_t*)&value;
}

value_t value_from_int(int32_t value)
{
    return value | INT_MASK;
}

double value_as_double(value_t value)
{
    assert(IS_DOUBLE(value));
    return *(double*)&value;
}

int32_t value_as_int(value_t value)
{
    assert(IS_INT(value));
    return (int32_t)(value & NAN_PAYLOAD);
}
