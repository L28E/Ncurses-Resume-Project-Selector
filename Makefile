CC ?= gcc

NRPS: NRPS.c NRPS.h
	$(CC) NRPS.c -o NRPS -ljansson -lncurses

debug: NRPS.c NRPS.h
	$(CC) NRPS.c -o NRPS_debug -g -ljansson -lncurses
