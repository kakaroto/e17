# $Id$

BIN = enthrall
SRC = $(BIN:%=%.c)
OBJ = $(SRC:%.c=%.o)
PREFIX = /usr/local

FLAGS = $(CFLAGS) \
        `ecore-config --cflags` `imlib2-config --cflags`
LIBS = `ecore-config --libs` `imlib2-config --libs`

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $@

.c.o:
	$(CC) $(FLAGS) -c -o $@ $<

install: $(BIN)
	install $(BIN) $(DESTDIR)$(PREFIX)/bin

clean:
	rm -f $(OBJ) $(BIN)
