#
# Projekt: KKO
#

#překladač jazyka C
CC=gcc
#parametry překladače
CFLAGS= -std=c99 -Wall -pedantic -Wextra -Wcast-qual -Wundef -g -lm

.PHONY: valg clean %.o %-valg

#překlad hlavního programu
main: main.o tree.o buffer.o ahed.o
	$(CC) $(CFLAGS) -o ahed $^ -lm
	make clean

#spuštění ve valgrindu
valg:
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --track-origins=yes ./ahed -c -i in1

#překlad souborů
%.o: %.c
	$(CC) $(CFLAGS) -c $^

#vyčištění adresáře
clean:
	rm $(wildcard *.o) $(wildcard *.h.gch)

