CFLAGS += $(shell pkg-config --cflags elementary evas ecore)
LIBS += $(shell pkg-config --libs elementary evas ecore)

CFLAGS += -Wall -O2

eblock: eblock.o
	$(CC) -o $@ $< $(LIBS)

clean:
	rm -f eblock eblock.o

