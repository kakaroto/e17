
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <stdlib.h>
#include <stdio.h>
#include <Edje.h>
#include <string.h>
#include <sys/wait.h>
#include <Eyelight.h>

#include "../../config.h"

Ecore_Evas *ee;

static void app_resize(Ecore_Evas *ee);
static int app_signal_exit(void *data, int ev_type, void *ev);
static void app_delete_request(Ecore_Evas *ee);

Eyelight_Viewer *pres;

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

    switch (eyelight_viewer_state_get(pres))
    {
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            switch (action)
            {
                case EYELIGHT_RIGHT:
                    eyelight_viewer_expose_next(pres);
                    break;
                case EYELIGHT_LEFT:
                    eyelight_viewer_expose_previous(pres);
                    break;
                case EYELIGHT_WINDOW_NEXT:
                    eyelight_viewer_expose_window_next(pres);
                    break;
                case EYELIGHT_WINDOW_PREVIOUS:
                    eyelight_viewer_expose_window_previous(pres);
                    break;
                case EYELIGHT_UP:
                    eyelight_viewer_expose_up(pres);
                    break;
                case EYELIGHT_DOWN:
                    eyelight_viewer_expose_down(pres);
                    break;
                case EYELIGHT_QUIT:
                    eyelight_viewer_expose_stop(pres);
                    break;
                case EYELIGHT_SELECT:
                    eyelight_viewer_expose_select(pres);
                    break;
            }
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            switch (action)
            {
                case EYELIGHT_QUIT:
                    eyelight_viewer_gotoslide_stop(pres);
                    break;
                case EYELIGHT_SELECT:
                    eyelight_viewer_gotoslide_goto(pres);
                    break;
                case EYELIGHT_REMOVE:
                    eyelight_viewer_gotoslide_digit_last_remove(pres);
                    break;
                default:
                    if (strlen(event->key) == 1 && strchr("0123456789", *event->key) != NULL)
                        eyelight_viewer_gotoslide_digit_add(pres,atoi(event->key));
                    break;
            }
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            switch (action)
            {
                case EYELIGHT_RIGHT:
                case EYELIGHT_DOWN:
                    eyelight_viewer_slideshow_next(pres);
                    break;
                case EYELIGHT_LEFT:
                case EYELIGHT_UP:
                    eyelight_viewer_slideshow_previous(pres);
                    break;
                case EYELIGHT_QUIT:
                    eyelight_viewer_slideshow_stop(pres);
                    break;
                case EYELIGHT_SELECT:
                    eyelight_viewer_slideshow_select(pres);
                    break;
            }
            break;
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            switch (action)
            {
                case EYELIGHT_RIGHT:
                case EYELIGHT_DOWN:
                    eyelight_viewer_tableofcontents_next(pres);
                    break;
                case EYELIGHT_LEFT:
                case EYELIGHT_UP:
                    eyelight_viewer_tableofcontents_previous(pres);
                    break;
                case EYELIGHT_QUIT:
                    eyelight_viewer_tableofcontents_stop(pres);
                    break;
                case EYELIGHT_SELECT:
                    eyelight_viewer_tableofcontents_select(pres);
                    break;
            }
            break;
        default:
            switch (action)
            {
                case EYELIGHT_RIGHT:
                    eyelight_viewer_slide_next(pres);
                    break;
                case EYELIGHT_LEFT:
                    eyelight_viewer_slide_previous(pres);
                    break;
                case EYELIGHT_UP:
                    eyelight_viewer_expose_start(pres,eyelight_viewer_current_id_get(pres), 4,4);
                    break;
                case EYELIGHT_SLIDE:
                    eyelight_viewer_slideshow_start(pres,eyelight_viewer_current_id_get(pres));
                    break;
                case EYELIGHT_TABLEOFCONTENTS:
                    eyelight_viewer_tableofcontents_start(pres,eyelight_viewer_current_id_get(pres));
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
                        eyelight_viewer_gotoslide_start(pres);
                        eyelight_viewer_gotoslide_digit_add(pres,atoi(event->key));
                    }
            }
            break;
    }

    //printf("key: %s\n",event->key);
    //printf("key: %s\n",event->keyname);
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
        "eyelight -p presentation/eyelight.elt -t theme/default -e gl\n"
        "eyelight -p presentation/eyelight.elt",
        1,
        {
            ECORE_GETOPT_VERSION('V', "version"),
            ECORE_GETOPT_COPYRIGHT('R', "copyright"),
            ECORE_GETOPT_LICENSE('L', "license"),
            ECORE_GETOPT_STORE_STR('p', "presentation", "specify the elt presentation file"),
            ECORE_GETOPT_STORE_STR('t', "theme", "specify the edje file theme"),
            ECORE_GETOPT_STORE_STR('e', "engine", "specify the evas engine"),
            ECORE_GETOPT_STORE_TRUE('d', "display-areas", "display the borders of each area"),
            ECORE_GETOPT_STORE_STR('b', "no-thumbs-bg", "deactivate the creation of the thumbnails list in the background, saves a lot of memory, some mode (expose, slideshow) will be slow"),
#ifdef PDF_SUPPORT
            ECORE_GETOPT_STORE_STR('z', "generate-pdf", "specify the pdf file"),
            ECORE_GETOPT_STORE_INT('x', "size-pdf_x", "the size of each slide generated in a pdf file (default: 2048)"),
            ECORE_GETOPT_STORE_INT('y', "size-pdf_y", "the size of each slide generated in a pdf file (default: 1536)"),
#endif
            ECORE_GETOPT_HELP('h', "help"),
            ECORE_GETOPT_SENTINEL
        }
};

int main(int argc, char*argv[])
{
    Evas          *evas;
    Evas_Coord w_win,h_win;

    unsigned char exit_option = 0;
    char* engine = NULL;
    char* theme = NULL;
    char* presentation = NULL;
    unsigned char with_border = 0;
    unsigned char no_thumbs_bg = 0;
    unsigned char generate_pdf = 0;
    int pdf_size_w = 0;
    int pdf_size_h = 0;

    if (!ecore_init()) {
        evas_shutdown ();
        return EXIT_FAILURE;
    }

    if (!ecore_evas_init()) {
        evas_shutdown ();
        ecore_shutdown ();
        return EXIT_FAILURE;
    }

    Ecore_Getopt_Value values[] = {
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_STR(presentation),
        ECORE_GETOPT_VALUE_STR(theme),
        ECORE_GETOPT_VALUE_STR(engine),
        ECORE_GETOPT_VALUE_BOOL(with_border),
        ECORE_GETOPT_VALUE_BOOL(no_thumbs_bg),
#ifdef PDF_SUPPORT
        ECORE_GETOPT_VALUE_BOOL(generate_pdf),
        ECORE_GETOPT_VALUE_INT(pdf_size_w),
        ECORE_GETOPT_VALUE_INT(pdf_size_h),
#endif
        ECORE_GETOPT_VALUE_BOOL(exit_option),
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

    if(exit_option)
        return 0;

    if(pdf_size_w==0 || pdf_size_h == 0)
    {
        pdf_size_w = 1024*2;
        pdf_size_h = 768*2;
    }

    if(!presentation)
    {
        fprintf(stderr,"A presentation is required !\n");
        return EXIT_FAILURE;
    }

    if(ecore_file_is_dir(presentation) || !ecore_file_exists(presentation))
    {
        fprintf(stderr,"The presentation file doesn't exists \n");
        exit(EXIT_FAILURE);
    }

    if(generate_pdf)
    {
        ee = ecore_evas_buffer_new(pdf_size_w , pdf_size_h);
    }
    else if(!engine)
    {
        ee = ecore_evas_software_x11_new (NULL, 0,  0, 0, 1024, 768);
        if (!ee) {
            printf ("Can not find Software X11 engine. Trying DirectDraw engine...\n");
            ee = ecore_evas_software_ddraw_new (NULL,  0, 0, 1024, 768);
            if (!ee) {
                fprintf (stderr,"Can not find DirectDraw engine.\n");
                eyelight_viewer_destroy(&pres);
                evas_shutdown ();
                ecore_shutdown ();
                return EXIT_FAILURE;
            }
        }
    }
    else
    {
        //if(strcmp(engine, "fws")==0)
        //    ee = ecore_evas_fws_buffer_window(NULL, 0, 0, 720, 576, 0);
        if(strcmp(engine, "sdl")==0)
            ee = ecore_evas_sdl_new(NULL, 1024, 768, 0, 0, 0, 0);
        else if(strcmp(engine, "sdl16")==0)
            ee = ecore_evas_sdl16_new(NULL, 720, 576, 0, 0, 0, 0);
        else if(strcmp(engine,"x11")==0)
            ee = ecore_evas_software_x11_new (NULL, 0,  0, 0, 1024, 768);
        else if(strcmp(engine,"gl")==0)
            ee = ecore_evas_gl_x11_new (NULL, 0,  0, 0, 1024, 768);
        else if(strcmp(engine,"fb")==0)
            ee = ecore_evas_fb_new (NULL, 0, 1024, 768);
        else if(strcmp(engine,"xr")==0)
            ee = ecore_evas_xrender_x11_new (NULL, 0,  0, 0, 1024, 768);
        else if(strcmp(engine,"ddraw")==0)
            ee = ecore_evas_software_ddraw_new (NULL, 0, 0, 1024, 768);
        else
        {
            fprintf(stderr,"Unknow engine %s\n",engine);
            eyelight_viewer_destroy(&pres);
            evas_shutdown ();
            ecore_shutdown ();

            return EXIT_FAILURE;
        }
    }
    if(!ee)
    {
        fprintf(stderr,"Failed to init the evas engine! \n");
        eyelight_viewer_destroy(&pres);
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

    Evas_Object *container= edje_object_add(evas);

    if(!theme)
        pres  = eyelight_viewer_new(evas,presentation,NULL,with_border);
    else
        pres  = eyelight_viewer_new(evas,presentation,theme,with_border);
    if(!pres)
    {
        fprintf(stderr,"Failed to create the presentation !\n");
        evas_shutdown ();
        ecore_shutdown ();

        return EXIT_FAILURE;
    }


    //display the presentation
    if(!generate_pdf)
        eyelight_viewer_slides_init(pres,1024,768);
    else if(generate_pdf )
        eyelight_viewer_slides_init(pres,pdf_size_w,pdf_size_h);

    eyelight_viewer_resize_screen(pres,w_win,h_win);
    if(!no_thumbs_bg)
        eyelight_viewer_thumbnails_background_load_start(pres);

    if(eyelight_viewer_size_get(pres)>0)
    {
        edje_object_signal_emit(eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres)),"show","eyelight");
    }
    evas_object_event_callback_add(container,EVAS_CALLBACK_KEY_DOWN, slide_cb,pres);
    evas_object_focus_set(container,1);
    evas_object_repeat_events_set(container,1);

#ifdef PDF_SUPPORT
    if(generate_pdf)
    {
        pres->pdf_file = strdup(generate_pdf);
        ecore_timer_add(0.0, eyelight_pdf_generate_start_timer,pres);
    }
#endif

    ecore_main_loop_begin ();

    eyelight_viewer_destroy(&pres);

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
    eyelight_viewer_resize_screen(pres,w,h);
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

