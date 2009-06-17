BIN = eflame
SRC = eflame.c
OBJ = eflame.o
PREFIX = /usr/local

EFLAME_CFLAGS = `pkg-config --cflags evas ecore ecore-evas`
EFLAME_LIBS = `pkg-config --libs evas ecore ecore-evas`

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(EFLAME_LIBS)

.c.o:
	$(CC) $(CFLAGS) $(EFLAME_CFLAGS) -c -o $@ $<

install: $(BIN)
	install $(BIN) $(DESTDIR)$(PREFIX)/bin

clean:
	rm -f $(OBJ) $(BIN)
