/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifndef efsd_fam_r_h
#define efsd_fam_r_h

#include <fam.h>

/* These are the usual FAM calls, wrapped by a mutex
   lock/unlock pair when we're using threads.
*/

int FAMOpen_r(FAMConnection* fc);

int FAMClose_r(FAMConnection* fc);

int FAMMonitorDirectory_r(FAMConnection *fc, const char *filename,
			  FAMRequest* fr, void* userData);

int FAMMonitorFile_r(FAMConnection *fc, const char *filename, 
		     FAMRequest* fr, void* userData);

int FAMCancelMonitor_r(FAMConnection *fc, const FAMRequest *fr);

int FAMNextEvent_r(FAMConnection *fc, FAMEvent *fe);

int FAMPending_r(FAMConnection* fc);

#endif
