#ifndef EPBB_H
#define EPBB_H

struct _Epbb {
    Ecore_Evas *ee;
    Evas_Object *obj;
    Ecore_Timer *timer;
};

typedef struct _Epbb Epbb;

/* instantiate and free */
Epbb* epbb_new(Ecore_Evas *ee);
void  epbb_free(Epbb *e);

/* callback functions */
void epbb_mute_set(Epbb *e);
void epbb_volume_set(Epbb *e, int val);
void epbb_battery_remaining_set(Epbb *e, int mins);
void epbb_battery_source_set(Epbb *e, int mins);
void epbb_brightness_set(Epbb *e, int val);
void epbb_warning_sleep_set(Epbb *e, int val);

#endif
