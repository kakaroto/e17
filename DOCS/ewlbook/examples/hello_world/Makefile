PROG=hello_world
CC=gcc
CFLAGS=`ewl-config --cflags`
LIBS=`ewl-config --libs`

all:
	$(CC) -o $(PROG) $(CFLAGS) main.c $(LIBS)	

clean:
	rm -f $(PROG)

