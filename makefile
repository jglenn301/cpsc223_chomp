CC=gcc
CFLAGS=-Wall -pedantic -std=c11 -g3

Chomp: chomp_solver.o chomp.o
	${CC} ${CCFLAGS} -o $@ $^

chomp.o: chomp.h
chomp_solver.o: chomp.h
