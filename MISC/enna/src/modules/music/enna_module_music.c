/* Interface */

#include "enna.h"
#include "smart_player.h"

#define ENNA_MODULE_NAME "music"

static void _create_gui();
static void _create_mediaplayer_gui();
static void _class_init(int dummy);
static void _class_shutdown(int dummy);
static void _class_show(int dummy);
static void _class_hide(int dummy);
static void _class_event(void *event_info);
static int em_init(Enna_Module *em);
static int em_shutdown(Enna_Module *em);
static void _list_transition_core(Evas_List *files, unsigned char direction);
static void _list_transition_left_end_cb(void *data, Evas_Object *o,
        const char *sig, const char *src);
static void _list_transition_right_end_cb(void *data, Evas_Object *o,
        const char *sig, const char *src);
static void _browse(void *data, void *data2);
static void _hilight(void *data, void *data2);
static void _browse_down();
static void _activate();
static int _eos_cb(void *data, int type, void *event);
static void _next_song(void);
static void _prev_song(void);
static int _show_mediaplayer_cb(void *data);

typedef struct _Enna_Module_Music Enna_Module_Music;

typedef enum _MUSIC_STATE MUSIC_STATE;

enum _MUSIC_STATE
{
    LIST_VIEW,
    MEDIAPLAYER_VIEW,
    DEFAULT_VIEW
};

struct _Enna_Module_Music
{
    Evas *e;
    Evas_Object *o_edje;
    Evas_Object *o_list;
    Evas_Object *o_location;
    Evas_Object *o_mediaplayer;
    Enna_Class_Vfs *vfs;
    Ecore_Timer *timer;
    Enna_Module *em;
    MUSIC_STATE state;
    Ecore_Timer *timer_show_mediaplayer;
    char *prev_selected;
    Ecore_Event_Handler *eos_event_handler;
    unsigned char is_root: 1;
};

static Enna_Module_Music *mod;

EAPI Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    "music"
};

static Enna_Class_Activity
        class =
        { "music", 1, "music", NULL, "icon/music",
                { _class_init, _class_shutdown, _class_show, _class_hide,
                        _class_event }, NULL };

static void _class_init(int dummy)
{
    _create_gui();
    enna_content_append("music", mod->o_edje);
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
        case MEDIAPLAYER_VIEW:
            edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
            edje_object_signal_emit(mod->o_edje, "list,hide", "enna");
            break;
        default:
            enna_log(ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                    "Error State Unknown in music module\n");
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
    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Key pressed music : %s\n",
            ev->keysymbol);
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
            if (mod->o_mediaplayer)
            {
                enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "delay 10s");
                ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
                mod->timer_show_mediaplayer = ecore_timer_add(10,
                        _show_mediaplayer_cb, NULL);

            }
            break;
        case MEDIAPLAYER_VIEW:
            switch (key)
            {
                case ENNA_KEY_OK:
	        case ENNA_KEY_SPACE:
                    enna_mediaplayer_play();
                    break;
                case ENNA_KEY_RIGHT:
                {
                    _next_song();
                    break;
                }
                case ENNA_KEY_LEFT:
                {
                    _prev_song();
                    break;
                }
                case ENNA_KEY_CANCEL:
                    ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
                    mod->timer_show_mediaplayer = ecore_timer_add(10,
                            _show_mediaplayer_cb, NULL);
                    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "Add timer 10s");
                    edje_object_signal_emit(mod->o_edje, "mediaplayer,hide",
                            "enna");
                    edje_object_signal_emit(mod->o_edje, "list,show", "enna");
                    mod->state = LIST_VIEW;
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
        mod->state = MEDIAPLAYER_VIEW;
        edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
        edje_object_signal_emit(mod->o_edje, "list,hide", "enna");
        ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
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

static void _list_transition_core(Evas_List *files, unsigned char direction)
{
    Evas_Object *o_list, *oe;
    Evas_List *l;

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
    if (evas_list_count(files))
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
        Evas_List *l, *categories;
        mod->is_root = 1;
        categories = enna_vfs_get(ENNA_CAPS_MUSIC);
        enna_list_icon_size_set(o_list, 200, 200);
        for (l = categories; l; l = l->next)
        {
            Enna_Class_Vfs *cat;
            Evas_Object *icon;
            Evas_Object *item;

            cat = l->data;
            icon = edje_object_add(mod->em->evas);
            edje_object_file_set(icon, enna_config_theme_get(), cat->icon);
            item = enna_listitem_add(mod->em->evas);
            enna_listitem_create_simple(item, icon, cat->label);
            enna_list_append(o_list, item, _browse, NULL, cat, NULL);
        }
        if (mod->o_mediaplayer)
        {
            Evas_Object *item;
            Enna_Metadata *metadata;
            Evas_Object *cover = NULL;
            const char *cover_file;
            mod->is_root = 0;
            metadata = enna_mediaplayer_metadata_get();
            item = enna_listitem_add(mod->em->evas);

            cover_file = enna_cover_album_get(metadata->music->artist,
                    metadata->music->album, metadata->uri);
            if (cover_file)
            {
                cover = enna_image_add(mod->em->evas);
                enna_image_file_set(cover, cover_file);
            }

            enna_listitem_create_full(item, cover, "Playing Now :",
                    metadata->title, metadata->music->album,
                    metadata->music->artist);
            enna_list_append(o_list, item, NULL, NULL, NULL, NULL);
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
        Evas_List *files;

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
    Evas_List *files, *l;

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
            prev_uri = prev_vfs->uri ? strdup(prev_vfs->uri) : NULL;
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
                        enna_mediaplayer_play();
                    }
                    i++;
                }
            }
            _create_mediaplayer_gui();
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

static int _update_position_timer(void *data)
{

    double pos;
    double length;

    length = enna_mediaplayer_length_get();
    pos = enna_mediaplayer_position_get();

    enna_smart_player_position_set(mod->o_mediaplayer, pos, length);
    return 1;
}

static void _next_song()
{
    Enna_Metadata *metadata;
    if (!enna_mediaplayer_next())
    {
        metadata = enna_mediaplayer_metadata_get();
        if (metadata)
        {
            enna_smart_player_metadata_set(mod->o_mediaplayer, metadata);
        }
    }
}

static void _prev_song()
{
    Enna_Metadata *metadata;
    if (!enna_mediaplayer_prev())
    {
        metadata = enna_mediaplayer_metadata_get();
        if (metadata)
        {
            enna_smart_player_metadata_set(mod->o_mediaplayer, metadata);
        }
    }
}

static int _eos_cb(void *data, int type, void *event)
{
    /* EOS received, update metadata */
    _next_song();
    return 1;
}

static void _create_mediaplayer_gui()
{
    Evas_Object *o;
    Enna_Metadata *metadata;

    mod->state = MEDIAPLAYER_VIEW;

    if (mod->o_mediaplayer)
    {
        ENNA_TIMER_DEL(mod->timer);
        ecore_event_handler_del(mod->eos_event_handler);
        evas_object_del(mod->o_mediaplayer);
    }

    mod->eos_event_handler = ecore_event_handler_add(
            ENNA_EVENT_MEDIAPLAYER_EOS, _eos_cb, NULL);

    o = enna_smart_player_add(mod->em->evas);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.mediaplayer", o);
    evas_object_show(o);
    metadata = enna_mediaplayer_metadata_get();
    if (metadata)
        enna_smart_player_metadata_set(o, metadata);

    mod->o_mediaplayer = o;
    mod->timer = ecore_timer_add(1, _update_position_timer, NULL);

    edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
    edje_object_signal_emit(mod->o_edje, "list,hide", "enna");

}

static void _create_gui()
{

    Evas_Object *o, *oe;
    Evas_List *l, *categories;
    Evas_Object *icon;

    mod->state = LIST_VIEW;
    /* First creation : show root */
    mod->is_root = 1;
    mod->timer_show_mediaplayer = NULL;
    o = edje_object_add(mod->em->evas);
    edje_object_file_set(o, enna_config_theme_get(), "module/music");
    mod->o_edje = o;
    mod->prev_selected = NULL;
    mod->timer_show_mediaplayer = NULL;
    mod->eos_event_handler = NULL;
    /* Create List */
    o = enna_list_add(mod->em->evas);
    oe = enna_list_edje_object_get(o);
    enna_list_freeze(o);
    edje_object_signal_emit(oe, "list,right,now", "enna");

    categories = enna_vfs_get(ENNA_CAPS_MUSIC);
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
    /* FIXME we must test play state instead of object here */
    if (mod->o_mediaplayer)
    {
        Evas_Object *item;
        Enna_Metadata *metadata;
        Evas_Object *cover = NULL;
        const char *cover_file;

        metadata = enna_mediaplayer_metadata_get();
        item = enna_listitem_add(mod->em->evas);

        cover_file = enna_cover_album_get(metadata->music->artist,
                metadata->music->album, metadata->uri);
        if (cover_file)
        {
            cover = enna_image_add(mod->em->evas);
            enna_image_file_set(cover, cover_file);
        }

        enna_listitem_create_full(item, cover, "Playing Now :",
                metadata->title, metadata->music->album,
                metadata->music->artist);
        enna_list_append(o, item, NULL, NULL, NULL, NULL);
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
    edje_object_file_set(icon, enna_config_theme_get(), "icon/music_mini");
    enna_location_append(o, "Music", icon, NULL, NULL, NULL);
    mod->o_location = o;
}

/* Module interface */

static int em_init(Enna_Module *em)
{
    mod = calloc(1, sizeof(Enna_Module_Music));
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
    ENNA_TIMER_DEL(mod->timer);
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
