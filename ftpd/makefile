# Makefile for ftpd
CC:=gcc
OPTIONS:=-O3 -Wall -g -lpthread
SUBOPTS:=-Wall -c -o
OBJECTS:=ftpd.o ftpd_main.o
SOURCES:=ftpd.c ftpd_main.c
HEADERS:=ftpd.h

ftpd_main:$(OBJECTS)
	$(CC) $(OPTIONS) $^ -o $@
ftpd_main.o: ftpd_main.c ftpd.h
	$(CC) ftpd_main.c $(SUBOPTS) $@
ftpd.o:ftpd.c ftpd.h
	$(CC) ftpd.c $(SUBOPTS) $@

.PHONY:clean
clean:
	rm -f *.o *.txt ftpd_main
