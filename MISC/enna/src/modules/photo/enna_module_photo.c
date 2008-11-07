/* Interface */

#include "enna.h"

#define ENNA_MODULE_NAME "photo"

static void _create_gui();
static void _list_transition_core(Evas_List *files, unsigned char direction);
static void _browse(void *data, void *data2);
static void _browse_down();
static void _activate();

typedef enum _PHOTO_STATE PHOTO_STATE;
enum _PHOTO_STATE
{
    WALL_VIEW,
    SLIDESHOW_VIEW,
    DEFAULT_VIEW,
};

typedef struct _Enna_Module_Photo
{
    Evas *e;
    Evas_Object *o_edje;
    Evas_Object *o_list;
    Evas_Object *o_wall;
    Evas_Object *o_location;
    Evas_Object *o_slideshow;
    PHOTO_STATE state;
    Enna_Class_Vfs *vfs;
    Enna_Module *em;
    char *prev_selected;
    unsigned char is_root : 1;
    unsigned char list_selected : 1;
    unsigned char no_dir : 1;
} Enna_Module_Photo;

static Enna_Module_Photo *mod;

/*****************************************************************************/
/*                              Photo Helpers                                */
/*****************************************************************************/

static void _activate()
{
    Enna_Vfs_File *f;
    Enna_Class_Vfs *vfs;

    vfs = (Enna_Class_Vfs*)enna_list_selected_data_get(mod->o_list);
    f = (Enna_Vfs_File*)enna_list_selected_data2_get(mod->o_list);
    _browse(vfs, f);

}

static void _list_transition_core(Eina_List *files, unsigned char direction)
{
    Evas_Object *o_wall, *o_list, *oe;
    Eina_List *l;

    o_wall = mod->o_wall;
    o_list = mod->o_list;

    evas_object_del(o_wall);
    evas_object_del(o_list);

    o_list = enna_list_add(mod->em->evas);
    oe = enna_list_edje_object_get(o_list);
    evas_object_show(o_list);



    if (direction == 0)
        edje_object_signal_emit(oe, "list,right,now", "enna");
    else
        edje_object_signal_emit(oe, "list,left,now", "enna");

    o_wall = enna_wall_add(mod->em->evas);
    evas_object_show(o_wall);



    if (eina_list_count(files))
    {
        int i = 0;
	int nb_dir = 0;
        mod->is_root = 0;
        /* Create list of files */
        for (l = files, i = 0; l; l = l->next, i++)
        {
	    Enna_Vfs_File *f;
            Evas_Object *icon;
            Evas_Object *item;

            f = l->data;
	    if (f->is_directory)
	    {
		nb_dir++;
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
		enna_list_append(o_list, item, _browse, NULL, mod->vfs, f);
	    }
	    else
	    {
		enna_wall_picture_append(o_wall, f->uri);
	    }
	}
	if (!nb_dir)
	{
	    mod->list_selected = 0;
	    mod->no_dir = 1;
	    edje_object_signal_emit(mod->o_edje, "list,hide", "enna");

	}
	else
	{
	    mod->list_selected = 1;
	    mod->no_dir = 0;
	    edje_object_signal_emit(mod->o_edje, "list,show", "enna");
	    edje_object_part_swallow(mod->o_edje, "enna.swallow.list", o_list);
	}
	edje_object_part_swallow(mod->o_edje, "enna.swallow.wall", o_wall);
	enna_wall_select_nth(o_wall, 0, 0);

    }
    else if (!direction)
    {
	Evas_Object *icon;
	Evas_Object *item;
        /* No files returned : create no media item */
	enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "No file found");
	mod->list_selected = 1;
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
        categories = enna_vfs_get(ENNA_CAPS_PHOTO);
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
	edje_object_part_swallow(mod->o_edje, "enna.swallow.list", o_list);
	edje_object_part_swallow(mod->o_edje, "enna.swallow.wall", o_wall);
        mod->vfs = NULL;
	mod->list_selected = 1;
    }

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

    mod->o_wall = o_wall;
    mod->o_list = o_list;
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

static void _create_slideshow_gui()
{
    Evas_Object *o;

    mod->state = SLIDESHOW_VIEW;

    if (mod->o_slideshow)
        evas_object_del(mod->o_slideshow);

    o = enna_slideshow_add(mod->em->evas);
    edje_object_part_swallow(enna->o_edje, "enna.swallow.fullscreen", o);
    evas_object_show(o);
    mod->o_slideshow = o;

    //edje_object_signal_emit(mod->o_edje, "slideshow,show", "enna");
    edje_object_signal_emit(mod->o_edje, "list,hide", "enna");
    edje_object_signal_emit(mod->o_edje, "wall,hide", "enna");

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
	Evas_Object *oe;

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

            _create_slideshow_gui();
            for (l = files; l; l = l->next)
            {
                Enna_Vfs_File *f;
                f = l->data;

                if (!f->is_directory)
                {
                    enna_slideshow_image_append(mod->o_slideshow, f->uri);
                    i++;
                }
            }
            enna_slideshow_play(mod->o_slideshow);
            return;
        }

        mod->vfs = vfs;

        /* Clear list and add new items */
        oe = enna_list_edje_object_get(mod->o_list);
        edje_object_signal_callback_add(oe, "list,transition,end", "edje",
                _list_transition_left_end_cb, files);
        edje_object_signal_emit(oe, "list,left", "enna");

    }

}

static void _create_gui(void)
{

    Evas_Object *o;
    Eina_List *l, *categories;
    Evas_Object *icon;

    o = edje_object_add(mod->em->evas);
    edje_object_file_set(o, enna_config_theme_get(), "module/photo");
    mod->o_edje = o;
    mod->prev_selected = NULL;
    mod->is_root = 1;
    mod->list_selected = 1;
    mod->no_dir = 0;
    mod->state = WALL_VIEW;
    /* Create List */
    o = enna_list_add(mod->em->evas);
    categories = enna_vfs_get(ENNA_CAPS_PHOTO);
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

    enna_list_selected_set(o, 0);

    mod->vfs = NULL;
    mod->o_list = o;

    o = enna_wall_add(mod->em->evas);
    evas_object_show(o);

    mod->o_wall = o;
    edje_object_part_swallow(mod->o_edje, "enna.swallow.wall", mod->o_wall);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.list", mod->o_list);


    /* Create Location bar */
    o = enna_location_add(mod->em->evas);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.location", o);

    icon = edje_object_add(mod->em->evas);
    edje_object_file_set(icon, enna_config_theme_get(), "icon/photo_mini");
    enna_location_append(o, "Photo", icon, NULL, NULL, NULL);
    mod->o_location = o;
}

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static void _class_init(int dummy)
{
    _create_gui();
    enna_content_append("photo", mod->o_edje);
}

static void _class_shutdown(int dummy)
{

}

static void _class_show(int dummy)
{
    edje_object_signal_emit(mod->o_edje, "module,show", "enna");
    edje_object_signal_emit(mod->o_edje, "list,show", "enna");
    edje_object_signal_emit(mod->o_edje, "wall,show", "enna");

    edje_object_signal_emit(mod->o_edje, "slideshow,hide", "enna");
}

static void _class_hide(int dummy)
{
    edje_object_signal_emit(mod->o_edje, "module,hide", "enna");
    edje_object_signal_emit(mod->o_edje, "list,hide", "enna");
    edje_object_signal_emit(mod->o_edje, "wall,hide", "enna");
}

static void _class_event(void *event_info)
{
    Evas_Event_Key_Down *ev = event_info;
    enna_key_t key = enna_get_key(ev);

    switch (mod->state)
    {
        case WALL_VIEW:
            switch (key)
            {
                case ENNA_KEY_CANCEL:
		    if (mod->is_root)
                    {
                        enna_content_hide();
                        enna_mainmenu_show(enna->o_mainmenu);
                    }
		    else if (mod->list_selected || mod->no_dir)
		    {
			_browse_down();
			mod->list_selected = 1;
		    }
		    else
		    {
			mod->list_selected = 1;
			edje_object_signal_emit(mod->o_edje, "list,show", "enna");
		    }
                    break;

                case ENNA_KEY_OK:
	        case ENNA_KEY_SPACE:
		    if (mod->list_selected)
			_activate();
                    break;
                case ENNA_KEY_RIGHT:
		    if (mod->list_selected)
		    {
			mod->list_selected = 0;
			edje_object_signal_emit(mod->o_edje, "list,hide", "enna");
		    }
		    enna_wall_right_select(mod->o_wall);
		    break;
                case ENNA_KEY_LEFT:
		    if (mod->list_selected)
		    {
			mod->list_selected = 0;
			edje_object_signal_emit(mod->o_edje, "list,hide", "enna");
		    }
		    enna_wall_left_select(mod->o_wall);
		    break;
                case ENNA_KEY_UP:
		    if (mod->list_selected)
			enna_list_event_key_down(mod->o_list, event_info);
		    else
			enna_wall_up_select(mod->o_wall);

		    break;
                case ENNA_KEY_DOWN:
		    if (mod->list_selected)
			enna_list_event_key_down(mod->o_list, event_info);
		    else
			enna_wall_down_select(mod->o_wall);
		    break;
                default:
		    break;

            }
            break;
        case SLIDESHOW_VIEW:
            switch (key)
            {
                case ENNA_KEY_CANCEL:
                    evas_object_del(mod->o_slideshow);
                    mod->state = WALL_VIEW;
		    edje_object_signal_emit(mod->o_edje, "wall,show", "enna");
                    edje_object_signal_emit(mod->o_edje, "list,show", "enna");
                    break;
                case ENNA_KEY_RIGHT:
                    enna_slideshow_next(mod->o_slideshow);
                    break;
                case ENNA_KEY_LEFT:
                    enna_slideshow_prev(mod->o_slideshow);
                    break;
                case ENNA_KEY_OK:
	            case ENNA_KEY_SPACE:
                    enna_slideshow_play(mod->o_slideshow);

                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

}

static Enna_Class_Activity
        class =
        { "photo", 1, "photo", NULL, "icon/photo",
                { _class_init, _class_shutdown, _class_show, _class_hide,
                        _class_event }, NULL };

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    "photo"
};

void module_init(Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_Photo));
    mod->em = em;
    em->mod = mod;

    enna_activity_add(&class);
}

void module_shutdown(Enna_Module *em)
{
    evas_object_del(mod->o_edje);

    evas_object_del(mod->o_wall);
    evas_object_del(mod->o_location);

    if (mod->vfs && mod->vfs->func.class_shutdown)
        mod->vfs->func.class_shutdown(0);

    if (mod->prev_selected)
        free(mod->prev_selected);

    free(mod);
}
