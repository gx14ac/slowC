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

main.o: main.c
	${CC} ${FLAGS} main.c -c

args_parser.o: args_parser.c
	${CC} ${FLAGS} args_parser.c -c

${PROG}: main.o args_parser.o
	${CC} ${FLAGS} main.o args_parser.o -o ${PROG}

clean:
	rm ${OBJ} ${PROG}