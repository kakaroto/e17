gcc -c esmart_rsvg.c `pkg-config librsvg-2.0 --cflags`  -o esmart_rsvg.o
gcc main.c -o main esmart_rsvg.o -lesmart `pkg-config librsvg-2.0 gtk+-2.0 --cflags --libs`

