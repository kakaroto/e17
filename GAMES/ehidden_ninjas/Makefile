CXXFLAGS += $(shell pkg-config --cflags eina evas ecore elementary evas)
LIBS += $(shell pkg-config --libs eina evas ecore elementary)
CXXFLAGS += -Wall -O2

SUBDIRS = src

BINARY=ehidden_ninjas

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

$(BINARY): app.o
	$(CC) -o $@ $< $(LIBS)

install: $(BINARY)
	@echo "installation of $(BINARY)"
	@mkdir -p $(BINDIR)
	@install -m 0755 $(BINARY) $(BINDIR)

clean:
	rm -f $(BINARY) app.o

