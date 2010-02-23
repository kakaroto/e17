#!/bin/sh

autoreconf -f -i

[ -z "$NOCONFIGURE" ] && ./configure "$@"
