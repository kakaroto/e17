BIN = src/enthrall
SRC = src/enthrall.c src/theora.c  src/ffmpeg.c src/rgb2yuv420.c
OBJ = $(SRC:%.c=%.o)
PREFIX = /usr/local

LIBS = `pkg-config --libs ecore-x imlib2 theora libavformat libavcodec`

CFLAGS = -g -Wall
ECORE_CFLAGS = `pkg-config --cflags ecore-x`
IMLIB_CFLAGS = `pkg-config --cflags imlib2`
THEORA_CFLAGS = `pkg-config --cflags theora`
FFMPEG_CFLAGS = `pkg-config --cflags libavformat libavcodec`

all: $(BIN)

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
