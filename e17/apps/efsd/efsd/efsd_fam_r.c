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
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <efsd_debug.h>
#include <efsd_fam_r.h>
#include <efsd_lock.h>

#if USE_THREADS
#include <pthread.h>

static pthread_mutex_t fam_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

int
FAMOpen_r(FAMConnection* fc)
{
  int result;

  D_ENTER;
  LOCK(&fam_mutex);

  result = FAMOpen(fc);

  UNLOCK(&fam_mutex);
  D_RETURN_(result);
}


int 
FAMClose_r(FAMConnection* fc)
{
  int result;

  D_ENTER;
  LOCK(&fam_mutex);

  result = FAMClose(fc);

  UNLOCK(&fam_mutex);
  D_RETURN_(result);
}


int 
FAMMonitorDirectory_r(FAMConnection *fc, const char *filename,
		      FAMRequest* fr, void* userData)
{
  int result;

  D_ENTER;
  LOCK(&fam_mutex);
  
  result = FAMMonitorDirectory(fc, filename, fr, userData);

  UNLOCK(&fam_mutex);
  D_RETURN_(result);
}


int
FAMMonitorFile_r(FAMConnection *fc, const char *filename, 
		 FAMRequest* fr, void* userData)
{
  int result;

  D_ENTER;
  LOCK(&fam_mutex);

  result = FAMMonitorFile(fc, filename, fr, userData);

  UNLOCK(&fam_mutex);
  D_RETURN_(result);
}


int 
FAMCancelMonitor_r(FAMConnection *fc, const FAMRequest *fr)
{
  int result;

  D_ENTER;
  LOCK(&fam_mutex);

  result = FAMCancelMonitor(fc, fr);

  UNLOCK(&fam_mutex);
  D_RETURN_(result);
}


int 
FAMNextEvent_r(FAMConnection *fc, FAMEvent *fe)
{
  int result;

  D_ENTER;
  LOCK(&fam_mutex);

  result = FAMNextEvent(fc, fe);

  UNLOCK(&fam_mutex);
  D_RETURN_(result);
}

int 
FAMPending_r(FAMConnection* fc)
{
  int result;

  D_ENTER;
  LOCK(&fam_mutex);

  result = FAMPending(fc);

  UNLOCK(&fam_mutex);
  D_RETURN_(result);
}

