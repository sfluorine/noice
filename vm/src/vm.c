#include "vm.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// offset by 12 to prevent program overflow.
#define RESIZE_IF_NEEDED()                                          \
    {                                                               \
        if (pb->program_len + 12 >= pb->program_cap) {              \
            pb->program_cap *= 2;                                   \
            pb->program = realloc(pb->program, pb->program_cap);    \
        }                                                           \
    }                                                               \

void npb_init(npb_t* pb)
{
    pb->program_cap = 1024;
    pb->program_len = 0;
    pb->program = malloc(pb->program_cap);
}

void npb_free(npb_t* pb)
{
    pb->program_cap = 0;
    pb->program_len = 0;
    free(pb->program);
}

void npb_halt(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_HALT;
}

void npb_ipush(npb_t* pb, int32_t value)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_IPUSH;

    memcpy(pb->program + pb->program_len, &value, sizeof(value));
    pb->program_len += sizeof(value);
}

void npb_dpush(npb_t* pb, double value)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DPUSH;

    memcpy(pb->program + pb->program_len, &value, sizeof(value));
    pb->program_len += sizeof(value);
}

void npb_pop(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_POP;
}

void npb_print(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_PRINT;
}

void npb_iadd(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_IADD;
}

void npb_isub(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_ISUB;
}

void npb_imul(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_IMUL;
}

void npb_idiv(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_IDIV;
}

#define FETCH(__type)                                           \
    ({                                                          \
        __type value;                                           \
        memcpy(&value, vm->program + vm->ip, sizeof(__type));   \
        vm->ip += sizeof(__type);                               \
        value;                                                  \
    })                                                          \

#define INTEGER_BINOP(__op)                                     \
    {                                                           \
        if (vm->sp < 1)                                         \
            return TRAP_STACK_UNDERFLOW;                        \
        int32_t b = value_as_int(pop(vm));                      \
        int32_t a = value_as_int(pop(vm));                      \
        int32_t result = a __op  b;                             \
        push(vm, value_from_int(result));                       \
        return TRAP_OK;                                         \
    }                                                           \

void noice_init(noice_t* vm)
{
    vm->program = NULL;
    vm->program_len = 0;

    vm->sp = -1;
    vm->fp = -1;
}

void noice_load_program(noice_t* vm, uint8_t* program, int32_t program_len, int32_t program_start)
{
    vm->program = program;
    vm->program_len = program_len;

    vm->ip = program_start;
}

static ntrap_t evaluate(noice_t* vm);

static void push(noice_t* vm, value_t value);
static value_t pop(noice_t* vm);

static void print_value(value_t value);

void noice_run(noice_t* vm)
{
    while (true) {
        switch (evaluate(vm)) {
            case TRAP_UNREACHABLE:
                fprintf(stderr, "ERROR: unreachable\n");
                return;
            case TRAP_HALT:
                return;
            case TRAP_OK:
                break;
            case TRAP_STACK_OVERFLOW:
                fprintf(stderr, "ERROR: stack overflow\n");
                return;
            case TRAP_STACK_UNDERFLOW:
                fprintf(stderr, "ERROR: stack underflow\n");
                return;
        }
    }
}

ntrap_t evaluate(noice_t* vm)
{
    const uint8_t instruction = FETCH(uint8_t);

    switch (instruction) {
        case INS_HALT: return TRAP_HALT;
        case INS_IPUSH: {
            if (vm->sp >= STACK_CAP)
                return TRAP_STACK_OVERFLOW;

            value_t value = value_from_int(FETCH(int32_t));
            push(vm, value);
            return TRAP_OK;
        }
        case INS_DPUSH: {
            if (vm->sp >= STACK_CAP)
                return TRAP_STACK_OVERFLOW;

            value_t value = value_from_double(FETCH(double));
            push(vm, value);
            return TRAP_OK;
        }
        case INS_POP: {
            pop(vm);
            return TRAP_OK;
        }
        case INS_PRINT: {
            if (vm->sp < 0)
                return TRAP_STACK_UNDERFLOW;

            print_value(pop(vm));
            return TRAP_OK;
        }
        case INS_IADD: INTEGER_BINOP(+);
        case INS_ISUB: INTEGER_BINOP(-);
        case INS_IMUL: INTEGER_BINOP(*);
        case INS_IDIV: INTEGER_BINOP(/);
        default: {
            return TRAP_UNREACHABLE;
        }
    }
}

static void push(noice_t* vm, value_t value)
{
    vm->stack[++vm->sp] = value;
}

static value_t pop(noice_t* vm)
{
    return vm->stack[vm->sp--];
}

static void print_value(value_t value)
{
    switch (value_get_type(value)) {
        case VAL_DOUBLE:
            printf("%lf\n", value_as_double(value));
            break;
        case VAL_INT:
            printf("%d\n", value_as_int(value));
            break;
        case VAL_UNKNOWN:
            printf("unknown value ;(");
            break;
    }
}
