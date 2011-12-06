#ifndef _TSUITE_H
#define _TSUITE_H

#include <Eina.h>
#include <Eet.h>

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
   tsuite_shot_do(win, x)

void tsuite_init(Evas_Object *win, char *name, api_data *d);
void tsuite_cleanup(void);
void tsuite_shot_do(Evas_Object *obj, char *name);
char *tsuite_test_name_get();
Eina_Bool  write_events_get(void);
void write_events_set(Eina_Bool val);
/* Macro declaring a function argument to be unused */
#define __UNUSED__ __attribute__((unused))

enum _Tsuite_Event_Type
{  /*  Add any supported events here */
   TSUITE_EVENT_NOT_SUPPORTED = 0,
   TSUITE_EVENT_MOUSE_IN,
   TSUITE_EVENT_MOUSE_OUT,
   TSUITE_EVENT_MOUSE_DOWN,
   TSUITE_EVENT_MOUSE_UP,
   TSUITE_EVENT_MOUSE_MOVE,
   TSUITE_EVENT_MOUSE_WHEEL,
   TSUITE_EVENT_MULTI_DOWN,
   TSUITE_EVENT_MULTI_UP,
   TSUITE_EVENT_MULTI_MOVE,
   TSUITE_EVENT_KEY_DOWN,
   TSUITE_EVENT_KEY_UP,
   TSUITE_EVENT_TAKE_SHOT
};
typedef enum _Tsuite_Event_Type Tsuite_Event_Type;

struct _eet_event_type_mapping
{
   Tsuite_Event_Type t;
   const char *name;
};
typedef struct _eet_event_type_mapping eet_event_type_mapping;

struct _Variant_Type_st
{
   const char *type;
   Eina_Bool   unknow : 1;
};
typedef struct _Variant_Type_st Variant_Type_st;

struct _Variant_st
{
   Variant_Type_st t;
   void                *data; /* differently than the union type, we
                               * don't need to pre-allocate the memory
                               * for the field*/
};
typedef struct _Variant_st Variant_st;

struct _Timer_Data
{
   Eet_File *fp;
   unsigned int recent_event_time;
   Evas *e;
   Eina_List *current_event;
};
typedef struct _Timer_Data Timer_Data;

void variant_list_append(Variant_st *v);
#endif
