CC = gcc
LD = gcc
CC_FLAGS = -g -ggdb
LD_FLAGS = -levas -lesd

all: slets

clean:
	rm -rf *.o slets

slets: main.o core.o game.o gfx.o theme.o
	${LD} ${LD_FLAGS} -o $@ main.o core.o game.o gfx.o theme.o

main.o: main.c main.h
	${CC} ${CC_FLAGS} -c -o $@ main.c

core.o: core.c core.h main.h
	${CC} ${CC_FLAGS} -c -o $@ core.c

game.o: game.c game.h main.h
	${CC} ${CC_FLAGS} -c -o $@ game.c

gfx.o: gfx.c gfx.h main.h
	${CC} ${CC_FLAGS} -c -o $@ gfx.c

theme.o: theme.c theme.h main.h
	${CC} ${CC_FLAGS} -c -o $@ theme.c
