
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <stdlib.h>
#include <stdio.h>
#include <Edje.h>
#include <string.h>
#include <sys/wait.h>
#include <Eyelight_Smart.h>

#include "../../config.h"

Ecore_Evas *ee;
Evas_Object *container;

static void app_resize(Ecore_Evas *ee);
static int app_signal_exit(void *data, int ev_type, void *ev);
static void app_delete_request(Ecore_Evas *ee);

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
    EYELIGHT_FULLSCREEN,
    EYELIGHT_TABLEOFCONTENTS
} Eyelight_Event_Action;

char* help_msg_cmd ="h:<NewLine>"
"left/right:<NewLine>"
"up:<NewLine>"
"f:<NewLine>"
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
"fullscreen mode<NewLine>"
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

Evas_Object *eyelight_smart;


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
    { "f", EYELIGHT_FULLSCREEN },
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

void slide_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
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
                    if (strlen(event->key) == 1 && strchr("0123456789", *event->key) != NULL)
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
                case EYELIGHT_FULLSCREEN:
                    if(ecore_evas_fullscreen_get(ee))
                        ecore_evas_fullscreen_set(ee,0);
                    else
                        ecore_evas_fullscreen_set(ee,1);
                    break;
                case EYELIGHT_QUIT:
                    ecore_main_loop_quit();
                    return ;
                    break;
                default:
                    if (strlen(event->key) == 1 && strchr("0123456789", *event->key) != NULL)
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


void mouse_event_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    evas_object_focus_set(obj, 1);
}

void mouse_event_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Up *ev = event_info;

    if(ev->button == 1)
        eyelight_object_slide_next(eyelight_smart);
    else if(ev->button == 3)
        eyelight_object_slide_previous(eyelight_smart);
}

static const Ecore_Getopt options = {
    "eyelight",
    NULL,
    VERSION,
    "(C) 2009 Enlightenment, see AUTHORS.",
    "LGPL with advertisement, see COPYING",
    "Launch eyelight, presentation viewer\n\n"
        "%prog use Edje to display a presentation.  \n\n\n"
        "Examples: \n"
        "eyelight -p presentation/eyelight.elt -t theme/default -e opengl-x11 -o presentation/eyelight.eye\n"
        "eyelight -p presentation/eyelight.elt\n"
        "eyelight -y presentation/eyelight.eye\n",
        1,
        {
            ECORE_GETOPT_VERSION('V', "version"),
            ECORE_GETOPT_COPYRIGHT('R', "copyright"),
            ECORE_GETOPT_LICENSE('L', "license"),
            ECORE_GETOPT_STORE_STR('p', "presentation", "specify the elt presentation file"),
            ECORE_GETOPT_STORE_STR('t', "theme", "specify the edje file theme"),
            ECORE_GETOPT_STORE_STR('e', "engine", "ecore-evas engine to use"),
            ECORE_GETOPT_CALLBACK_NOARGS('E', "list-engines", "list ecore-evas engines",
                    ecore_getopt_callback_ecore_evas_list_engines, NULL),
            ECORE_GETOPT_STORE_TRUE('d', "display-areas", "display the borders of each area"),
            ECORE_GETOPT_STORE_STR('b', "no-thumbs-bg", "deactivate the creation of the thumbnails list in the background, saves a lot of memory, some mode (expose, slideshow) will be slow"),
            ECORE_GETOPT_STORE_TRUE('H', "hd", "Display the presentation with a default size of 1280x720"),
            ECORE_GETOPT_HELP('h', "help"),
	    ECORE_GETOPT_STORE_STR('o', "dump", "dump all generated value/image/theme inside a file use it with -y"),
	    ECORE_GETOPT_STORE_STR('y', "eye", "use an eye file, faster than elt + theme"),
            ECORE_GETOPT_SENTINEL
        }
};

int main(int argc, char*argv[])
{
    Evas          *evas;
    Evas_Coord w_win,h_win;

    unsigned char exit_option = 0;
    unsigned char engines_listed = 0;
    char* engine = NULL;
    char* theme = NULL;
    char* presentation = NULL;
    char* dump_in = NULL;
    char* dump_out = NULL;
    unsigned char with_border = 0;
    unsigned char no_thumbs_bg = 0;
    unsigned char hd = 0;

    Evas_Coord sizew = 1024;
    Evas_Coord sizeh = 768;

    if (!ecore_init()) {
        evas_shutdown ();
        return EXIT_FAILURE;
    }

    if (!ecore_evas_init()) {
        evas_shutdown ();
        ecore_shutdown ();
        return EXIT_FAILURE;
    }

    eina_log_level_set(5);
    eyelight_init();

    Ecore_Getopt_Value values[] = {
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_STR(presentation),
        ECORE_GETOPT_VALUE_STR(theme),
        ECORE_GETOPT_VALUE_STR(engine),
        ECORE_GETOPT_VALUE_BOOL(engines_listed),
        ECORE_GETOPT_VALUE_BOOL(with_border),
        ECORE_GETOPT_VALUE_BOOL(no_thumbs_bg),
        ECORE_GETOPT_VALUE_BOOL(hd),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
	ECORE_GETOPT_VALUE_STR(dump_out),
	ECORE_GETOPT_VALUE_STR(dump_in)
    };

    ecore_app_args_set(argc, (const char **) argv);
    int nonargs = ecore_getopt_parse(&options, values, argc, argv);
    if (nonargs < 0)
        return 1;
    else if (nonargs != argc)
    {
        fputs("Invalid non-option argument", stderr);
        ecore_getopt_help(stderr, &options);
        return 1;
    }

    if(exit_option || engines_listed)
        return 0;

    if ((presentation || theme) && dump_in)
    {
       fprintf(stderr, "You can't set an eye file as an input if you want to specify a theme and a presentation.\nBetter specify it with -o.\n");
       return EXIT_FAILURE;
    }

    if((dump_out || !dump_in) && !presentation)
    {
        fprintf(stderr,"A presentation is required !\n");
        return EXIT_FAILURE;
    }

    if(presentation && (ecore_file_is_dir(presentation) || !ecore_file_exists(presentation)))
    {
        fprintf(stderr,"The presentation file doesn't exists \n");
        exit(EXIT_FAILURE);
    } else if (dump_in && (ecore_file_is_dir(dump_in) || !ecore_file_exists(dump_in)))
    {
        fprintf(stderr,"The eye file doesn't exists \n");
        exit(EXIT_FAILURE);
    }

    if(hd)
    {
        sizew = 1280;
        sizeh = 720;
    }

    ee = ecore_evas_new(engine, 0, 0, sizew, sizeh, NULL);
    if(!ee)
    {
        fprintf(stderr,"Failed to init the evas engine! \n");
        evas_shutdown ();
        ecore_shutdown ();

        return EXIT_FAILURE;
    }

    edje_init();
    ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);
    ecore_evas_callback_delete_request_set (ee, app_delete_request);
    ecore_evas_title_set (ee, "Eyelight: presentation tools");
    ecore_evas_name_class_set (ee, "eyelight", "eyelight");
    ecore_evas_callback_resize_set (ee, app_resize);
    ecore_evas_show (ee);

    evas = ecore_evas_get (ee);
    evas_output_viewport_get(evas, NULL, NULL, &w_win, &h_win);

    eyelight_smart = eyelight_object_add(evas);
    eyelight_object_theme_file_set(eyelight_smart, theme);
    eyelight_object_presentation_file_set(eyelight_smart, presentation);
    eyelight_object_eye_file_set(eyelight_smart, dump_in);
    eyelight_object_dump_file_set(eyelight_smart, dump_out);
    eyelight_object_border_set(eyelight_smart, with_border);
    evas_object_move(eyelight_smart,0,0);
    evas_object_resize(eyelight_smart, sizew, sizeh);
    evas_object_show(eyelight_smart);

    container= evas_object_rectangle_add(evas);
    evas_object_color_set(container,0,0,0,0);
    evas_object_event_callback_add(container,EVAS_CALLBACK_KEY_DOWN, slide_cb, NULL);
    evas_object_event_callback_add(container,EVAS_CALLBACK_MOUSE_MOVE, mouse_event_cb, NULL);
    evas_object_event_callback_add(container,EVAS_CALLBACK_MOUSE_IN, mouse_event_cb, NULL);
    evas_object_event_callback_add(container,EVAS_CALLBACK_MOUSE_UP, mouse_event_up_cb, NULL);
    evas_object_repeat_events_set(container,1);
    evas_object_show(container);
    evas_object_resize(container, sizew, sizeh);

    evas_object_raise(container);

    ecore_main_loop_begin ();

    eyelight_shutdown();
    ecore_evas_shutdown ();
    ecore_shutdown ();
    evas_shutdown ();

    return 1;
}

static void app_resize(Ecore_Evas *ee)
{
    Evas_Coord w, h;
    Evas *evas;
    evas = ecore_evas_get(ee);
    evas_output_viewport_get(evas, NULL, NULL, &w, &h);
    evas_object_resize(eyelight_smart, w,h);
    evas_object_resize(container, w, h);
}

static int app_signal_exit(void *data, int ev_type, void *ev)
{
    ecore_main_loop_quit();
    return 1;
}

static void app_delete_request(Ecore_Evas *ee)
{
    ecore_main_loop_quit();
}

