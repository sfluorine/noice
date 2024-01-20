#include <stdint.h>
#include <stdio.h>

#include "vm.h"

int main()
{
    npb_t npb;
    npb_init(&npb);

    npb_ipush(&npb, 0);
    npb_ipush(&npb, 1);
    npb_iadd(&npb);
    npb_dup(&npb, 0);
    npb_dup(&npb, 0);
    npb_print(&npb);
    npb_dup(&npb, 0);
    npb_ipush(&npb, 69420000);
    npb_ilt(&npb);
    npb_brit(&npb, 10);

    noice_t vm;
    noice_init(&vm);
    noice_load_program(&vm, npb.program, npb.program_len, 0);

    noice_run(&vm);

    npb_free(&npb);
}
