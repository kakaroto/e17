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
#include <Edb.h>

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_macros.h>
#include <efsd_misc.h>

static char *meta_db_get_file(char *filename);
static int   meta_db_set_data(EfsdSetMetadataCmd *esmc, char *dbfile);
static void *meta_db_get_data(EfsdGetMetadataCmd *egmc, char *dbfile, int *data_len);
static char *get_full_key(char *file, char* key);


static char*
meta_db_get_file(char *filename)
{
  char *dbfile = NULL;

  D_ENTER;

  if (!filename || filename[0] == '\0' || filename[0] != '/')
    D_RETURN_(NULL);
  
  D_RETURN_(dbfile);
}


static int   
meta_db_set_data(EfsdSetMetadataCmd *esmc, char *dbfile)
{
  char      *key;
  E_DB_File *db;

  D_ENTER;

  if ( (db = e_db_open(dbfile)) == NULL)
    D_RETURN_(0);

  key = get_full_key(esmc->file, esmc->key);

  switch (esmc->datatype)
    {
    case EFSD_INT:
      e_db_int_set(db, key, *((int*)esmc->data));
      break;
    case EFSD_FLOAT:
      e_db_float_set(db, key, *((float*)esmc->data));
      break;
    case EFSD_STRING:
      e_db_str_set(db, key, esmc->data);
      break;
    case EFSD_RAW:
      e_db_data_set(db, key, esmc->data, esmc->data_len);
      break;
    default:
      D(("Unknown data type!\n"));
      e_db_close(db);
      D_RETURN_(0);
    }

  e_db_close(db);

  D_RETURN_(1);
}


static void *
meta_db_get_data(EfsdGetMetadataCmd *egmc, char *dbfile, int *data_len)
{
  void      *result;
  char      *key;
  int        success = FALSE;
  E_DB_File *db;

  D_ENTER;

  if ( (db = e_db_open_read(dbfile)) == NULL)
    D_RETURN_(0);

  key = get_full_key(egmc->file, egmc->key);

  switch (egmc->datatype)
    {
    case EFSD_INT:
      {
	result = NEW(int);
	*data_len = sizeof(int);

	success = e_db_int_get(db, key, (int*)result);
      }
      break;
    case EFSD_FLOAT:
      {
	result = NEW(float);
	*data_len = sizeof(float);

	success = e_db_float_get(db, key, (float*)result);
      }
      break;
    case EFSD_STRING:
      {
	result = e_db_str_get(db, key);
	*data_len = strlen(result) + 1;

	if (result)
	  success = TRUE;
	else
	  success = FALSE;
      }
      break;
    case EFSD_RAW:
      {
	result = e_db_data_get(db, key, data_len);
      }
      break;
    default:
      D(("Unknown data type!\n"));
      e_db_close(db);
      D_RETURN_(0);
    }

  e_db_close(db);

  if (!success)
    D_RETURN_(NULL);

  D_RETURN_(result);
}


static char *
get_full_key(char *file, char *key)
{
  D_ENTER;
  D_RETURN_(NULL);
}


int 
efsd_meta_set(EfsdCommand *ec)
{
  char *dbfile = NULL;
  EfsdSetMetadataCmd *esmc;

  D_ENTER;

  if ((!ec) || (ec->type != EFSD_CMD_SETMETA))
    D_RETURN_(0);

  esmc = &(ec->efsd_set_metadata_cmd);
  dbfile = meta_db_get_file(esmc->file);

  D_RETURN_(meta_db_set_data(esmc, dbfile));
}


void *
efsd_meta_get(EfsdCommand *ec, int *data_len)
{
  char *dbfile = NULL;
  EfsdGetMetadataCmd *egmc;

  D_ENTER;

  if ((!ec) || (ec->type != EFSD_CMD_GETMETA) || (!data_len))
    D_RETURN_(NULL);

  egmc = &(ec->efsd_get_metadata_cmd);
  dbfile = meta_db_get_file(egmc->file);

  D_RETURN_(meta_db_get_data(egmc, dbfile, data_len));
}

