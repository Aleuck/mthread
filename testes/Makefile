#
# Makefile para programas testes
# 

CC=gcc
LIB_DIR=../lib

all: teste_mlock

teste_mlock: teste_mlock.c $(LIB_DIR)/libmthread.a
	$(CC) -o teste_mlock teste_mlock.c -L$(LIB_DIR) -lmthread -lm -Wall

clean:
	rm -rf teste_mlock *~