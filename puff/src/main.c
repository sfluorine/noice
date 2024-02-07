#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "ERROR: please provide filepath to command line argument\n");
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "ERROR: failed to open %s\n", argv[1]);
        return 1;
    }

    fseek(input, SEEK_SET, SEEK_END);
    long size = ftell(input);
    rewind(input);

    if (size == 0)
        return 0;

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, input);
    buffer[size] = '\0';

    fclose(input);

    noice_t vm;
    noice_init(&vm);

    npb_t pb;
    codegen_init(&pb);

    parser_init(buffer);
    program_t* program = parse_program();

    int main_ip  = codegen_program(&pb, program);
    program_free(program);
    free(buffer);

    if (main_ip == -1) {
        fprintf(stderr, "ERROR: no main function defined\n");
        return 1;
    }

    noice_load_program(&vm, pb.program, pb.program_len, main_ip);
    noice_run(&vm);

    npb_free(&pb);
}
