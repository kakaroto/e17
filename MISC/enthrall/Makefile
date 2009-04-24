BIN = src/enthrall
SRC = src/enthrall.c src/theora.c  src/ffmpeg.c src/rgb2yuv420.c
OBJ = $(SRC:%.c=%.o)
PREFIX = /usr/local

LIBS = `pkg-config --libs ecore-x imlib2 theora libavutil libavformat libavcodec libswscale`

CFLAGS = -g -Wall
ECORE_CFLAGS = `pkg-config --cflags ecore-x`
IMLIB_CFLAGS = `pkg-config --cflags imlib2`
THEORA_CFLAGS = `pkg-config --cflags theora`
FFMPEG_CFLAGS = `pkg-config --cflags libavutil libavformat libavcodec libswscale`

all: init $(BIN)

init:
	@if ! `pkg-config --atleast-version=50.2 libavutil 2> /dev/null`; then \
	  echo "Need at least libavutil version 50.2"; \
	  exit 1; \
	fi
	@if ! `pkg-config --atleast-version=52.23 libavcodec 2> /dev/null`; then \
	  echo "Need at least libavcodec version 52.23"; \
	  exit 1; \
	fi
	@if ! `pkg-config --atleast-version=52.31 libavformat 2> /dev/null`; then \
	  echo "Need at least libavformat version 52.31"; \
	  exit 1; \
	fi
	@if ! `pkg-config --atleast-version=0.7 libswscale 2> /dev/null`; then \
	  echo "Need at least libswscale version 0.7"; \
	  exit 1; \
	fi

$(BIN): $(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $@

src/enthrall.o: src/enthrall.c
	$(CC) $(CFLAGS) $(ECORE_CFLAGS) $(IMLIB_CFLAGS) $(THEORA_CFLAGS) $(FFMPEG_CFLAGS) -c -o $@ $<

src/theora.o: src/theora.c
	$(CC) $(CFLAGS) $(THEORA_CFLAGS) -c -o $@ $<

src/ffmpeg.o: src/ffmpeg.c
	$(CC) $(CFLAGS) $(FFMPEG_CFLAGS) -c -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(BIN)
	install $(BIN) $(DESTDIR)$(PREFIX)/bin

clean:
	rm -f $(OBJ) $(BIN)
