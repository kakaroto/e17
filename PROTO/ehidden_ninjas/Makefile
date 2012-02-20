CXX := g++
SUBDIRS+=src
BINARY=ehninjas
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

CXXFLAGS += $(shell pkg-config --cflags eina evas ecore elementary evas)
LIBS += $(shell pkg-config --libs eina evas ecore elementary)
CXXFLAGS += -g -Wall -Wundef -O2 -DMPDIR=\"${PREFIX}/lib/eina\"

$(BINARY): memmgr.o ehninjas.o
	$(CXX) $(CXXFLAGS) memmgr.cpp ehninjas.cpp -c
	$(CXX) memmgr.o ehninjas.o -o $(BINARY) $(LIBS)

install: $(BINARY)
	@echo "installation of $(BINARY)"
	@mkdir -p $(BINDIR)
	@install -m 0755 $(BINARY) $(BINDIR)

clean:
	rm -f $(BINARY) memmgr.o ehninjas.o

