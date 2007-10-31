import epsilon.request
import ecore
import sys
import os

try:
    folder = sys.argv[1]
except IndexError, e:
    raise SystemExit("Usage: %s <directory>" % sys.argv[0])

count = [0]

def cb(req):
    print "GET:", req
    count[0] -= 1
    if count[0] < 1:
        ecore.main_loop_quit()
    return True

epsilon.request.event_done_handler_set(cb)

for f in os.listdir(folder):
    p = os.path.join(folder, f)
    if os.path.isfile(p):
        print "REQ:", epsilon.request.Request(p)
        count[0] += 1

ecore.main_loop_begin()
