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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <efsd_debug.h>
#include <efsd_misc.h>
#include <efsd_misc.h>
#include <efsd_macros.h>
#include <efsd_options.h>
#include <efsd_types.h>


EfsdCommand *
efsd_cmd_new(void)
{
  EfsdCommand *cmd;

  D_ENTER;

  cmd = NEW(EfsdCommand);
  memset(cmd, 0, sizeof(EfsdCommand));

  D_RETURN_(cmd);
}


void
efsd_cmd_duplicate(EfsdCommand *ec_src, EfsdCommand *ec_dst)
{
  D_ENTER;

  if (!ec_src || !ec_dst)
    {
      D_RETURN;
    }

  /* Shallow copy: */
  *ec_dst = *ec_src;

  /* Deep copy: */
  switch (ec_src->type)
    {
    case EFSD_CMD_REMOVE:
    case EFSD_CMD_MAKEDIR:
    case EFSD_CMD_CHMOD:
    case EFSD_CMD_STARTMON_FILE:
    case EFSD_CMD_STARTMON_DIR:
    case EFSD_CMD_STARTMON_META:
    case EFSD_CMD_STOPMON_FILE:
    case EFSD_CMD_STOPMON_DIR:
    case EFSD_CMD_STOPMON_META:
    case EFSD_CMD_LISTDIR:
    case EFSD_CMD_STAT:
    case EFSD_CMD_LSTAT:
    case EFSD_CMD_GETFILETYPE:
    case EFSD_CMD_READLINK:
    case EFSD_CMD_MOVE:
    case EFSD_CMD_COPY:
    case EFSD_CMD_SYMLINK:
      {
	int i;
	
	ec_dst->efsd_file_cmd.files = malloc(sizeof(char*) * ec_src->efsd_file_cmd.num_files);
	for (i = 0; i < ec_src->efsd_file_cmd.num_files; i++)
	  ec_dst->efsd_file_cmd.files[i] = strdup(ec_src->efsd_file_cmd.files[i]);
      }      
      break;
    case EFSD_CMD_SETMETA:
      ec_dst->efsd_set_metadata_cmd.data = malloc(ec_src->efsd_set_metadata_cmd.data_len);
      memcpy(ec_dst->efsd_set_metadata_cmd.data,
	     ec_src->efsd_set_metadata_cmd.data,
	     ec_src->efsd_set_metadata_cmd.data_len);
      ec_dst->efsd_set_metadata_cmd.key  = strdup(ec_src->efsd_set_metadata_cmd.key);
      ec_dst->efsd_set_metadata_cmd.file = strdup(ec_src->efsd_set_metadata_cmd.file);
      break;
    case EFSD_CMD_GETMETA:
      ec_dst->efsd_get_metadata_cmd.key  = strdup(ec_src->efsd_get_metadata_cmd.key);
      ec_dst->efsd_get_metadata_cmd.file = strdup(ec_src->efsd_get_metadata_cmd.file);
      break;
    case EFSD_CMD_CLOSE:
      break;
    default:
      D(("Warning -- unknown command type in duplicate().\n"));
    }

  D_RETURN;
}


void          
efsd_cmd_free(EfsdCommand *ec)
{
  D_ENTER;

  if (!ec)
    D_RETURN;

  efsd_cmd_cleanup(ec);
  FREE(ec);

  D_RETURN;
}


void
efsd_event_duplicate(EfsdEvent *ee_src, EfsdEvent *ee_dst)
{
  void      *d = NULL;

  D_ENTER;

  if (!ee_src || !ee_dst)
    {
      D_RETURN;
    }

  /* The easy part -- shallow copy. */
  *ee_dst = *ee_src;

  /* Now duplicate memory that is pointed to ... */
  switch (ee_src->type)
    {
    case EFSD_EVENT_FILECHANGE:
      ee_dst->efsd_filechange_event.file =
	strdup(ee_src->efsd_filechange_event.file);
      break;
    case EFSD_EVENT_METADATA_CHANGE:
      ee_dst->efsd_metachange_event.file =
	strdup(ee_src->efsd_metachange_event.file);

      ee_dst->efsd_metachange_event.key =
	strdup(ee_src->efsd_metachange_event.key);

      d = malloc(sizeof(char) * ee_src->efsd_metachange_event.data_len);
      memcpy(d, ee_src->efsd_metachange_event.data, ee_src->efsd_metachange_event.data_len);
      ee_dst->efsd_metachange_event.data = d;      
      break;
    case EFSD_EVENT_REPLY:
      efsd_cmd_duplicate(&(ee_src->efsd_reply_event.command),
			 &(ee_dst->efsd_reply_event.command));

      d = malloc(sizeof(char) * ee_src->efsd_reply_event.data_len);
      memcpy(d, ee_src->efsd_reply_event.data, ee_src->efsd_reply_event.data_len);
      ee_dst->efsd_reply_event.data = d;
      break;
    default:
      D(("Warning -- unknown event type.\n"));
    }
}


void        
efsd_event_free(EfsdEvent *ee)
{
  D_ENTER;

  if (!ee)
    {
      D_RETURN;
    }

  efsd_event_cleanup(ee);
  FREE(ee);

  D_RETURN;
}


void     
efsd_cmd_cleanup(EfsdCommand *ec)
{
  D_ENTER;

  if (!ec)
    D_RETURN;

  switch (ec->type)
    {
    case EFSD_CMD_REMOVE:
    case EFSD_CMD_MAKEDIR:
    case EFSD_CMD_STOPMON_FILE:
    case EFSD_CMD_STOPMON_DIR:
    case EFSD_CMD_STOPMON_META:
    case EFSD_CMD_STARTMON_DIR:
    case EFSD_CMD_STARTMON_FILE:
    case EFSD_CMD_STARTMON_META:
    case EFSD_CMD_STAT:
    case EFSD_CMD_LSTAT:
    case EFSD_CMD_GETFILETYPE:
    case EFSD_CMD_READLINK:
    case EFSD_CMD_LISTDIR:
    case EFSD_CMD_COPY:
    case EFSD_CMD_MOVE:
    case EFSD_CMD_SYMLINK:
      {
	int i;

	for (i = 0; i < ec->efsd_file_cmd.num_files; i++)
	  {
	    FREE(ec->efsd_file_cmd.files[i]);
	  }
	
	FREE(ec->efsd_file_cmd.files);

	if (ec->efsd_file_cmd.num_options > 0)
	  {
	    for (i = 0; i < ec->efsd_file_cmd.num_options; i++)
	      {
		efsd_option_cleanup(&(ec->efsd_file_cmd.options[i]));
	      }

	    FREE(ec->efsd_file_cmd.options);
	  }
      }
      break;
    case EFSD_CMD_CHMOD:
      FREE(ec->efsd_chmod_cmd.file);
      break;
    case EFSD_CMD_SETMETA:
      /* FREE(ec->efsd_set_metadata_cmd.data);*/
      FREE(ec->efsd_set_metadata_cmd.key);
      FREE(ec->efsd_set_metadata_cmd.file);
      break;
    case EFSD_CMD_GETMETA:
      FREE(ec->efsd_get_metadata_cmd.key);
      FREE(ec->efsd_get_metadata_cmd.file);
      break;
    case EFSD_CMD_CLOSE:
      break;
    default:
      D(("Warning -- unknown command type in cleanup().\n"));
    }

  memset(ec, 0, sizeof(EfsdCommand));
  D_RETURN;
}


void     
efsd_event_cleanup(EfsdEvent *ev)
{
  D_ENTER;

  if (!ev)
    D_RETURN;
  
  switch (ev->type)
    {
    case EFSD_EVENT_REPLY:
      FREE(ev->efsd_reply_event.data);      
      /*efsd_cmd_cleanup(&ev->efsd_reply_event.command); FIXME - uncomment*/ 
      break;
    case EFSD_EVENT_METADATA_CHANGE:
      FREE(ev->efsd_metachange_event.data);
      FREE(ev->efsd_metachange_event.key);
      FREE(ev->efsd_metachange_event.file);
      break;
    case EFSD_EVENT_FILECHANGE:
      FREE(ev->efsd_filechange_event.file);
      break;
    default:
      D(("Warning -- unknown event type.\n"));
    }  

  memset(ev, 0, sizeof(EfsdEvent));

  D_RETURN;
}
