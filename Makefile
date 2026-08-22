# Makefile da atividade de pthreads
# Uso: make          (compila tudo)
#      make clean    (remove os executaveis)

CC      = gcc
CFLAGS  = -Wall -O2
LDFLAGS = -pthread

ALL = ex0_hello ex1_paralelo ex2_corrida

all: $(ALL)

ex0_hello: ex0_hello.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

ex1_paralelo: ex1_paralelo.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lm

ex2_corrida: ex2_corrida.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(ALL)

.PHONY: all clean
