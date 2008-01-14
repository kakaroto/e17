#ifndef _ENNA_CDDA_H
#define _ENNA_CDDA_H

EAPI Evas_Object   *enna_cdda_add(Evas * evas);
EAPI void           enna_cdda_process_event(Evas_Object * obj,
					    enna_event event);
EAPI int            enna_cdda_exe_track_cb_set(Evas_Object * obj,
					       void (exe_file_cb) (void *data1,
								   void *data2,
								   int
								   selected),
					       void *data1, void *data2);
EAPI int            enna_cdda_exit_cb_set(Evas_Object * obj,
					  void (exit_cb) (void *data1,
							  void *data2),
					  void *data1, void *data2);
EAPI void           enna_cdda_device_set(Evas_Object * obj, char *device);
EAPI int            enna_cdda_nbtracks_get(Evas_Object * obj);
EAPI char          *enna_cdda_device_get(Evas_Object * obj);
#endif
