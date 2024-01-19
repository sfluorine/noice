#include <stdio.h>

#include "vm.h"

int main()
{
    npb_t npb;
    npb_init(&npb);

    npb_ipush(&npb, 34);
    npb_ipush(&npb, 35);
    npb_iadd(&npb);
    npb_print(&npb);
    npb_halt(&npb);

    noice_t vm;
    noice_init(&vm);
    noice_load_program(&vm, npb.program, npb.program_len, 0);

    noice_run(&vm);

    npb_free(&npb);
}
