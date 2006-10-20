BIN = enthrall
SRC = enthrall.c theora.c rgb2yuv420.c
OBJ = $(SRC:%.c=%.o)
PREFIX = /usr/local

LIBS = `ecore-config --libs-x` `imlib2-config --libs` \
       `pkg-config theora --libs`

ECORE_CFLAGS = `ecore-config --cflags`
IMLIB_CFLAGS = `imlib2-config --cflags`
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
