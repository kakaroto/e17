CC=gcc
LEX_FILE=edje.l
PARSE_FILE=edje.y

all: lexer parser
	$(CC) lex.yy.c edje_cc.tab.c main.c -lfl -o edje_cc

lexer: $(LEX_FILE)
	flex $(LEX_FILE)
parser: $(PARSE_FILE)
	yacc -b edje_cc -d $(PARSE_FILE)
clean:
	rm lex.yy.c
	rm edje_cc.tab.c
	rm edje_cc.tab.h
	rm edje_cc
	rm *.o
