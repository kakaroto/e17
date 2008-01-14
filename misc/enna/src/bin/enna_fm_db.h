#ifndef _ENNA_FM_DB_H
#define _ENNA_FM_DB_H

EAPI Evas_Object   *enna_fm_db_add(Evas * evas);
EAPI void           enna_fm_db_process_event(Evas_Object * obj,
					     enna_event event);
EAPI int            enna_fm_db_root_set(Evas_Object * obj, char *root_path);
EAPI int            enna_fm_db_filter_set(Evas_Object * obj, char *filter);
EAPI int            enna_fm_db_exe_file_cb_set(Evas_Object * obj,
					       void (exe_file_cb) (void *data1,
								   void *data2,
								   char
								   *filename),
					       void *data1, void *data2);
EAPI int            enna_fm_db_hilight_file_cb_set(Evas_Object * obj,
						   void (hilight_file_cb) (void
									   *data1,
									   void
									   *data2,
									   char
									   *filename),
						   void *data1, void *data2);
EAPI int            enna_fm_db_exit_cb_set(Evas_Object * obj,
					   void (exit_cb) (void *data1,
							   void *data2),
					   void *data1, void *data2);
EAPI int            enna_fm_db_change_path_cb_set(Evas_Object * obj,
						  void (change_path_cb) (void
									 *data,
									 char
									 *path),
						  void *data);
EAPI void           enna_fm_db_parent_go(Evas_Object * obj);
#endif
