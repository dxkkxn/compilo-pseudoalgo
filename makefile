CC = gcc
CFLAGS = -Wall -g
OBJ = ts.o asa.o parser.o lexer.o

all: arc

arc :  $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lfl 

*.o: *.c

parser.h: parser.y

parser.c: parser.y asa.o
	bison -o $@ -d  $<
#-Wcounterexamples
lexer.c: lexer.lex parser.h
	flex -o  $@ $^

clean:
	rm -rf *~ *.o
tests:
	./arc tests/sommenbpairs.algo -o snbp.ram
	./arc tests/tri_selection.algo -o tri_sel.ram
	./arc tests/max.algo -o max.ram
	./arc tests/est_pair.algo -o pair.ram
	./arc tests/syracuse.algo -o syracuse.ram
	./arc tests/tri_a_bulles.algo -o tri_bulles.ram
