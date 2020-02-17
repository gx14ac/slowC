# Makefile
# Author: flekystyley
# Date: 31.1.2020
# Brief: Makefile for a `Slow HTTP Attack` Tool
#

CC = gcc
FLAGS = -Iinclude -Wall -Wextra -pedantic -Werror
OBJ = *.o
PROG = slowC

all: ${PROG}

args_parser.o: src/args_parser.c
	${CC} ${FLAGS} -c src/args_parser.c

request.o: src/request.c
	${CC} ${FLAGS} -c src/request.c

main.o: src/main.c
	${CC} ${FLAGS} -c src/main.c

${PROG}: main.o args_parser.o request.o
	${CC} ${FLAGS} main.o args_parser.o request.o -o ${PROG}

clean:
	rm ${OBJ} ${PROG}
