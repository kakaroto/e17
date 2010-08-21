#include "main.h"

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
    EYELIGHT_HELP,
    EYELIGHT_REMOVE,
    EYELIGHT_SLIDE,
    EYELIGHT_TABLEOFCONTENTS
} Eyelight_Event_Action;

char* help_msg_cmd ="h:<NewLine>"
"left/right:<NewLine>"
"up:<NewLine>"
"t:<NewLine>"
"s:<NewLine>"
"0..9:<NewLine><NewLine>"
"In the expose mode:<NewLine>"
"left/right/up/down:<NewLine>"
"page up/page down:<NewLine>"
"return:<NewLine>"
"escape:<NewLine><NewLine>"
"In the slideshow or in the table of contents mode:<NewLine>"
"left/right/up/down:<NewLine>"
"escape:";

char* help_msg_desc ="display this message <NewLine>"
"move to the previous or next slide<NewLine>"
"start the expose mode<NewLine>"
"show the table of contents<NewLine>"
"start the slideshow mode<NewLine>"
"start selecting a slide by his number<NewLine><NewLine>"
"<NewLine>"
"move into the slide list<NewLine>"
"previous/next slide window<NewLine>"
"go to the selected slide<NewLine>"
"exit<NewLine><NewLine>"
"move into the slide list<NewLine>"
"exit";

Evas_Object *eyelight_smart = NULL;
Evas_Object *container = NULL;
Evas_Object *_win = NULL;

typedef struct _Eyelight_Key Eyelight_Key;
struct _Eyelight_Key
{
    const char *keyname;
    Eyelight_Event_Action action;
};

int is_help = 0;
Evas_Object* help_background;
Evas_Object* help_text_cmd;
Evas_Object* help_text_desc;

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
    { "h", EYELIGHT_HELP },
    { "H", EYELIGHT_HELP },
    { "BackSpace", EYELIGHT_REMOVE },
    { "s", EYELIGHT_SLIDE },
    { "t", EYELIGHT_TABLEOFCONTENTS}
};

    static void
display_help(Evas *e)
{
    Evas_Textblock_Style *style;
    int h_win, w_win;

    evas_output_viewport_get(e, NULL, NULL, &w_win, &h_win);

    help_background = evas_object_rectangle_add(e);
    help_text_cmd = evas_object_textblock_add(e);
    help_text_desc = evas_object_textblock_add(e);

    style =  evas_textblock_style_new();
    evas_font_path_append(e,PACKAGE_DATA_DIR"/viewer");
    evas_textblock_style_set(style,"DEFAULT='font=DejaVuSans-Bold font_size=20 align=left color=#000000 wrap=word style=soft_outline outline_color=#3779cb'"
            "NewLine= '+\n'");

    evas_object_resize(help_text_cmd, w_win,h_win);
    evas_object_move(help_text_cmd,200,140);
    evas_object_show(help_text_cmd);

    evas_object_textblock_style_set(help_text_cmd,style);
    evas_object_textblock_text_markup_set(help_text_cmd,help_msg_cmd);

    evas_object_resize(help_text_desc, w_win,h_win);
    evas_object_move(help_text_desc,450,140);
    evas_object_show(help_text_desc);

    evas_object_textblock_style_set(help_text_desc,style);
    evas_object_textblock_text_markup_set(help_text_desc,help_msg_desc);



    evas_object_show(help_background);
    evas_object_color_set(help_background, 0, 0, 0, 200);
    evas_object_resize(help_background, w_win,h_win);
    evas_object_move(help_background,0,0);

    is_help = 1;
}


void _mouse_event_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _slide_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

Evas_Object *slideshow_create()
{
    _win = elm_win_add(win, "Slide show", ELM_WIN_BASIC);
    elm_win_autodel_set(_win, 1);
    elm_win_fullscreen_set(_win, 1);

    eyelight_smart = eyelight_object_add(evas_object_evas_get(_win));
    eyelight_object_presentation_file_set(eyelight_smart, eyelight_object_presentation_file_get(pres));
    evas_object_show(eyelight_smart);

    elm_win_resize_object_add(_win, eyelight_smart);

    container= evas_object_rectangle_add(evas_object_evas_get(_win));
    evas_object_color_set(container,0,0,0,0);
    evas_object_event_callback_add(container,EVAS_CALLBACK_KEY_DOWN, _slide_cb, NULL);
    evas_object_event_callback_add(container,EVAS_CALLBACK_MOUSE_MOVE, _mouse_event_cb, NULL);
    evas_object_event_callback_add(container,EVAS_CALLBACK_MOUSE_IN, _mouse_event_cb, NULL);
    evas_object_repeat_events_set(container,1);
    evas_object_show(container);
    elm_win_resize_object_add(_win, container);


    evas_object_raise(container);


    evas_object_show(_win);
    return _win;
}

void _mouse_event_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    evas_object_focus_set(obj, 1);
}

void _slide_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Key_Down* event = (Evas_Event_Key_Down*) event_info;
    Eyelight_Event_Action action = EYELIGHT_NONE;
    unsigned int i;

    for (i = 0; i < sizeof (keys) / sizeof (Eyelight_Key); ++i)
        if (strcmp(event->keyname, keys[i].keyname) == 0)
        {
            action = keys[i].action;
            break;
        }

    if(is_help)
    {
        if(action == EYELIGHT_HELP
                || action == EYELIGHT_QUIT)
        {
            is_help = 0;
            evas_object_del(help_background);
            evas_object_del(help_text_cmd);
            evas_object_del(help_text_desc);
        }
        return;
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
                case EYELIGHT_HELP:
                    display_help(e);
                    break;
                case EYELIGHT_QUIT:
                    evas_object_del(_win);
                    return ;
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
}
