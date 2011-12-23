#!/bin/bash

CFLAGS="-Wall -Wextra -O0 -g -ggdb3 $(pkg-config --libs --cflags azy ecore eina)"
azy_parser services.azy

gcc $CFLAGS -o server server.c Demo_HowAreYou.azy_server.c Demo_Common_Azy.c Demo_Common.c

gcc $CFLAGS -o client client.c Demo_HowAreYou.azy_client.c Demo_Common.c

