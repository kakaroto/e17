#ifndef _TSUITE_H
#define _TSUITE_H

#include <Eina.h>
#include <Eet.h>
#include <tsuite_file_data.h>

#define fail_if(expr) \
   do { \
        if ( expr ) \
          { \
             FILE *fp; \
             char buf[1024]; \
             sprintf(buf, "fail_%s.txt", tsuite_test_name_get()); \
             if ((fp = fopen(buf, "a")) != NULL) \
               { \
                  fprintf(fp, "Failed at %s:%d on <%s>\n", __FILE__, __LINE__, #expr); \
                  fclose(fp); \
               } \
          } \
   } while(0)

/* #define DEBUG_TSUITE 1 */
struct _api_data
{
   unsigned int state; /* What state we are testing */
   Evas_Object *win; /* Parent Window of widgets */
   Evas_Object *bt; /* Button to progress in states */
   void *data;   /* Additional data defined by test */
   Eina_Bool free_data; /* free data on close */
};
typedef struct _api_data api_data;

void
_test_close_win(void *data, Evas_Object *obj, void *event_info);

#define TEST_START(x) \
   void x(void) \
     { \
        Evas_Object *win = elm_win_add(NULL, #x, ELM_WIN_BASIC); \
        api_data *api = calloc(1, sizeof(api_data)); \
        tsuite_init(win, #x, api); \
        elm_win_title_set(win, #x); \
        elm_win_autodel_set(win, 1); \
        evas_object_smart_callback_add(win, "delete,request", _test_close_win, NULL); \
        evas_object_show(win);

#define TEST_END \
     }

#define TEST_SHOT(x) \
   tsuite_shot_do(x)

void tsuite_init(Evas_Object *win, char *name, api_data *d);
void tsuite_cleanup(void);
void tsuite_shot_do(char *name);
char *tsuite_test_name_get();
Eina_Bool  write_events_get(void);
void write_events_set(Eina_Bool val);
void variant_list_append(Variant_st *v);
#endif
