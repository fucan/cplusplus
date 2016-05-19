# Makefile for main
CC:=g++
OPTIONS:=-O3 -Wall -lrt
SUBOPTS:=-Wall -c -o
OBJECTS:=main.o main.o
SOURCES:=main.cc main.cc
HEADERS:=main.h

main:$(OBJECTS)
	$(CC) $(OPTIONS) $^ -o $@
main.o: main.cc
	$(CC) main.cc $(SUBOPTS) $@

.PHONY:clean
clean:
	rm -f *.o *.txt main
