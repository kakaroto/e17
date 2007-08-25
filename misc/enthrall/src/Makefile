BIN = enthrall
SRC = enthrall.c theora.c rgb2yuv420.c
OBJ = $(SRC:%.c=%.o)
PREFIX = /usr/local

LIBS = `pkg-config --libs ecore-x imlib2 theora`

ECORE_CFLAGS = `pkg-config --cflags ecore-x`
IMLIB_CFLAGS = `pkg-config --cflags imlib2`
THEORA_CFLAGS = `pkg-config theora --cflags`

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $@

enthrall.o: enthrall.c
	$(CC) $(CFLAGS) $(ECORE_CFLAGS) $(IMLIB_CFLAGS) $(THEORA_CFLAGS) -c -o $@ $<

theora.o: theora.c
	$(CC) $(CFLAGS) $(THEORA_CFLAGS) -c -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(BIN)
	install $(BIN) $(DESTDIR)$(PREFIX)/bin

clean:
	rm -f $(OBJ) $(BIN)
