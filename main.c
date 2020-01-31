#include <stdio.h>
#include <stdlib.h>

#include "args_parser.h"

#define UNUSED(expr) do { (void)(expr); } while (0)

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    Arguments args;
    init_args(&args);
    if (args.help) {
        print_help(argv[0]);
        exit(0);
    }
    return 0;
}