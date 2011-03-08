CFLAGS += $(shell pkg-config --cflags elementary evas ecore)
LIBS += $(shell pkg-config --libs elementary evas ecore)
CFLAGS += -Wall -O2

BINARY=eblock

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

$(BINARY): eblock.o
	$(CC) -o $@ $< $(LIBS)

install: $(BINARY)
	@echo "installation of $(BINARY)"
	@mkdir -p $(BINDIR)
	@install -m 0755 $(BINARY) $(BINDIR)

clean:
	rm -f $(BINARY) eblock.o

