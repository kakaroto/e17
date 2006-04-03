#ifdef E_MOD_DEVIAN_TYPEDEFS

#else

#ifndef E_MOD_SOURCE_H_INCLUDED
#define E_MOD_SOURCE_H_INCLUDED

#define SOURCE_NO 0
#ifdef HAVE_PICTURE
#define SOURCE_PICTURE 1 /* Must be the same than DEVIAN_THEME_TYPE_PICTURE */
#endif
#ifdef HAVE_RSS
#define SOURCE_RSS 2 /* Must be the same than DEVIAN_THEME_TYPE_RSS */
#endif
#ifdef HAVE_FILE
#define SOURCE_FILE 3 /* Must be the same than DEVIAN_THEME_TYPE_FILE */
#endif

#define SOURCE_SIZE_POLICY_DEFAULT 1
#define SOURCE_SIZE_POLICY_AUTO 0
#define SOURCE_SIZE_POLICY_USER 1

int DEVIANF(source_change)(DEVIANN *devian, int source);
void DEVIANF(source_detach)(DEVIANN *devian, int part);

int DEVIANF(source_evas_object_get)(DEVIANN *devian,
				    Evas_Object **obj0,
				    Evas_Object **obj1);


int DEVIANF(source_original_size_get)(DEVIANN *devian,
				      int part, int *w, int *h);
void DEVIANF(source_dialog_infos)(DEVIANN *devian);
char *DEVIANF(source_name_get)(DEVIANN *devian, int part);
char *DEVIANF(source_infos_get)(DEVIANN *devian, int part);

void DEVIANF(source_idle_timer_change)(DEVIANN *devian, int active, int time);
int DEVIANF(source_idle_refresh)(DEVIANN *devian, int option);
int DEVIANF(source_idle_set_bg)(DEVIANN *devian);
int DEVIANF(source_idle_viewer)(DEVIANN *devian);
int DEVIANF(source_idle_gui_update)(DEVIANN *devian);

#endif
#endif
