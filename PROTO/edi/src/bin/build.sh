#!/bin/sh

gcc '-DPACKAGE_NAME="edi"' '-DPACKAGE_VERSION="0.0.1"' '-DPACKAGE_BIN_DIR="/usr/devel/bin"' '-DPACKAGE_DATA_DIR="/usr/devel/share"'  edi_main.c -o edi `pkg-config --cflags --libs eina evas elementary` -lclang -L/usr/lib/llvm $CFLAGS
