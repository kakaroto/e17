BIN = src/enthrall
SRC = src/enthrall.c src/theora.c src/rgb2yuv420.c
OBJ = $(SRC:%.c=%.o)
PREFIX = /usr/local

LIBS = `pkg-config --libs ecore-x imlib2 theora`

ECORE_CFLAGS = `pkg-config --cflags ecore-x`
IMLIB_CFLAGS = `pkg-config --cflags imlib2`
THEORA_CFLAGS = `pkg-config theora --cflags`

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $@

src/enthrall.o: src/enthrall.c
	$(CC) $(CFLAGS) $(ECORE_CFLAGS) $(IMLIB_CFLAGS) $(THEORA_CFLAGS) -c -o $@ $<

src/theora.o: src/theora.c
	$(CC) $(CFLAGS) $(THEORA_CFLAGS) -c -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(BIN)
	install $(BIN) $(DESTDIR)$(PREFIX)/bin

clean:
	rm -f $(OBJ) $(BIN)
