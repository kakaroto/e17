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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <efsd_options.h>
#include <efsd_debug.h>
#include <efsd_macros.h>

EfsdOption *
efsd_option_new_get_stat(void)
{
  EfsdOption *eo;

  D_ENTER;
  eo = NEW(EfsdOption);
  memset(eo, 0, sizeof(EfsdOption));
  eo->type = EFSD_OP_LS_GET_STAT;
  D_RETURN_(eo);
}


EfsdOption *
efsd_option_new_get_metadata(char *key, EfsdDatatype type)
{
  EfsdOption *eo;

  D_ENTER;
  eo = NEW(EfsdOption);
  memset(eo, 0, sizeof(EfsdOption));
  eo->type = EFSD_OP_LS_GET_META;
  eo->efsd_op_ls_getmeta.key = strdup(key);
  eo->efsd_op_ls_getmeta.datatype = type;
  D_RETURN_(eo);
}


EfsdOption *
efsd_option_new_get_filetype(void)
{
  EfsdOption *eo;

  D_ENTER;
  eo = NEW(EfsdOption);
  memset(eo, 0, sizeof(EfsdOption));
  eo->type = EFSD_OP_LS_GET_FILE;
  D_RETURN_(eo);
}


EfsdOption *
efsd_option_new_force(void)
{
  EfsdOption *eo;

  D_ENTER;
  eo = NEW(EfsdOption);
  memset(eo, 0, sizeof(EfsdOption));
  eo->type = EFSD_OP_FS_FORCE;
  D_RETURN_(eo);
}


EfsdOption *
efsd_option_new_recursive(void)
{
  EfsdOption *eo;

  D_ENTER;
  eo = NEW(EfsdOption);
  memset(eo, 0, sizeof(EfsdOption));
  eo->type = EFSD_OP_FS_RECURSIVE;
  D_RETURN_(eo);
}


void           
efsd_option_cleanup(EfsdOption *eo)
{
  D_ENTER;

  switch (eo->type)
    {
    case EFSD_OP_FS_FORCE:
      break;
    case EFSD_OP_FS_RECURSIVE:
      break;
    case EFSD_OP_LS_GET_STAT:
      break;
    case EFSD_OP_LS_GET_FILE:
      break;
    case EFSD_OP_LS_GET_META:
      FREE(eo->efsd_op_ls_getmeta.key);
      break;
    default:
      D(("Unknown option.\n"));
    }
  
  D_RETURN;
}
