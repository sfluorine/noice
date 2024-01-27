#include "vm.h"

int main()
{
    npb_t pb;
    npb_init(&pb);

    npb_dpush(&pb, 0.0);

    int32_t start_loop = pb.program_len;

    npb_dpush(&pb, 0.00001);
    npb_dadd(&pb);
    npb_dup(&pb, 0);
    npb_dup(&pb, 0);
    npb_print(&pb);
    npb_dpush(&pb, 69.420);
    npb_dlt(&pb);
    npb_brit(&pb, start_loop);
    npb_halt(&pb);

    noice_t vm;
    noice_init(&vm);
    noice_load_program(&vm, pb.program, pb.program_len, 0);
    noice_run(&vm);

    npb_free(&pb);
}
