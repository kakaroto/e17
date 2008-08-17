#!/usr/bin/env python

import epsilon
import sys
import os

try:
    path = sys.argv[1]
except IndexError, e:
    raise SystemExit("Usage: %s <path>" % sys.argv[0])

thumb = epsilon.Epsilon(path)
if not thumb.exists():
    print "Generating thumb", thumb
    print thumb.generate()
else:
    print "Already generated."

print "Thumb is", thumb.thumb_file
