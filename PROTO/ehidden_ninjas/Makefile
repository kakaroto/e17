CXXFLAGS += $(shell pkg-config --cflags eina evas ecore elementary evas)
LIBS += $(shell pkg-config --libs eina evas ecore elementary)
CXXFLAGS += -Wall -Wundef -O2

SUBDIRS = src

BINARY=ehninjas

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

$(BINARY): ehninjas.o
	$(CC) -o $@ $< $(LIBS) -lstdc++

install: $(BINARY)
	@echo "installation of $(BINARY)"
	@mkdir -p $(BINDIR)
	@install -m 0755 $(BINARY) $(BINDIR)

clean:
	rm -f $(BINARY) ehninjas.o

