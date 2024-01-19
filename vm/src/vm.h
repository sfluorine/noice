#pragma once

#include "value.h"

// noice program builder.
typedef struct {
    uint8_t* program;
    int32_t program_len;
    int32_t program_cap;
} npb_t;

void npb_init(npb_t* pb);
void npb_free(npb_t* pb);

void npb_halt(npb_t* pb);
void npb_ipush(npb_t* pb, int32_t value);
void npb_dpush(npb_t* pb, double value);
void npb_pop(npb_t* pb);
void npb_print(npb_t* pb);
void npb_iadd(npb_t* pb);
void npb_isub(npb_t* pb);
void npb_imul(npb_t* pb);
void npb_idiv(npb_t* pb);

#define STACK_CAP 1024

typedef enum {
    TRAP_UNREACHABLE,
    TRAP_OK,
    TRAP_HALT,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
} ntrap_t;

typedef enum {
    INS_HALT,
    INS_IPUSH,
    INS_DPUSH,
    INS_POP,
    INS_PRINT,
    INS_IADD,
    INS_ISUB,
    INS_IMUL,
    INS_IDIV,
} ninstruction_t;

typedef struct {
    uint8_t* program;
    int32_t program_len;

    int32_t ip; // instruction pointer

    value_t stack[STACK_CAP];

    int32_t sp; // stack pointer
    int32_t fp; // frame pointer
} noice_t;

void noice_init(noice_t* vm);
void noice_load_program(noice_t* vm, uint8_t* program, int32_t program_len, int32_t program_start);

void noice_run(noice_t* vm);
