#ifndef _EPEG_PRIVATE_H
#define _EPEG_PRIVATE_H

#define _GNU_SOURCE /* need this for fmemopen & open_memstream */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"

FILE *_epeg_memfile_read_open   (void *data, size_t size);
void  _epeg_memfile_read_close  (FILE *f);
FILE *_epeg_memfile_write_open  (void **data, size_t *size);
void  _epeg_memfile_write_close (FILE *f);
    
#endif
