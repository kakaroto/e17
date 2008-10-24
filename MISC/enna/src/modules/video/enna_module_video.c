/* Interface */

#include "enna.h"
#include "smart_player.h"

#define ENNA_MODULE_NAME "video"

static void _create_gui();
static void _create_video_info_gui();
static void _video_info_prev();
static void _video_info_next();
static void _create_videoplayer_gui();
static void _class_init(int dummy);
static void _class_shutdown(int dummy);
static void _class_show(int dummy);
static void _class_hide(int dummy);
static void _class_event(void *event_info);
static int em_init(Enna_Module *em);
static int em_shutdown(Enna_Module *em);
static void _seek_video(double value);
static void _list_transition_core(Eina_List *files, unsigned char direction);
static void _list_transition_left_end_cb(void *data, Evas_Object *o,
        const char *sig, const char *src);
static void _list_transition_right_end_cb(void *data, Evas_Object *o,
        const char *sig, const char *src);
static void _browse(void *data, void *data2);
static void _hilight(void *data, void *data2);
static void _browse_down();
static void _activate();
static int _eos_cb(void *data, int type, void *event);
static int _show_mediaplayer_cb(void *data);

typedef struct _Enna_Module_Video Enna_Module_Video;

typedef enum _VIDEO_STATE VIDEO_STATE;

enum _VIDEO_STATE
{
    LIST_VIEW,
    VIDEOPLAYER_VIEW,
    VIDEO_INFO_VIEW,
    DEFAULT_VIEW,
};

struct _Enna_Module_Video
{
    Evas *e;
    Evas_Object *o_edje;
    Evas_Object *o_list;
    Evas_Object *o_location;
    Evas_Object *o_switcher;
    Evas_Object *o_mediaplayer;
    Evas_Object *o_mediaplayer_old;
    Enna_Class_Vfs *vfs;
    Enna_Module *em;
    VIDEO_STATE state;
    Ecore_Timer *timer_show_mediaplayer;
    char *prev_selected;
    Ecore_Event_Handler *eos_event_handler;
    unsigned char is_root: 1;
};

static Enna_Module_Video *mod;

EAPI Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    "video"
};

static Enna_Class_Activity
        class =
        { "video", 1, "video", NULL, "icon/video",
                { _class_init, _class_shutdown, _class_show, _class_hide,
                        _class_event }, NULL };

static void _class_init(int dummy)
{
    _create_gui();
    enna_content_append("video", mod->o_edje);
}

static void _class_shutdown(int dummy)
{
}

static void _class_show(int dummy)
{
    edje_object_signal_emit(mod->o_edje, "module,show", "enna");
    switch (mod->state)
    {
        case LIST_VIEW:
            edje_object_signal_emit(mod->o_edje, "list,show", "enna");
            edje_object_signal_emit(mod->o_edje, "mediaplayer,hide", "enna");
            break;
        case VIDEO_INFO_VIEW:
            edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
            edje_object_signal_emit(mod->o_edje, "list,hide", "enna");
            break;
        case VIDEOPLAYER_VIEW:
            break;
        default:
            enna_log(ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                    "State Unknown in video module");
    }

}

static void _class_hide(int dummy)
{
    edje_object_signal_emit(mod->o_edje, "module,hide", "enna");
}

static void _class_event(void *event_info)
{
    Ecore_X_Event_Key_Down *ev = event_info;
    enna_key_t key = enna_get_key(ev);

    switch (mod->state)
    {
        case LIST_VIEW:
            switch (key)
            {
                case ENNA_KEY_LEFT:
                case ENNA_KEY_CANCEL:
                    if (!mod->is_root)
                        _browse_down();
                    else
                    {
                        enna_content_hide();
                        enna_mainmenu_show(enna->o_mainmenu);
                     }
                    break;
                case ENNA_KEY_RIGHT:
                case ENNA_KEY_OK:
	        case ENNA_KEY_SPACE:
                    _activate();
                    break;
                default:
                    enna_list_event_key_down(mod->o_list, event_info);
            }
            break;
        case VIDEO_INFO_VIEW:
            switch (key)
            {
                case ENNA_KEY_CANCEL:
                    mod->state = LIST_VIEW;
                    ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
                    mod->timer_show_mediaplayer = ecore_timer_add(10,
                            _show_mediaplayer_cb, NULL);
                    edje_object_signal_emit(mod->o_edje, "mediaplayer,hide",
                            "enna");
                    edje_object_signal_emit(mod->o_edje, "list,show", "enna");
                    break;
                case ENNA_KEY_RIGHT:
                    _video_info_next();
                    break;
                case ENNA_KEY_LEFT:
                    _video_info_prev();
                    break;
                case ENNA_KEY_OK:
                case ENNA_KEY_SPACE:
                    _create_videoplayer_gui();
                    break;
                default:
                    break;
            }
            break;
        case VIDEOPLAYER_VIEW:
            switch (key)
            {
                case ENNA_KEY_CANCEL:
                case ENNA_KEY_OK:
                    enna_mediaplayer_stop();
                    enna_smart_player_hide_video(mod->o_mediaplayer);
                    mod->state = VIDEO_INFO_VIEW;
                    edje_object_signal_emit(mod->o_edje, "mediaplayer,show",
                            "enna");
                    break;
                case ENNA_KEY_SPACE:
                    enna_mediaplayer_play();
                    break;
                case ENNA_KEY_RIGHT:
                    _seek_video(+0.01);
                    break;
                case ENNA_KEY_LEFT:
                    _seek_video(-0.01);
                    break;
                case ENNA_KEY_UP:
                    _seek_video(+0.05);
                    break;
                case ENNA_KEY_DOWN:
                    _seek_video(-0.05);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

static int _show_mediaplayer_cb(void *data)
{

    if (mod->o_mediaplayer)
    {
        mod->state = VIDEO_INFO_VIEW;
        edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
        edje_object_signal_emit(mod->o_edje, "list,hide", "enna");
        ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
        mod->timer_show_mediaplayer = NULL;
    }

    return 0;
}

static void _activate()
{
    Enna_Vfs_File *f;
    Enna_Class_Vfs *vfs;

    vfs = (Enna_Class_Vfs*)enna_list_selected_data_get(mod->o_list);
    f = (Enna_Vfs_File*)enna_list_selected_data2_get(mod->o_list);
    _browse(vfs, f);

}

static void _seek_video(double value)
{
    double pos = 0.0;
    double len = 0.0;
    double seek = 0.0;

    pos = enna_mediaplayer_position_get();
    len = enna_mediaplayer_length_get();
    if (len)
        seek = (pos / len) + value;
    if (seek <= 1.0 && seek >= 0.0)
        enna_mediaplayer_seek(seek);

    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "Seek value : %f", seek);

}

static void _list_transition_core(Eina_List *files, unsigned char direction)
{
    Evas_Object *o_list, *oe;
    Eina_List *l;

    o_list = mod->o_list;
    oe = enna_list_edje_object_get(o_list);
    if (!direction)
        edje_object_signal_callback_del(oe, "list,transition,end", "edje",
                _list_transition_left_end_cb);
    else
        edje_object_signal_callback_del(oe, "list,transition,end", "edje",
                _list_transition_right_end_cb);

    enna_list_freeze(o_list);
    evas_object_del(o_list);

    o_list = enna_list_add(mod->em->evas);
    oe = enna_list_edje_object_get(o_list);
    evas_object_show(o_list);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.list", o_list);

    if (direction == 0)
        edje_object_signal_emit(oe, "list,right,now", "enna");
    else
        edje_object_signal_emit(oe, "list,left,now", "enna");

    enna_list_freeze(o_list);
    enna_list_icon_size_set(o_list, 200, 200);
    if (eina_list_count(files))
    {
        int i = 0;
        mod->is_root = 0;
        /* Create list of files */
        for (l = files, i = 0; l; l = l->next, i++)
        {
            Enna_Vfs_File *f;
            Evas_Object *icon;
            Evas_Object *item;

            f = l->data;

            if (f->icon_file && f->icon_file[0] == '/')
            {
                icon = enna_image_add(mod->em->evas);
                enna_image_file_set(icon, f->icon_file);
            }
            else
            {
                icon = edje_object_add(mod->em->evas);
                edje_object_file_set(icon, enna_config_theme_get(), f->icon);
            }

            item = enna_listitem_add(mod->em->evas);
            enna_listitem_create_simple(item, icon, f->label);
            enna_list_append(o_list, item, _browse, _hilight, mod->vfs, f);

        }

    }
    else if (!direction)
    {
        /* No files returned : create no media item */
        Evas_Object *icon;
        Evas_Object *item;
        mod->is_root = 0;
        icon = edje_object_add(mod->em->evas);
        edje_object_file_set(icon, enna_config_theme_get(), "icon_nofile");
        item = enna_listitem_add(mod->em->evas);
        enna_listitem_create_simple(item, icon, "No Media found!");
        enna_list_append(o_list, item, NULL, NULL, NULL, NULL);
    }
    else
    {
        /* Browse down and no file detected : Root */
        Eina_List *l, *categories;
        mod->is_root = 1;
        categories = enna_vfs_get(ENNA_CAPS_VIDEO);
        enna_list_icon_size_set(o_list, 200, 200);
        for (l = categories; l; l = l->next)
        {
            Enna_Class_Vfs *cat;
            Evas_Object *icon;
            Evas_Object *item;

            cat = l->data;
            enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "cat : %s", cat->label);
            icon = edje_object_add(mod->em->evas);
            edje_object_file_set(icon, enna_config_theme_get(), cat->icon);
            item = enna_listitem_add(mod->em->evas);
            enna_listitem_create_simple(item, icon, cat->label);
            enna_list_append(o_list, item, _browse, NULL, cat, NULL);
        }

        mod->vfs = NULL;
    }

    enna_list_thaw(o_list);
    if (mod->prev_selected)
    {
        enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "prev_selected : %s",
                mod->prev_selected);
        if (!enna_list_jump_label(o_list, mod->prev_selected) > 0)
            enna_list_selected_set(o_list, 0);

        free(mod->prev_selected);
        mod->prev_selected = NULL;
    }
    else
    {
        enna_list_selected_set(o_list, 0);
    }

    mod->o_list = o_list;
    edje_object_signal_emit(oe, "list,default", "enna");

}

static void _list_transition_left_end_cb(void *data, Evas_Object *o,
        const char *sig, const char *src)
{

    _list_transition_core(data, 0);

}

static void _list_transition_right_end_cb(void *data, Evas_Object *o,
        const char *sig, const char *src)
{
    _list_transition_core(data, 1);
}

static void _browse_down()
{
    if (mod->vfs && mod->vfs->func.class_browse_down)
    {
        Evas_Object *o, *oe;
        Eina_List *files;

        files = mod->vfs->func.class_browse_down();
        o = mod->o_list;
        /* Clear list and add new items */
        oe = enna_list_edje_object_get(o);
        edje_object_signal_callback_add(oe, "list,transition,end", "edje",
                _list_transition_right_end_cb, files);
        edje_object_signal_emit(oe, "list,right", "enna");

        mod->prev_selected = strdup(enna_location_label_get_nth(
                mod->o_location, enna_location_count(mod->o_location) - 1));
        enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "prev selected : %s",
                mod->prev_selected);
        enna_location_remove_nth(mod->o_location,
                enna_location_count(mod->o_location) - 1);
    }
}

static void _hilight(void *data, void *data2)
{
    /*Enna_Class_Vfs *vfs = data;
     Enna_Vfs_File *file = data2;*/
}

static void _browse(void *data, void *data2)
{

    Enna_Class_Vfs *vfs = data;
    Enna_Vfs_File *file = data2;
    Eina_List *files, *l;

    if (!vfs)
        return;

    if (vfs->func.class_browse_up)
    {

        Evas_Object *o, *oe;

        if (!file)
        {
            Evas_Object *icon;
            /* file param is NULL => create Root menu */
            files = vfs->func.class_browse_up(NULL);
            icon = edje_object_add(mod->em->evas);
            edje_object_file_set(icon, enna_config_theme_get(),
                    "icon/home_mini");
            enna_location_append(mod->o_location, "Root", icon, _browse, vfs,
                    NULL);
        }
        else if (file->is_directory)
        {
            /* File selected is a directory */
            enna_location_append(mod->o_location, file->label, NULL, _browse,
                    vfs, file);
            files = vfs->func.class_browse_up(file->uri);
        }
        else if (!file->is_directory)
        {
            /* File selected is a regular file */
            int i = 0;
            Enna_Vfs_File *prev_vfs;
            char *prev_uri;

            prev_vfs = vfs->func.class_vfs_get();
            prev_uri = strdup(prev_vfs->uri);
            files = vfs->func.class_browse_up(prev_uri);
            ENNA_FREE(prev_uri);
            enna_mediaplayer_playlist_clear();
            enna_mediaplayer_stop();
            for (l = files; l; l = l->next)
            {
                Enna_Vfs_File *f;
                f = l->data;

                if (!f->is_directory)
                {
                    enna_mediaplayer_uri_append(f->uri, f->label);
                    if (!strcmp(f->uri, file->uri))
                    {
                        enna_mediaplayer_select_nth(i);
                    }
                    i++;
                }
            }
            _create_video_info_gui();
            return;
        }

        mod->vfs = vfs;
        o = mod->o_list;
        /* Clear list and add new items */
        oe = enna_list_edje_object_get(o);
        edje_object_signal_callback_add(oe, "list,transition,end", "edje",
                _list_transition_left_end_cb, files);
        edje_object_signal_emit(oe, "list,left", "enna");
    }

}



static void _create_videoplayer_gui()
{
    mod->state = VIDEOPLAYER_VIEW;
    ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
    edje_object_signal_emit(mod->o_edje, "mediaplayer,hide", "enna");
    mod->state = VIDEOPLAYER_VIEW;
    enna_mediaplayer_play();
    enna_smart_player_show_video(mod->o_mediaplayer);
}


static void
_switcher_transition_done_cb(void *data, Evas_Object *obj, void *event_info)
{
    ENNA_OBJECT_DEL(mod->o_mediaplayer_old);
    mod->o_mediaplayer_old = NULL;

}

static void _video_info_prev()
{
    int n;
    n = enna_mediaplayer_selected_get();

    if (n > 0)
    {
        Enna_Metadata *m;
        Evas_Object *o;
        n--;
        enna_mediaplayer_select_nth(n);

        ENNA_OBJECT_DEL(mod->o_mediaplayer_old);
        mod->o_mediaplayer_old = NULL;

        o = enna_smart_player_add(mod->em->evas);
        evas_object_show(o);
         m = enna_mediaplayer_metadata_get();
        enna_smart_player_snapshot_set(o, m);
        enna_smart_player_cover_set(o, m);
        enna_smart_player_metadata_set(o, m);
        enna_metadata_free(m);
        mod->o_mediaplayer_old = mod->o_mediaplayer;
        mod->o_mediaplayer = o;
        enna_switcher_objects_switch(mod->o_switcher, o);
    }


}

static void _video_info_next()
{
    int n;
    n = enna_mediaplayer_selected_get();

    if (n < enna_mediaplayer_playlist_count - 2)
    {
        Enna_Metadata *m;
        Evas_Object *o;
        n++;

        ENNA_OBJECT_DEL(mod->o_mediaplayer_old);
        mod->o_mediaplayer_old = NULL;

        enna_mediaplayer_select_nth(n);
        o = enna_smart_player_add(mod->em->evas);
        evas_object_show(o);
        m = enna_mediaplayer_metadata_get();
        enna_smart_player_snapshot_set(o, m);
        enna_smart_player_cover_set(o, m);
        enna_smart_player_metadata_set(o, m);
        enna_metadata_free(m);
        mod->o_mediaplayer_old = mod->o_mediaplayer;
        mod->o_mediaplayer = o;
        enna_switcher_objects_switch(mod->o_switcher, o);
    }
}

static void _create_video_info_gui()
{
    Evas_Object *o;
    Enna_Metadata *m;

    mod->state = VIDEO_INFO_VIEW;

    ENNA_OBJECT_DEL(mod->o_mediaplayer_old);
    mod->o_mediaplayer_old = NULL;
    ENNA_OBJECT_DEL(mod->o_mediaplayer);
    if (mod->eos_event_handler)
        ecore_event_handler_del(mod->eos_event_handler);
    mod->eos_event_handler = ecore_event_handler_add(ENNA_EVENT_MEDIAPLAYER_EOS, _eos_cb, NULL);

    o = enna_switcher_add(mod->em->evas);
    evas_object_show(o);
    evas_object_smart_callback_add(o, "transition_done", _switcher_transition_done_cb, mod);
    mod->o_switcher = o;

    o = enna_smart_player_add(mod->em->evas);
    evas_object_show(o);
    mod->o_mediaplayer = o;

    m = enna_mediaplayer_metadata_get();
    enna_smart_player_snapshot_set(mod->o_mediaplayer, m);
    enna_smart_player_cover_set(mod->o_mediaplayer, m);
    enna_smart_player_metadata_set(mod->o_mediaplayer, m);
    enna_metadata_free(m);

    enna_switcher_objects_switch(mod->o_switcher, mod->o_mediaplayer);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.mediaplayer", mod->o_switcher);

    edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
    edje_object_signal_emit(mod->o_edje, "list,hide", "enna");


}

static int _eos_cb(void *data, int type, void *event)
{
    /* EOS received, update metadata */
    edje_object_signal_emit(mod->o_edje, "mediaplayer,hide", "enna");
    edje_object_signal_emit(mod->o_edje, "list,show", "enna");
    evas_object_del(mod->o_mediaplayer);
    mod->o_mediaplayer = NULL;
    return 1;
}

static void _create_gui()
{

    Evas_Object *o, *oe;
    Eina_List *l, *categories;
    Evas_Object *icon;

    mod->state = LIST_VIEW;
    /* First creation : show root */
    mod->is_root = 1;
    mod->timer_show_mediaplayer = NULL;
    mod->eos_event_handler = NULL;
    o = edje_object_add(mod->em->evas);
    edje_object_file_set(o, enna_config_theme_get(), "module/video");
    mod->o_edje = o;
    mod->prev_selected = NULL;
    /* Create List */
    o = enna_list_add(mod->em->evas);
    oe = enna_list_edje_object_get(o);
    enna_list_freeze(o);
    edje_object_signal_emit(oe, "list,right,now", "enna");

    categories = enna_vfs_get(ENNA_CAPS_VIDEO);
    enna_list_icon_size_set(o, 200, 200);
    for (l = categories; l; l = l->next)
    {
        Evas_Object *item;
        Enna_Class_Vfs *cat;

        cat = l->data;
        icon = edje_object_add(mod->em->evas);
        enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "icon : %s", cat->icon);
        edje_object_file_set(icon, enna_config_theme_get(), cat->icon);
        item = enna_listitem_add(mod->em->evas);
        enna_listitem_create_simple(item, icon, cat->label);
        enna_list_append(o, item, _browse, NULL, cat, NULL);
    }

    enna_list_thaw(o);
    mod->vfs = NULL;
    evas_object_show(o);
    enna_list_selected_set(o, 0);
    mod->o_list = o;
    edje_object_part_swallow(mod->o_edje, "enna.swallow.list", o);
    edje_object_signal_emit(oe, "list,default", "enna");

    /* Create Location bar */
    o = enna_location_add(mod->em->evas);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.location", o);

    icon = edje_object_add(mod->em->evas);
    edje_object_file_set(icon, enna_config_theme_get(), "icon/video_mini");
    enna_location_append(o, "Video", icon, NULL, NULL, NULL);
    mod->o_location = o;
}

/* Module interface */

static int em_init(Enna_Module *em)
{
    mod = calloc(1, sizeof(Enna_Module_Video));
    mod->em = em;
    em->mod = mod;

    enna_activity_add(&class);

    return 1;
}

static int em_shutdown(Enna_Module *em)
{
    ENNA_OBJECT_DEL(mod->o_edje);
    enna_list_freeze(mod->o_list);
    ENNA_OBJECT_DEL(mod->o_list);
    ENNA_OBJECT_DEL(mod->o_location);
    ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
    ENNA_OBJECT_DEL(mod->o_mediaplayer);
    if (mod->vfs && mod->vfs->func.class_shutdown)
    {
        mod->vfs->func.class_shutdown(0);
    }
    if (mod->prev_selected)
        free(mod->prev_selected);

    free(mod);
    return 1;
}

EAPI void module_init(Enna_Module *em)
{
    if (!em)
        return;

    if (!em_init(em))
        return;
}

EAPI void module_shutdown(Enna_Module *em)
{
    em_shutdown(em);
}
