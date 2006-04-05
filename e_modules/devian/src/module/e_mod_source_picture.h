#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Source_Picture Source_Picture;

#else

#ifndef E_MOD_SOURCE_PICTURE_H_INCLUDED
#define E_MOD_SOURCE_PICTURE_H_INCLUDED

/* Default things */
#define SOURCE_PICTURE_TIMER_DEFAULT 20
#define SOURCE_PICTURE_TIMER_MIN 3
#define SOURCE_PICTURE_SET_BG_PURGE_DEFAULT 1

#define SOURCE_PICTURE_HISTO_MAX_SIZE 10

struct _Source_Picture
{
   DEVIANN *devian;

   Picture *picture0;
   Picture *picture1;
   Evas_Object *obj_infos;      // REMOVE IT (and change rss)

   Ecore_Timer *timer;

   /* List of edj set as bg */
   Evas_List *was_set_bg;

   /* Historic of viewed pictures */
   Evas_List *histo;/** <Stack, FILO */
   int histo_pos;/** <0 means first */
};

int DEVIANF(source_picture_add) (DEVIANN *devian);
void DEVIANF(source_picture_del) (Source_Picture *source);

int DEVIANF(source_picture_change) (DEVIANN *devian, int option);
int DEVIANF(source_picture_set_bg) (DEVIANN *devian);
int DEVIANF(source_picture_viewer) (DEVIANN *devian);

int DEVIANF(source_picture_evas_object_get) (DEVIANN *devian, Evas_Object **picture0, Evas_Object **picture1);
int DEVIANF(source_picture_original_size_get) (Source_Picture *source, int part, int *w, int *h);
void DEVIANF(source_picture_dialog_infos) (Source_Picture *source);
char *DEVIANF(source_picture_name_get) (Source_Picture *source, int part);
char *DEVIANF(source_picture_infos_get) (Source_Picture *source, int part);
void DEVIANF(source_picture_timer_change) (DEVIANN *devian, int active, int time);

void DEVIANF(source_picture_histo_picture_attach) (Source_Picture *source, Picture *picture);
void DEVIANF(source_picture_histo_picture_detach) (Source_Picture *source, Picture *picture);
void DEVIANF(source_picture_histo_clear) (Source_Picture *source);

#endif
#endif
