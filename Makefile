CC = gcc
CFLAGS = -Wall -O3 -std=c99
LDFLAGS = -lm

all: solver install start

solver: solver.c
	$(CC) $(CFLAGS) solver.c $(LDFLAGS) -o solver

install:
	npm install

start:
	npm start

clean:
	rm -f solver
	rm -rf node_modules