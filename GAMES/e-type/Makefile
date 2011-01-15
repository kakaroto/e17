OBJECTS=alien.o explosion.o game.o main.o powerup.o scenario.o scrolling.o ship.o shoot.o stars.o
BINARY=e-type
DIRNAME=etype
TARNAME=$(BINARY).tar

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin


CC = gcc

CPPFLAGS = `pkg-config --cflags edje ecore-evas`
CFLAGS = -g -Wall -W -Wextra -Wshadow
LDFLAGS = `pkg-config --libs edje ecore-evas`


EDJE_CC = edje_cc
EDJE_FLAGS = -v -id .


all: $(OBJECTS) $(BINARY) theme.edj

%.o : %.c
	@echo "  Compilation of $(@D)/$(<F)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(BINARY): $(OBJECTS)
	@echo "  Linking  $(@F)"
	@$(CC) -o $(BINARY) $(OBJECTS) $(LDFLAGS)

theme.edj: theme.edc
	@echo "  Compilation of $(@D)/$(<F)"
	@$(EDJE_CC) $(EDJE_FLAGS) theme.edc theme.edj

install: $(BINARY) theme.edj
	@echo "installation of $(BINARY)"
	@mkdir -p $(BINDIR)
	@install -m 0755 $(BINARY) $(BINDIR)

clean:
	@rm -f *.o theme.edj *~ $(BINARY)

distclean: clean
	@rm -rf $(DIRNAME) $(TARNAME).bz2


predist:
	@rm -rf $(DIRNAME) $(TARNAME).bz2
	@mkdir $(DIRNAME)
	@cp Makefile TODO README *.c *.h *.png theme.edc $(BINARY) $(DIRNAME)/

dist: predist
	@tar cf $(TARNAME) $(DIRNAME)/
	@bzip2 -9 $(TARNAME)
	@rm -rf $(DIRNAME)/

distcheck: predist
	cd $(DIRNAME)/ && make
	@rm -rf $(DIRNAME)/
