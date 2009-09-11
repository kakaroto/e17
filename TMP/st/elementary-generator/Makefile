CFLAGS := $(shell pkg-config --cflags elementary) -Wall
LDFLAGS := $(shell pkg-config --libs elementary)

ui: ui.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

run: ui
	./ui
