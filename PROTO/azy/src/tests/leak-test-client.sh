#!/bin/sh

export G_SLICE=always-malloc G_DEBUG=gc-friendly
FLAGS=--gen-suppressions=all
FLAGS=

valgrind \
  --leak-check=full \
  --leak-resolution=high \
  --show-reachable=yes \
  --log-file-exactly=leak-client.log \
  --suppressions=leak.supp \
  $FLAGS \
  ./client
