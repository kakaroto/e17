CFLAGS += $(shell pkg-config --cflags elementary)
LIBS += $(shell pkg-config --libs elementary)

CFLAGS += -Wall -O2

eblock: eblock.o
	$(CC) -o $@ $< $(LIBS)

clean:
	rm -f eblock eblock.o

