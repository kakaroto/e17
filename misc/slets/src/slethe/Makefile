CC = gcc
LD = gcc
CC_FLAGS = -g -ggdb
LD_FLAGS = -levas -lesd

all: slethe

clean:
	rm -rf *.o slethe

slethe: main.o core.o view.o gfx.o theme.o
	${LD} ${LD_FLAGS} -o $@ main.o core.o view.o gfx.o theme.o

main.o: main.c main.h
	${CC} ${CC_FLAGS} -c -o $@ main.c

core.o: ../slets/core.c ../slets/core.h main.h
	${CC} ${CC_FLAGS} -c -o $@ ../slets/core.c

view.o: view.c view.h main.h
	${CC} ${CC_FLAGS} -c -o $@ view.c

gfx.o: gfx.c gfx.h main.h
	${CC} ${CC_FLAGS} -c -o $@ gfx.c

theme.o: theme.c theme.h main.h
	${CC} ${CC_FLAGS} -c -o $@ theme.c
