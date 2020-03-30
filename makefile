# Makefile for ZDK game, assignment 1, cab202, semester 2, 2018
# by Matthew Stevenson

CC=gcc
FLAGS=-Werror -Wall -std=gnu99 -lm -lzdk -lncurses -LZDK -IZDK

make: Main.c
	$(CC) Main.c $(FLAGS)