# GBS Shell Makefile

CC = g++
CFLAGS  = -Wall -g
OBJ = gbsh.o start.o

all: gbsh

gbsh: $(OBJ)
	$(CC) $(CFLAGS) -o gbsh $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
