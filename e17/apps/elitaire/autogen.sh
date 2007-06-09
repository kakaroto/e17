#!/bin/sh

autoreconf -v --install || exit 1

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
