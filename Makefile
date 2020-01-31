# Makefile
# Author: flekystyley
# Date: 31.1.2020
# Brief: Makefile for a `Slow HTTP Attack` Tool
#

CC = gcc
FLAGS = -Wall -Wextra -pedantic -Werror
OBJ = *.o
PROG = slowC

all: ${PROG}

args_parser.o: args_parser.c
	${CC} ${FLAGS} args_parser.c -c

${PROG}: args_parser.o
	${CC} ${FLAGS} args_parser.o -o ${PROG}

clean:
	rm ${OBJ} ${PROG}