#!/usr/bin/python

import sys, os, tempfile

try:
    infile = open(sys.argv[1], "rb")
except IndexError, e:
    raise SystemExit("missing input file name")
except OSError, e:
    raise SystemExit("could not open file '%s': %s" % (sys.argv[1], e))

dirname = os.path.dirname(sys.argv[0])
check_path_bin = os.path.join(dirname, "check-patch.py")

r = os.system("'%s' '%s'" % (check_path_bin, sys.argv[1]))
if r != 0:
    print "problems with patch, use check-patch.py first!"
    sys.exit(r)

msg = []
for line in sys.argv[1]:
    if line.startswith("Index: "):
        break
    else:
        msg.append(line)

if not msg:
    raise SystemExit("no commit message in patch!")

f = tempfile.NamedTemporaryFile()
f.writelines(msg)
f.flush()

r = os.system("svn commit -F '%s'" % f.name)
f.close()
if r != 0:
    print "problems running svn commit!"
    sys.exit(r)

