#include "vm.h"

int main()
{
    npb_t pb;
    npb_init(&pb);

    npb_loadarg(&pb, 0);
    npb_ret(&pb);

    int32_t start = pb.program_len;

    npb_ipush(&pb, 69);
    npb_call(&pb, 0, 1);
    npb_print(&pb);
    npb_halt(&pb);

    noice_t vm;
    noice_init(&vm);
    noice_load_program(&vm, pb.program, pb.program_len, start);
    noice_run(&vm);

    npb_free(&pb);
}
