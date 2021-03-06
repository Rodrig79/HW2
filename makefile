#Makefile
OBJS = p1.o
CC = g++
DEBUG = -g 
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

proj1: proj1.o BashFunc.o
	g++ proj1.o BashFunc.o -o proj1

proj1.o: proj1.cpp
	g++ -c proj1.cpp

BashFunc.o : BashFunc.cpp BashFunc.h
	g++ -c BashFunc.cpp
clean:
	\rm *.o *~
