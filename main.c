#include <stdio.h>

#include "args_parser.h"

int main(int argc, char *argv[])
{
    Arguments args;
    init_args(&args);
    if (args.help) {
        print_help(argv[0]);
        exit(0);
    }
    return 0;
}