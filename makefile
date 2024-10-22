CC=gcc
CFLAGS=-Wall -pedantic -std=c11 -g3

Chomp: chomp_solver.o chomp.o gmap.o
	${CC} ${CCFLAGS} -o $@ $^

chomp.o: chomp.h
chomp_solver.o: chomp.h gmap.h
gmap.o: gmap.h

