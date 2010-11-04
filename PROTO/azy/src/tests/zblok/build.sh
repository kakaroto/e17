#!/bin/sh

zxdl-compiler -i zblok.zxdl -o .

gcc -o zm-server server.c ZMCommon.c ZMServer.c ZMServer.stubs.c \
  ZMServer.zxrs.c `pkg-config --cflags --libs libzxr` -lssl -lcrypto

gcc -o zm-client client.c ZMCommon.c ZMServer.c ZMServer.zxrc.c \
  `pkg-config --cflags --libs libzxr` -lssl -lcrypto
