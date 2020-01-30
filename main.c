#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void print_usage()
{
    printf("Usage: slowC option args\n\n");
    printf(
    "Available options:\n"
    " c - contentLength.\n"
    " d - dialWorkerCount.\n"
    " g - goMaxProcs.\n"
    " r - rampUpInterval.\n"
    " s - sleepInterval.\n"
    " t - testDuration.\n"
    " v - victimURL.\n"
    " h - hostHeader.\n"
    );
    return;
}

int main(int argc, char *argv[])
{   
    if (argc < 2)
    {
        print_usage();
        return 2;
    }

    return 0;
}