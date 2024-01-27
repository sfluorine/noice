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

void npb_dup(npb_t* pb, int32_t offset)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DUP;

    memcpy(pb->program + pb->program_len, &offset, sizeof(offset));
    pb->program_len += sizeof(offset);
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

void npb_ieq(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_IEQ;
}

void npb_ineq(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_INEQ;
}

void npb_ilt(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_ILT;
}

void npb_igt(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_IGT;
}

void npb_ilte(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_ILTE;
}

void npb_igte(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_IGTE;
}

void npb_dadd(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DADD;
}

void npb_dsub(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DSUB;
}

void npb_dmul(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DMUL;
}

void npb_ddiv(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DDIV;
}

void npb_deq(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DEQ;
}

void npb_dneq(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DNEQ;
}

void npb_dlt(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DLT;
}

void npb_dgt(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DGT;
}

void npb_dlte(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DLTE;
}

void npb_dgte(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_DGTE;
}

void npb_br(npb_t* pb, int32_t addr)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_BR;

    memcpy(pb->program + pb->program_len, &addr, sizeof(addr));
    pb->program_len += sizeof(addr);
}

void npb_brit(npb_t* pb, int32_t addr)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_BRIT;

    memcpy(pb->program + pb->program_len, &addr, sizeof(addr));
    pb->program_len += sizeof(addr);
}

void npb_call(npb_t* pb, int32_t addr, int32_t num_args)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_CALL;

    memcpy(pb->program + pb->program_len, &addr, sizeof(addr));
    pb->program_len += sizeof(addr);

    memcpy(pb->program + pb->program_len, &num_args, sizeof(num_args));
    pb->program_len += sizeof(num_args);
}

void npb_ret(npb_t* pb)
{
    RESIZE_IF_NEEDED();

    pb->program[pb->program_len++] = INS_RET;
}

#define FETCH(__type)                                           \
    ({                                                          \
        __type value = 0;                                       \
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

#define DOUBLE_COMP(__op)                                       \
    {                                                           \
        if (vm->sp < 1)                                         \
            return TRAP_STACK_UNDERFLOW;                        \
        double b = value_as_double(pop(vm));                    \
        double a = value_as_double(pop(vm));                    \
        int32_t result = a __op  b;                             \
        push(vm, value_from_int(result));                       \
        return TRAP_OK;                                         \
    }                                                           \

#define DOUBLE_BINOP(__op)                                      \
    {                                                           \
        if (vm->sp < 1)                                         \
            return TRAP_STACK_UNDERFLOW;                        \
        double b = value_as_double(pop(vm));                    \
        double a = value_as_double(pop(vm));                    \
        double result = a __op  b;                              \
        push(vm, value_from_double(result));                    \
        return TRAP_OK;                                         \
    }                                                           \


void noice_init(noice_t* vm)
{
    vm->program = NULL;
    vm->program_len = 0;

    vm->ip = 0;

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
            case TRAP_UNKNOWN_OPCODE:
                fprintf(stderr, "ERROR: unknown opcode: 0x%X\n", vm->program[vm->ip - 1]);
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
            if (vm->sp < 0)
                return TRAP_STACK_UNDERFLOW;

            pop(vm);
            return TRAP_OK;
        }
        case INS_DUP: {
            if (vm->sp >= STACK_CAP)
                return TRAP_STACK_OVERFLOW;

            int32_t offset = FETCH(int32_t);

            push(vm, vm->stack[vm->sp - offset]);
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
        case INS_IEQ:  INTEGER_BINOP(==);
        case INS_INEQ: INTEGER_BINOP(!=);
        case INS_ILT:  INTEGER_BINOP(<);
        case INS_IGT:  INTEGER_BINOP(>);
        case INS_ILTE: INTEGER_BINOP(<=);
        case INS_IGTE: INTEGER_BINOP(>=);
        case INS_DADD: DOUBLE_BINOP(+);
        case INS_DSUB: DOUBLE_BINOP(-);
        case INS_DMUL: DOUBLE_BINOP(*);
        case INS_DDIV: DOUBLE_BINOP(/);
        case INS_DEQ:  DOUBLE_COMP(==);
        case INS_DNEQ: DOUBLE_COMP(!=);
        case INS_DLT:  DOUBLE_COMP(<);
        case INS_DGT:  DOUBLE_COMP(>);
        case INS_DLTE: DOUBLE_COMP(<=);
        case INS_DGTE: DOUBLE_COMP(>=);
        case INS_BR: {
            int32_t addr = FETCH(int32_t);
            vm->ip = addr;
            return TRAP_OK;
        }
        case INS_BRIT: {
            if (vm->sp < 0)
                return TRAP_STACK_UNDERFLOW;

            int32_t addr = FETCH(int32_t);

            if (value_as_int(pop(vm)))
                vm->ip = addr;

            return TRAP_OK;
        }
        case INS_CALL: {
            if (vm->sp + 3 >= STACK_CAP)
                return TRAP_STACK_OVERFLOW;

            int32_t addr = FETCH(int32_t);
            int32_t num_args = FETCH(int32_t);

            push(vm, value_from_int(num_args));
            push(vm, value_from_int(vm->fp));
            push(vm, value_from_int(vm->ip));

            vm->fp = vm->sp;
            vm->ip = addr;

            return TRAP_OK;
        }
        case INS_RET: {
            if (vm->sp - 3 < 0)
                return TRAP_STACK_UNDERFLOW;

            value_t ret_val = vm->stack[vm->sp];

            vm->sp = vm->fp;

            vm->ip = value_as_int(pop(vm));
            vm->fp = value_as_int(pop(vm));

            int32_t num_args = value_as_int(pop(vm));

            if (vm->sp - num_args < 0)
                return TRAP_STACK_UNDERFLOW;
            
            for (int32_t i = 0; i < num_args; i++)
                pop(vm);

            push(vm, ret_val);

            return TRAP_OK;
        }
        default: {
            return TRAP_UNKNOWN_OPCODE;
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
