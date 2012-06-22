#!/usr/bin/env python

import sys
import os
import ethumb.client
import ecore

if len(sys.argv) < 2:
    raise SystemExit("missing file names.")

def pop_existent(client, files):
    while files:
        f = files.pop()
        client.file_set(f)
        print client
        if not client.exists():
            return f
        tf, tk = client.thumb_path
        print "%s already thumbnailed %s, %s" % (f, tf, tk)
    return None

def request_next(client, files):
    if pop_existent(client, files):
        id = client.generate(generated_cb, files)
        print "generating id=%d" % id
    else:
        print "nothing left to generate."
        ecore.main_loop_quit()

def generated_cb(client, id, file, key, tpath, tkey, success, files):
    if success:
        print "id=%d (%s, %s) successfully generated %s, %s" % \
            (id, file, key, tpath, tkey)
    else:
        print "id=%d (%s, %s) failed to generate" % (id, file, key)

    request_next(client, files)


def connected_cb(client, success, files):
    if not success:
        print "could not connect to server."
        ecore.main_loop_quit()
        return
    request_next(client, files)

c = ethumb.client.Client(connected_cb, sys.argv[1:])
ecore.main_loop_begin()
del c
