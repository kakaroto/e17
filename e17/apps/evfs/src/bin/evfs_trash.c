        /*
         * 
         * Copyright (C) 2006 by Alex Taylor
         * 
         * Permission is hereby granted, free of charge, to any person obtaining a copy
         * of this software and associated documentation files (the "Software"), to
         * deal in the Software without restriction, including without limitation the
         * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
         * sell copies of the Software, and to permit persons to whom the Software is
         * furnished to do so, subject to the following conditions:
         * 
         * The above copyright notice and this permission notice shall be included in
         * all copies of the Software and its documentation and acknowledgment shall be
         * given in the documentation and software packages that this Software was
         * used.
         * 
         * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
         * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
         * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
         * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
         * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
         * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
         * 
         */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "evfs.h"


static char _evfs_trash_info[PATH_MAX];
static char _evfs_trash_files[PATH_MAX];

static int _evfs_trash_init = 0;

void evfs_trash_initialise()
{
	if (_evfs_trash_init) return;
	_evfs_trash_init++;

	snprintf(_evfs_trash_info, PATH_MAX, "%s/.Trash/info", getenv("HOME"));
	snprintf(_evfs_trash_files, PATH_MAX, "%s/.Trash/files", getenv("HOME"));
}

char* evfs_trash_info_dir_get()
{
	return _evfs_trash_info;
}

char* evfs_trash_files_dir_get()
{
	return _evfs_trash_files;
}
