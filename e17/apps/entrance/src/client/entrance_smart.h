#ifndef ENTRANCE_SMART
#define ENTRANCE_SMART

typedef struct _Entrance_Smart Entrance_Smart;

struct _Entrance_Smart
{
   Evas_Object *clip;
   Evas_Object *edje;
   Evas_Object *avatar;
   Evas_Coord x, y, w, h;
};

Evas_Object *entrance_smart_add(Evas * e);
void entrance_smart_edje_set(Evas_Object * o, Evas_Object * edje);
void entrance_smart_avatar_set(Evas_Object * o, Evas_Object * avatar);

#endif
