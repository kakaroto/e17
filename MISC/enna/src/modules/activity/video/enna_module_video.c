/* Interface */

#include "enna.h"
#include "smart_player.h"

#define ENNA_MODULE_NAME "video"

static void _create_menu();
static void _create_gui();
static void _create_video_info_gui();
static void _return_to_video_info_gui();
static void _create_videoplayer_gui();
static void _video_info_prev();
static void _video_info_next();
static void _browser_root_cb (void *data, Evas_Object *obj, void *event_info);
static void _browser_selected_cb (void *data, Evas_Object *obj, void *event_info);
static void _browser_browse_down_cb (void *data, Evas_Object *obj, void *event_info);

static void _seek_video(double value);
static void _browse(void *data, void *data2);
static int _eos_cb(void *data, int type, void *event);
static int _show_mediaplayer_cb(void *data);
static void _class_init(int dummy);
static void _class_shutdown(int dummy);
static void _class_show(int dummy);
static void _class_hide(int dummy);
static void _class_event(void *event_info);
static int em_init(Enna_Module *em);
static int em_shutdown(Enna_Module *em);

typedef struct _Enna_Module_Video Enna_Module_Video;

typedef enum _VIDEO_STATE VIDEO_STATE;

enum _VIDEO_STATE
{
    MENU_VIEW,
    BROWSER_VIEW,
    VIDEOPLAYER_VIEW,
    VIDEO_INFO_VIEW,
};

struct _Enna_Module_Video
{
    Evas *e;
    Evas_Object *o_edje;
    Evas_Object *o_list;
    Evas_Object *o_browser;
    Evas_Object *o_location;
    Evas_Object *o_switcher;
    Evas_Object *o_mediaplayer;
    Evas_Object *o_mediaplayer_old;
    Enna_Module *em;
    VIDEO_STATE state;
    Ecore_Timer *timer_show_mediaplayer;
    Ecore_Event_Handler *eos_event_handler;
    Enna_Playlist *enna_playlist;
};

static Enna_Module_Video *mod;

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_ACTIVITY,
    "activity_video"
};

static Enna_Class_Activity class =
{
    "video",
    1,
    "video",
    NULL,
    "icon/video",
    {
	_class_init,
	_class_shutdown,
	_class_show,
	_class_hide,
	_class_event
    },
    NULL
};

static void
_class_init(int dummy)
{
    _create_gui();
    enna_content_append("video", mod->o_edje);
}

static void
_class_shutdown(int dummy)
{
}

static void
_class_show(int dummy)
{
    edje_object_signal_emit(mod->o_edje, "module,show", "enna");
    switch (mod->state)
    {
    case BROWSER_VIEW:
    case MENU_VIEW:
	edje_object_signal_emit(mod->o_edje, "content,show", "enna");
	edje_object_signal_emit(mod->o_edje, "mediaplayer,hide", "enna");
	break;
    case VIDEO_INFO_VIEW:
	edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
	edje_object_signal_emit(mod->o_edje, "content,hide", "enna");
	break;
    case VIDEOPLAYER_VIEW:
	break;
    default:
	enna_log(ENNA_MSG_ERROR, ENNA_MODULE_NAME, "State Unknown in video module");
    }

}

static void
_class_hide(int dummy)
{
    edje_object_signal_emit(mod->o_edje, "module,hide", "enna");
}

static void
_class_event(void *event_info)
{
    Evas_Event_Key_Down *ev = event_info;
    enna_key_t key = enna_get_key(ev);

    switch (mod->state)
    {
    case MENU_VIEW:
	switch (key)
	{
	case ENNA_KEY_LEFT:
	case ENNA_KEY_CANCEL:
	    enna_content_hide();
	    enna_mainmenu_show(enna->o_mainmenu);
	    break;
	case ENNA_KEY_RIGHT:
	case ENNA_KEY_OK:
	case ENNA_KEY_SPACE:
	    _browse(enna_list_selected_data_get(mod->o_list), NULL);
	    break;
	default:
                    enna_list_event_key_down(mod->o_list, event_info);
	}
	break;
    case BROWSER_VIEW:
	if (mod->o_mediaplayer)
	{
	    ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
	    mod->timer_show_mediaplayer = ecore_timer_add(10,_show_mediaplayer_cb, NULL);
	}
	enna_browser_event_feed(mod->o_browser, event_info);
	break;
    case VIDEO_INFO_VIEW:
	switch (key)
	{
	case ENNA_KEY_CANCEL:
	    mod->state = BROWSER_VIEW;
	    ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
	    mod->timer_show_mediaplayer = ecore_timer_add(10,
		_show_mediaplayer_cb, NULL);
	    edje_object_signal_emit(mod->o_edje, "mediaplayer,hide",
		"enna");
	    edje_object_signal_emit(mod->o_edje, "content,show", "enna");
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
        _return_to_video_info_gui();
	    break;
	case ENNA_KEY_SPACE:
	    enna_mediaplayer_play(mod->enna_playlist);
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

static int
_show_mediaplayer_cb(void *data)
{

    if (mod->o_mediaplayer)
    {
        mod->state = VIDEO_INFO_VIEW;
        edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
        edje_object_signal_emit(mod->o_edje, "content,hide", "enna");
        ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
        mod->timer_show_mediaplayer = NULL;
    }

    return 0;
}

static void
_seek_video(double value)
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

static void
_browser_root_cb (void *data, Evas_Object *obj, void *event_info)
{
    mod->state = MENU_VIEW;
    evas_object_smart_callback_del(mod->o_browser, "root", _browser_root_cb);
    evas_object_smart_callback_del(mod->o_browser, "selected", _browser_selected_cb);
    evas_object_smart_callback_del(mod->o_browser, "browse_down", _browser_browse_down_cb);
    ENNA_OBJECT_DEL(mod->o_browser);
    mod->o_browser = NULL;
    _create_menu();
    enna_location_remove_nth(mod->o_location,enna_location_count(mod->o_location) - 1);
}

static void
_browser_browse_down_cb (void *data, Evas_Object *obj, void *event_info)
{
    int n;
    char *label ;

    n = enna_location_count(mod->o_location) - 1;
    label = enna_location_label_get_nth(mod->o_location, n);
    enna_browser_select_label(mod->o_browser, label);
    enna_location_remove_nth(mod->o_location, n);
}


static void
_browser_selected_cb (void *data, Evas_Object *obj, void *event_info)
{
    int i = 0;
    Enna_Vfs_File *f;
    Eina_List *l;
    Browser_Selected_File_Data *ev = event_info;

    if (!ev || !ev->file) return;

    if (ev->file->is_directory)
    {
	enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Directory Selected %s\n", ev->file->uri);
	enna_location_append(mod->o_location, ev->file->label, NULL, NULL, NULL, NULL);
    }
    else
    {
	enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME , "File Selected %s\n", ev->file->uri);
	enna_mediaplayer_playlist_clear(mod->enna_playlist);
	/* File selected, create mediaplayer */
	EINA_LIST_FOREACH(ev->files, l, f)
	{
	    if (!f->is_directory)
	    {
		enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Append : %s %s to playlist\n", f->label, f->uri);
		enna_mediaplayer_uri_append(mod->enna_playlist,f->uri, f->label);
		if (!strcmp(f->uri, ev->file->uri))
		{
		    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Select : %s %d in playlist\n", f->uri, i);
		    enna_mediaplayer_select_nth(mod->enna_playlist,i);
		}
		i++;
	    }
	}
	_create_video_info_gui();
    }
    free(ev);
}

static void
_browse(void *data, void *data2)
{
    Enna_Class_Vfs *vfs = data;

    mod->o_browser = enna_browser_add(mod->em->evas);
    evas_object_smart_callback_add(mod->o_browser, "root", _browser_root_cb, NULL);
    evas_object_smart_callback_add(mod->o_browser, "selected", _browser_selected_cb, NULL);
    evas_object_smart_callback_add(mod->o_browser, "browse_down", _browser_browse_down_cb, NULL);
    evas_object_show(mod->o_browser);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.browser", mod->o_browser);
    enna_browser_root_set(mod->o_browser, vfs);
    evas_object_del(mod->o_list);
    mod->o_list = NULL;
    enna_location_append(mod->o_location, vfs->label, NULL, NULL, NULL, NULL);
    mod->state = BROWSER_VIEW;
}

static void
_create_videoplayer_gui()
{
    ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
    edje_object_signal_emit(mod->o_edje, "mediaplayer,hide", "enna");
    enna_mediaplayer_stop();
    enna_mediaplayer_play(mod->enna_playlist);
    enna_smart_player_show_video(mod->o_mediaplayer);
    mod->state = VIDEOPLAYER_VIEW;
}


static void
_switcher_transition_done_cb(void *data, Evas_Object *obj, void *event_info)
{
    ENNA_OBJECT_DEL(mod->o_mediaplayer_old);
    mod->o_mediaplayer_old = NULL;

}

static void
_video_info_prev()
{
    int n;
    n = enna_mediaplayer_selected_get(mod->enna_playlist);

    if (n > 0)
    {
        Enna_Metadata *m;
        Evas_Object *o;
        n--;
        enna_mediaplayer_select_nth(mod->enna_playlist,n);

        ENNA_OBJECT_DEL(mod->o_mediaplayer_old);
        mod->o_mediaplayer_old = NULL;

        o = enna_smart_player_add(mod->em->evas);
        evas_object_show(o);
        m = enna_mediaplayer_metadata_get(mod->enna_playlist);
        enna_metadata_grab (m, ENNA_GRABBER_CAP_AUDIO | ENNA_GRABBER_CAP_VIDEO | ENNA_GRABBER_CAP_COVER);
        enna_smart_player_snapshot_set(o, m);
        enna_smart_player_cover_set(o, m);
        enna_smart_player_metadata_set(o, m);
        enna_metadata_free(m);
        mod->o_mediaplayer_old = mod->o_mediaplayer;
        mod->o_mediaplayer = o;
        enna_switcher_objects_switch(mod->o_switcher, o);
    }


}

static void
_video_info_next()
{
    int n;
    n = enna_mediaplayer_selected_get(mod->enna_playlist);

    if (n < enna_mediaplayer_playlist_count(mod->enna_playlist) - 1)
    {
        Enna_Metadata *m;
        Evas_Object *o;
        n++;

        ENNA_OBJECT_DEL(mod->o_mediaplayer_old);
        mod->o_mediaplayer_old = NULL;

        enna_mediaplayer_select_nth(mod->enna_playlist,n);
        o = enna_smart_player_add(mod->em->evas);
        evas_object_show(o);
        m = enna_mediaplayer_metadata_get(mod->enna_playlist);
        enna_metadata_grab (m, ENNA_GRABBER_CAP_AUDIO | ENNA_GRABBER_CAP_VIDEO | ENNA_GRABBER_CAP_COVER);
        enna_smart_player_snapshot_set(o, m);
        enna_smart_player_cover_set(o, m);
        enna_smart_player_metadata_set(o, m);
        enna_metadata_free(m);
        mod->o_mediaplayer_old = mod->o_mediaplayer;
        mod->o_mediaplayer = o;
        enna_switcher_objects_switch(mod->o_switcher, o);
    }
}

static void
_create_video_info_gui()
{
    Evas_Object *o;
    Enna_Metadata *m;

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

    m = enna_mediaplayer_metadata_get(mod->enna_playlist);
    enna_log (ENNA_MSG_INFO, NULL, "Metadata get");
    enna_metadata_grab (m, ENNA_GRABBER_CAP_AUDIO | ENNA_GRABBER_CAP_VIDEO | ENNA_GRABBER_CAP_COVER);
    enna_smart_player_snapshot_set(mod->o_mediaplayer, m);
    enna_smart_player_cover_set(mod->o_mediaplayer, m);
    enna_smart_player_metadata_set(mod->o_mediaplayer, m);
    enna_metadata_free(m);

    enna_switcher_objects_switch(mod->o_switcher, mod->o_mediaplayer);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.mediaplayer", mod->o_switcher);

    edje_object_signal_emit(mod->o_edje, "mediaplayer,show", "enna");
    edje_object_signal_emit(mod->o_edje, "content,hide", "enna");

    mod->state = VIDEO_INFO_VIEW;

}

static void
_return_to_video_info_gui()
{
    enna_mediaplayer_stop();
    enna_smart_player_hide_video(mod->o_mediaplayer);
    mod->state = VIDEO_INFO_VIEW;
    edje_object_signal_emit(mod->o_edje, "mediaplayer,show",
    "enna");
}

static int
_eos_cb(void *data, int type, void *event)
{
    _return_to_video_info_gui();
    return 1;
}

static void
_create_menu()
{
    Evas_Object *o;
    Eina_List *l, *categories;

    /* Create List */
    o = enna_list_add(mod->em->evas);
    edje_object_signal_emit(mod->o_edje, "list,right,now", "enna");

    categories = enna_vfs_get(ENNA_CAPS_VIDEO);
    for (l = categories; l; l = l->next)
    {
        Evas_Object *item;
        Enna_Class_Vfs *cat;
	Evas_Object *icon;

        cat = l->data;
        icon = edje_object_add(mod->em->evas);
        edje_object_file_set(icon, enna_config_theme_get(), cat->icon);
        item = enna_listitem_add(mod->em->evas);
        enna_listitem_create_simple(item, icon, cat->label);
        enna_list_append(o, item, _browse, NULL, cat, NULL);
    }

    enna_list_selected_set(o, 0);
    mod->o_list = o;
    edje_object_part_swallow(mod->o_edje, "enna.swallow.list", o);
    edje_object_signal_emit(mod->o_edje, "list,default", "enna");
}

static void
_create_gui()
{
    Evas_Object *o;
    Evas_Object *icon;

    mod->state = MENU_VIEW;
    o = edje_object_add(mod->em->evas);
    edje_object_file_set(o, enna_config_theme_get(), "module/music_video");
    mod->o_edje = o;
    _create_menu();
    /* Create Location bar */
    o = enna_location_add(mod->em->evas);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.location", o);
    icon = edje_object_add(mod->em->evas);
    edje_object_file_set(icon, enna_config_theme_get(), "icon/video_mini");
    enna_location_append(o, "Video", icon, NULL, NULL, NULL);
    mod->o_location = o;
}

/* Module interface */

static int
em_init(Enna_Module *em)
{
    mod = calloc(1, sizeof(Enna_Module_Video));
    mod->em = em;
    em->mod = mod;

    enna_activity_add(&class);
    mod->enna_playlist = enna_mediaplayer_playlist_create();
    return 1;
}

static int
em_shutdown(Enna_Module *em)
{
    ENNA_OBJECT_DEL(mod->o_edje);
    ENNA_OBJECT_DEL(mod->o_list);
    evas_object_smart_callback_del(mod->o_browser, "root", _browser_root_cb);
    evas_object_smart_callback_del(mod->o_browser, "selected", _browser_selected_cb);
    evas_object_smart_callback_del(mod->o_browser, "browse_down", _browser_browse_down_cb);
    ENNA_OBJECT_DEL(mod->o_browser);
    ENNA_OBJECT_DEL(mod->o_location);
    ENNA_TIMER_DEL(mod->timer_show_mediaplayer);
    ENNA_OBJECT_DEL(mod->o_mediaplayer);
    enna_mediaplayer_playlist_free(mod->enna_playlist);
    free(mod);
    return 1;
}

void
module_init(Enna_Module *em)
{
    if (!em)
        return;

    if (!em_init(em))
        return;
}

void
module_shutdown(Enna_Module *em)
{
    em_shutdown(em);
}
