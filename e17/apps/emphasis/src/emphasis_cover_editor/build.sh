#!/bin/sh
gcc -g `enhance-config --cflags --libs` `etk-config --cflags --libs` emphasis_cover_editor.c -o emphasis_cover_editor
