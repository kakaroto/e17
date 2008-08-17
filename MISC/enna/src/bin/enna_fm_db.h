#ifndef _ENNA_FM_DB_H
#define _ENNA_FM_DB_H

#include "enna.h"

EAPI Evas_Object   *enna_fm_db_add(Evas * evas);
EAPI void           enna_fm_db_process_event(Evas_Object * obj,
					     enna_event event);
EAPI int            enna_fm_db_exe_file_cb_set(Evas_Object * obj,
					       void (exe_file_cb) (void *data1,
								   void *data2,
								   char
								   *filename),
					       void *data1, void *data2);
EAPI int            enna_fm_db_exit_cb_set(Evas_Object * obj,
					   void (exit_cb) (void *data1,
							   void *data2),
					   void *data1, void *data2);

EAPI void           enna_fm_db_parent_go(Evas_Object * obj);
#endif
