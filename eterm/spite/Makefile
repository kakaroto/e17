
CC = gcc

spite: viewer.c eterm.c eterm.h action.c action.h
	$(CC) -g  -Wall `gtk-config --cflags` viewer.c eterm.c action.c -o spite `gtk-config --libs`

clean: 
	rm -f *.o spite *.*~
