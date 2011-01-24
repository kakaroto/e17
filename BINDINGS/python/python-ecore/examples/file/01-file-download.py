#!/usr/bin/python

import os

import ecore
import ecore.file


def cb_completion(file, status):
    # This callback will be called at the end of the download, status will be
    # 0 if the download finish well or 1 in case of errors
    if status == 0:
        print("Download ended Successfully %s [%d]" % (file, status))
    else:
        print("Download ended with Errors %s [%d]" % (file, status))
    ecore.main_loop_quit()

def cb_progress(file, dltotal, dlnow, ultotal, ulnow):
    # This will be called several time during the download.
    print("Progress [%.2f%% %d %d] %s" %
          ((float(dlnow) / float(dltotal)) * 100,
          dltotal, dlnow, os.path.basename(file)))
    return 0 # continue the download (or 1 to abort)


if __name__ == "__main__":
    # get a 8.1 Mb file from the ubuntu servers
    # and save the file in the current dir
    url = "http://cdimage.ubuntu.com/releases/10.10/release/ubuntu-10.10-dvd-i386.iso.zsync"
    dst = os.path.join(os.getcwd(), os.path.basename(url))

    print 'URL: ' + url
    print 'DST: ' + dst

    # check if ecore has the correct protocol support available
    if not ecore.file.download_protocol_available("http://"):
        print "Download protocol not available"
        exit(1)

    # start the async download, cb_completion and cb_progress will be called
    # during the operation
    try:
        ecore.file.download(url, dst, cb_completion, cb_progress)
    except SystemError:
        print "Download error, maybe dst exists? or dst path isn't there"
        exit(1)
    else:
        print "Download started Successfully"

    # start the main loop
    ecore.main_loop_begin()
