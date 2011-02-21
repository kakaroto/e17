#!/bin/sh

autoreconf -f -i

if [ -z "$NOCONFIGURE" ]; then
	./configure -C "$@"
fi
