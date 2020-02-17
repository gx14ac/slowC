/***** main.c *****
* Author : flekystyley
* Date   : 31.1.2019
* Brief  : entry point
**************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "args_parser.h"

#define UNUSED(expr) do { (void)(expr); } while (0)

int main(int argc, char const *argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  Arguments args;
  init_args(&args);
  parse(argc, argv, &args);

  return 0;
}
