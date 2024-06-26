NAME=gesamt
LEX=flex
OBJECTS = lex.yy.o oxout.tab.o symt.o $(NAME)_iburg.o gentree.o assembly.o reg.o clist.o

all: $(NAME)

$(NAME): $(OBJECTS)
	gcc -g -o $@ $^ -lfl

symt.o: symt.c symt.h
	gcc -g -c -pedantic -Wall -o symt.o symt.c

clist.o: clist.c clist.h
	gcc -g -c -pedantic -Wall -o clist.o clist.c

gentree.o: gentree.c gentree.h
	gcc -g -c -pedantic -Wall -o gentree.o gentree.c

assembly.o: assembly.c assembly.h
	gcc -g -c -pedantic -Wall -o assembly.o assembly.c

reg.o: reg.c reg.h
	gcc -g -c -pedantic -Wall -o reg.o reg.c


$(NAME)_iburg.c: code.bfe
	bfe < code.bfe | iburg > $(NAME)_iburg.c

$(NAME)_iburg.o: $(NAME)_iburg.c gentree.h
	gcc -g -c -DUSE_IBURG -DBURM -o $(NAME)_iburg.o $(NAME)_iburg.c

oxout.l oxout.y: parser.y scanner.l
	ox parser.y scanner.l

oxout.tab.c oxout.tab.h: oxout.y
	bison -dv oxout.y

lex.yy.c: oxout.l
	flex oxout.l

lex.yy.o: lex.yy.c oxout.tab.h
	gcc -c -o lex.yy.o lex.yy.c

oxout.tab.o: oxout.tab.c gentree.h
	gcc -c -o oxout.tab.o oxout.tab.c

clean:
	rm -rf *.tab.* lex.yy.c parser oxout.* *.o $(NAME)_iburg.c $(NAME)

test:
	./test.sh

testp:
	make
	./$(NAME) < tests/test4 > playground/ass.s
	cd playground && make && ./a.out