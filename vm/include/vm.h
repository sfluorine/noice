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
void npb_dup(npb_t* pb, int32_t offset);
void npb_print(npb_t* pb);
void npb_iadd(npb_t* pb);
void npb_isub(npb_t* pb);
void npb_imul(npb_t* pb);
void npb_idiv(npb_t* pb);
void npb_ieq(npb_t* pb);
void npb_ineq(npb_t* pb);
void npb_ilt(npb_t* pb);
void npb_igt(npb_t* pb);
void npb_ilte(npb_t* pb);
void npb_igte(npb_t* pb);
void npb_dadd(npb_t* pb);
void npb_dsub(npb_t* pb);
void npb_dmul(npb_t* pb);
void npb_ddiv(npb_t* pb);
void npb_deq(npb_t* pb);
void npb_dneq(npb_t* pb);
void npb_dlt(npb_t* pb);
void npb_dgt(npb_t* pb);
void npb_dlte(npb_t* pb);
void npb_dgte(npb_t* pb);
void npb_br(npb_t* pb, int32_t addr);
void npb_brit(npb_t* pb, int32_t addr);
void npb_call(npb_t* pb, int32_t addr, int32_t num_args);
void npb_ret(npb_t* pb);
void npb_loadarg(npb_t* pb, int32_t n);

#define STACK_CAP 1024

typedef enum {
    TRAP_OK,
    TRAP_HALT,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_UNKNOWN_OPCODE,
} ntrap_t;

typedef enum {
    INS_HALT,
    INS_IPUSH,
    INS_DPUSH,
    INS_POP,
    INS_DUP,
    INS_PRINT,
    INS_IADD,
    INS_ISUB,
    INS_IMUL,
    INS_IDIV,
    INS_IEQ,
    INS_INEQ,
    INS_ILT,
    INS_IGT,
    INS_ILTE,
    INS_IGTE,
    INS_DADD,
    INS_DSUB,
    INS_DMUL,
    INS_DDIV,
    INS_DEQ,
    INS_DNEQ,
    INS_DLT,
    INS_DGT,
    INS_DLTE,
    INS_DGTE,
    INS_BR,
    INS_BRIT,
    INS_CALL,
    INS_RET,
    INS_LOADARG,
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
