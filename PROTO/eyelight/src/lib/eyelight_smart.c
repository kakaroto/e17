#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eyelight.h"
#include "Eyelight_Smart.h"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
    Eyelight_Viewer *pres;
    //this obj is a clipper and block all event
    Evas_Object *obj;
    //It is place in the background and block all events
    Evas_Object *back;
    //This object is place in the foreground and catch events
    Evas_Object *catch_event;
};
#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret) \
   { \
      char *_e_smart_str; \
      \
      if (!o) return ret; \
      smart = evas_object_smart_data_get(o); \
      if (!smart) return ret; \
      _e_smart_str = (char *)evas_object_type_get(o); \
      if (!_e_smart_str) return ret; \
      if (strcmp(_e_smart_str, type)) return ret; \
   }

#define E_SMART_OBJ_GET(smart, o, type) \
     { \
	char *_e_smart_str; \
	\
	if (!o) return; \
	smart = evas_object_smart_data_get(o); \
	if (!smart) return; \
	_e_smart_str = (char *)evas_object_type_get(o); \
	if (!_e_smart_str) return; \
	if (strcmp(_e_smart_str, type)) return; \
     }

#define E_OBJ_NAME "eyelight_object"
static Evas_Smart  *smart = NULL;

static void _smart_init(void);
static void _smart_add(Evas_Object * obj);
static void _smart_del(Evas_Object * obj);
static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object * obj);
static void _smart_hide(Evas_Object * obj);
static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object * obj);
static Eina_Bool _eyelight_object_init(Evas_Object *obj);
static void _smart_member_add(Evas_Object *obj, Evas_Object *member);

static void key_event_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

Evas_Object *eyelight_object_add(Evas *evas)
{
    _smart_init();
    return evas_object_smart_add(evas, smart);
}

Eina_Bool _eyelight_object_init(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, EINA_FALSE);

    sd->pres = eyelight_viewer_new(evas_object_evas_get(obj), NULL, NULL, 0);

    eyelight_viewer_clip_set(sd->pres,sd->obj);

    eyelight_viewer_smart_obj_set(sd->pres, obj);
    return EINA_TRUE;
}

void eyelight_object_thumbnails_size_set(Evas_Object *obj, int w, int h)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return ;

    eyelight_viewer_thumbnails_size_set(sd->pres, w, h);
}

Eyelight_Viewer_State eyelight_object_state_get(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, EYELIGHT_VIEWER_STATE_DEFAULT);
    if(!sd->pres) return 0;

    return eyelight_viewer_state_get(sd->pres);
}

void eyelight_object_presentation_file_set(Evas_Object *obj, const char* presentation)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    eyelight_viewer_presentation_file_set(sd->pres,presentation);
}

void eyelight_object_theme_file_set(Evas_Object *obj, const char* theme)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    eyelight_viewer_theme_file_set(sd->pres,theme);
}

EAPI const char* eyelight_object_presentation_file_get(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);
    if (!sd->pres) return NULL;

    return eyelight_viewer_presentation_file_get(sd->pres);
}

EAPI const char* eyelight_object_theme_file_get(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);
    if (!sd->pres) return NULL;

    return eyelight_viewer_theme_file_get(sd->pres);
}

EAPI void eyelight_object_eye_file_set(Evas_Object *obj, const char *eye)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres || !eye) return;

    eyelight_object_theme_file_set(obj, eye);
    eyelight_viewer_eye_file_set(sd->pres, eye);
}

EAPI void eyelight_object_dump_file_set(Evas_Object *obj, const char *dump)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    eyelight_viewer_dump_file_set(sd->pres, dump);
}

void eyelight_object_border_set(Evas_Object *obj, int border)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    eyelight_viewer_border_set(sd->pres,border);
}

Eyelight_Viewer *eyelight_object_pres_get(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);
    return sd->pres;
}


void eyelight_object_clear_cache_set(Evas_Object *obj, int clear)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    eyelight_viewer_clear_cache_set(sd->pres, clear);
}

/**
 * The eyelight smart object can handle some events (right, left, expose ...) by himself
 * This function allows to activate the built in events
 */
void eyelight_object_event_set(Evas_Object *obj, int event)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    if(event)
        evas_object_show(sd->catch_event);
    else
    {
        evas_object_hide(sd->catch_event);
        evas_object_focus_set(sd->catch_event,0);
    }
}

void eyelight_object_focus_set(Evas_Object *obj, int focus)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    evas_object_focus_set(sd->catch_event, focus);
}

int eyelight_object_size_get(Evas_Object* obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, 0);
    if (!sd->pres) return 0;

    return eyelight_viewer_size_get(sd->pres);
}

void eyelight_object_slide_next(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    eyelight_viewer_slide_next(sd->pres);
}

void eyelight_object_slide_previous(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    eyelight_viewer_slide_previous(sd->pres);
}

void eyelight_object_slide_goto(Evas_Object *obj, int id)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if (!sd->pres) return;

    eyelight_viewer_slide_goto(sd->pres, id);
}

int eyelight_object_current_id_get(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, 0);
    if(!sd->pres) return 0;

    return eyelight_viewer_current_id_get(sd->pres);
}

void eyelight_object_expose_start(Evas_Object *obj, int select)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_expose_start(sd->pres, select);
}

void eyelight_object_expose_stop(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return ;

    eyelight_viewer_expose_stop(sd->pres);
}

void eyelight_object_expose_next(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return ;

    eyelight_viewer_expose_next(sd->pres);
}

void eyelight_object_expose_previous(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_expose_previous(sd->pres);
}

void eyelight_object_expose_window_next(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_expose_window_next(sd->pres);
}

void eyelight_object_expose_window_previous(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_expose_window_previous(sd->pres);
}

void eyelight_object_expose_down(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_expose_down(sd->pres);
}

void eyelight_object_expose_up(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return ;

    eyelight_viewer_expose_up(sd->pres);
}

void eyelight_object_expose_select(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_expose_select(sd->pres);
}

 void eyelight_object_slideshow_start(Evas_Object *obj,int select)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_slideshow_start(sd->pres, select);
}

 void eyelight_object_slideshow_stop(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_slideshow_stop(sd->pres);
}

 void eyelight_object_slideshow_next(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_slideshow_next(sd->pres);
}

 void eyelight_object_slideshow_previous(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_slideshow_previous(sd->pres);
}

 void eyelight_object_slideshow_select(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_slideshow_select(sd->pres);
}



void eyelight_object_tableofcontents_start(Evas_Object *obj, int select)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_tableofcontents_start(sd->pres, select);
}

void eyelight_object_tableofcontents_stop(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_tableofcontents_stop(sd->pres);
}

void eyelight_object_tableofcontents_next(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_tableofcontents_next(sd->pres);
}

void eyelight_object_tableofcontents_previous(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_tableofcontents_previous(sd->pres);
}

void eyelight_object_tableofcontents_select(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_tableofcontents_select(sd->pres);
}


void eyelight_object_gotoslide_start(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_gotoslide_start(sd->pres);
}

void eyelight_object_gotoslide_stop(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_gotoslide_stop(sd->pres);
}

void eyelight_object_gotoslide_digit_add(Evas_Object *obj, int digit)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_gotoslide_digit_add(sd->pres,digit);
}


void eyelight_object_gotoslide_goto(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_gotoslide_goto(sd->pres);
}

void eyelight_object_gotoslide_digit_last_remove(Evas_Object *obj)
{
    Smart_Data *sd;

    E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);
    if(!sd->pres) return;

    eyelight_viewer_gotoslide_digit_last_remove(sd->pres);
}



void eyelight_object_event_send(Evas_Object *obj, void *event_info)
{
    key_event_cb(obj, evas_object_evas_get(obj), obj, event_info);
}


/*****************/
/* Event manager */
/*****************/


typedef enum _Eyelight_Event_Action
{
    EYELIGHT_NONE,
    EYELIGHT_RIGHT,
    EYELIGHT_LEFT,
    EYELIGHT_QUIT,
    EYELIGHT_WINDOW_PREVIOUS,
    EYELIGHT_WINDOW_NEXT,
    EYELIGHT_UP,
    EYELIGHT_DOWN,
    EYELIGHT_SELECT,
    EYELIGHT_REMOVE,
    EYELIGHT_SLIDE,
    EYELIGHT_TABLEOFCONTENTS
} Eyelight_Event_Action;


typedef struct _Eyelight_Key Eyelight_Key;
struct _Eyelight_Key
{
    const char *keyname;
    Eyelight_Event_Action action;
};

static const Eyelight_Key keys[] = {
    { "Right", EYELIGHT_RIGHT },
    { "FP/Right", EYELIGHT_RIGHT },
    { "RC/Right", EYELIGHT_RIGHT },
    { "RCL/Right", EYELIGHT_RIGHT },
    { "GP/Right", EYELIGHT_RIGHT },
    { "space", EYELIGHT_RIGHT },
    { "Left", EYELIGHT_LEFT },
    { "FP/Left", EYELIGHT_LEFT },
    { "RC/Left", EYELIGHT_LEFT },
    { "RCL/Left", EYELIGHT_LEFT },
    { "GP/Left", EYELIGHT_LEFT },
    { "Escape", EYELIGHT_QUIT },
    { "Home", EYELIGHT_QUIT },
    { "equal", EYELIGHT_QUIT },
    { "Start", EYELIGHT_QUIT },
    { "Stop", EYELIGHT_QUIT },
    { "Prior", EYELIGHT_WINDOW_PREVIOUS },
    { "Next", EYELIGHT_WINDOW_NEXT },
    { "Up", EYELIGHT_UP },
    { "Down", EYELIGHT_DOWN },
    { "Return", EYELIGHT_SELECT },
    { "BackSpace", EYELIGHT_REMOVE },
    { "s", EYELIGHT_SLIDE },
    { "t", EYELIGHT_TABLEOFCONTENTS}
};


static void key_event_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
    Evas_Event_Key_Down* event = (Evas_Event_Key_Down*) event_info;
    Eyelight_Event_Action action = EYELIGHT_NONE;
    unsigned int i;

    Evas_Object *eyelight_smart = (Evas_Object*) data;


    for (i = 0; i < sizeof (keys) / sizeof (Eyelight_Key); ++i)
        if (strcmp(event->keyname, keys[i].keyname) == 0)
        {
            action = keys[i].action;
            break;
        }

    switch (eyelight_object_state_get(eyelight_smart))
    {
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            switch (action)
            {
                case EYELIGHT_RIGHT:
                    eyelight_object_expose_next(eyelight_smart);
                    break;
                case EYELIGHT_LEFT:
                    eyelight_object_expose_previous(eyelight_smart);
                    break;
                case EYELIGHT_WINDOW_NEXT:
                    eyelight_object_expose_window_next(eyelight_smart);
                    break;
                case EYELIGHT_WINDOW_PREVIOUS:
                    eyelight_object_expose_window_previous(eyelight_smart);
                    break;
                case EYELIGHT_UP:
                    eyelight_object_expose_up(eyelight_smart);
                    break;
                case EYELIGHT_DOWN:
                    eyelight_object_expose_down(eyelight_smart);
                    break;
                case EYELIGHT_QUIT:
                    eyelight_object_expose_stop(eyelight_smart);
                    break;
                case EYELIGHT_SELECT:
                    eyelight_object_expose_select(eyelight_smart);
                    break;
                default: ;
            }
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            switch (action)
            {
                case EYELIGHT_QUIT:
                    eyelight_object_gotoslide_stop(eyelight_smart);
                    break;
                case EYELIGHT_SELECT:
                    eyelight_object_gotoslide_goto(eyelight_smart);
                    break;
                case EYELIGHT_REMOVE:
                    eyelight_object_gotoslide_digit_last_remove(eyelight_smart);
                    break;
                default:
                    if (strlen(event->key) == 1 && strchr("0123456789", *event->key))
                        eyelight_object_gotoslide_digit_add(eyelight_smart,atoi(event->key));
                    break;
            }
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            switch (action)
            {
                case EYELIGHT_RIGHT:
                case EYELIGHT_DOWN:
                    eyelight_object_slideshow_next(eyelight_smart);
                    break;
                case EYELIGHT_LEFT:
                case EYELIGHT_UP:
                    eyelight_object_slideshow_previous(eyelight_smart);
                    break;
                case EYELIGHT_QUIT:
                    eyelight_object_slideshow_stop(eyelight_smart);
                    break;
                case EYELIGHT_SELECT:
                    eyelight_object_slideshow_select(eyelight_smart);
                    break;
                default: ;
            }
            break;
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            switch (action)
            {
                case EYELIGHT_RIGHT:
                case EYELIGHT_DOWN:
                    eyelight_object_tableofcontents_next(eyelight_smart);
                    break;
                case EYELIGHT_LEFT:
                case EYELIGHT_UP:
                    eyelight_object_tableofcontents_previous(eyelight_smart);
                    break;
                case EYELIGHT_QUIT:
                    eyelight_object_tableofcontents_stop(eyelight_smart);
                    break;
                case EYELIGHT_SELECT:
                    eyelight_object_tableofcontents_select(eyelight_smart);
                    break;
                default: ;
            }
            break;
        default:
            switch (action)
            {
                case EYELIGHT_RIGHT:
                    eyelight_object_slide_next(eyelight_smart);
                    break;
                case EYELIGHT_LEFT:
                    eyelight_object_slide_previous(eyelight_smart);
                    break;
                case EYELIGHT_UP:
                    eyelight_object_expose_start(eyelight_smart,
                            eyelight_object_current_id_get(eyelight_smart));
                    break;
                case EYELIGHT_SLIDE:
                    eyelight_object_slideshow_start(eyelight_smart,
                            eyelight_object_current_id_get(eyelight_smart));
                    break;
                case EYELIGHT_TABLEOFCONTENTS:
                    eyelight_object_tableofcontents_start(eyelight_smart,eyelight_object_current_id_get(eyelight_smart));
                    break;
                default:
                    if (strlen(event->key) == 1 && strchr("0123456789", *event->key))
                    {
                        eyelight_object_gotoslide_start(eyelight_smart);
                        eyelight_object_gotoslide_digit_add(eyelight_smart,atoi(event->key));
                    }
            }
            break;
    }
    //printf("key: %s\n",event->key);
    //printf("key: %s\n",event->keyname);
}




static void mouse_event_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
    Evas_Object *eyelight_smart = (Evas_Object*) data;
    eyelight_object_focus_set(eyelight_smart, 1);
}

/*******************************************/
/* Internal smart object required routines */
/*******************************************/
    static void
_smart_init(void)
{
    if (smart) return;
    {
        static const Evas_Smart_Class sc =
        {
            E_OBJ_NAME,
            EVAS_SMART_CLASS_VERSION,
            _smart_add,
            _smart_del,
            _smart_move,
            _smart_resize,
            _smart_show,
            _smart_hide,
            NULL,
            _smart_clip_set,
            _smart_clip_unset,
            NULL,
            _smart_member_add,
            NULL,
            NULL
        };
        smart = evas_smart_class_new(&sc);
    }
}

    static void
_smart_add(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = calloc(1, sizeof(Smart_Data));
    if (!sd) return;
    evas_object_smart_data_set(obj, sd);

    sd->obj = evas_object_rectangle_add(evas_object_evas_get(obj));
    evas_object_smart_member_add(sd->obj, obj);

    sd->back = evas_object_rectangle_add(evas_object_evas_get(obj));
    evas_object_smart_member_add(sd->back, obj);
    evas_object_color_set(sd->back,0,0,0,0);
    evas_object_lower(sd->back);
    evas_object_show(sd->back);


    sd->catch_event = evas_object_rectangle_add(evas_object_evas_get(obj));
    evas_object_smart_member_add(sd->catch_event, obj);
    evas_object_event_callback_add(sd->catch_event,EVAS_CALLBACK_KEY_DOWN, key_event_cb, obj);
    evas_object_event_callback_add(sd->catch_event,EVAS_CALLBACK_MOUSE_MOVE, mouse_event_cb, obj);
    evas_object_event_callback_add(sd->catch_event,EVAS_CALLBACK_MOUSE_IN, mouse_event_cb, obj);
    evas_object_color_set(sd->catch_event,0,0,0,0);
    evas_object_repeat_events_set(sd->catch_event,1);
    evas_object_hide(sd->catch_event);

    evas_object_propagate_events_set(obj,0);

    _eyelight_object_init(obj);
}

    static void
_smart_del(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    eyelight_viewer_destroy(&(sd->pres));
    EYELIGHT_FREE(sd);
}

static void
_smart_member_add(Evas_Object *obj, Evas_Object *member __UNUSED__)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    evas_object_raise(sd->catch_event);
}


    static void
_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    eyelight_viewer_move(sd->pres, x, y);
    evas_object_move(sd->obj, x, y);
    evas_object_move(sd->catch_event,x,y);
}

    static void
_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    eyelight_viewer_resize(sd->pres,w,h);
    evas_object_resize(sd->obj,w,h);
    evas_object_resize(sd->catch_event,w,h);
}

    static void
_smart_show(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    evas_object_show(sd->obj);
}

    static void
_smart_hide(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    evas_object_hide(sd->obj);
}

    static void
_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    evas_object_clip_set(sd->obj, clip);
}

    static void
_smart_clip_unset(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    evas_object_clip_unset(sd->obj);
}
