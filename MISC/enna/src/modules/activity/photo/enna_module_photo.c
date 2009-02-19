/* Interface */

#include "enna.h"
#ifdef BUILD_LIBEXIF
#include "libexif/exif-data.h"
#endif

#define ENNA_MODULE_NAME "photo"

static void _create_menu();
static void _create_gui();
static void _browser_root_cb (void *data, Evas_Object *obj, void *event_info);
static void _browser_selected_cb (void *data, Evas_Object *obj, void *event_info);
static void _browser_browse_down_cb (void *data, Evas_Object *obj, void *event_info);
static void _browse(void *data, void *data2);

static void _photo_info_fs();

typedef enum _PHOTO_STATE PHOTO_STATE;
enum _PHOTO_STATE
{
    WALL_VIEW,
    WALL_PREVIEW,
    SLIDESHOW_VIEW,
    MENU_VIEW,
    BROWSER_VIEW
};

typedef struct _Enna_Module_Photo
{
    Evas *e;
    Evas_Object *o_edje;
    Evas_Object *o_menu;
    Evas_Object *o_browser;
    Evas_Object *o_wall;
    Evas_Object *o_preview;
    Evas_Object *o_slideshow;
    PHOTO_STATE state;
    Enna_Module *em;
#ifdef BUILD_LIBEXIF
    struct {
	Evas_Object *o_scroll;
	Evas_Object *o_exif;
	char *str;
    }exif;
#endif

} Enna_Module_Photo;

static Enna_Module_Photo *mod;

/*****************************************************************************/
/*                              Photo Helpers                                */
/*****************************************************************************/

static void _photo_info_delete_cb(void *data,
    Evas_Object *obj,
    const char *emission,
    const char *source)
{

    Evas_Object *o_pict;


    edje_object_signal_callback_del(mod->o_preview, "done", "", _photo_info_delete_cb);
    o_pict = edje_object_part_swallow_get(mod->o_preview, "enna.swallow.content");

#ifdef BUILD_LIBEXIF
    ENNA_OBJECT_DEL(mod->exif.o_exif);
    ENNA_OBJECT_DEL(mod->exif.o_scroll);
    free(mod->exif.str);
    mod->exif.str = NULL;
#endif

    ENNA_OBJECT_DEL(o_pict);
    ENNA_OBJECT_DEL(mod->o_preview);
    mod->state = WALL_VIEW;
}


static void _photo_info_delete()
{
    edje_object_signal_callback_add(mod->o_preview, "done","", _photo_info_delete_cb, NULL);
    edje_object_signal_emit(mod->o_preview, "hide", "enna");
    edje_object_signal_emit(mod->o_preview, "hide,exif", "enna");
    edje_object_signal_emit(mod->o_edje, "wall,show", "enna");
}


#ifdef BUILD_LIBEXIF
static void _exif_content_foreach_func(ExifEntry *entry, void *callback_data)
{
  char buf[2000];
  char buf_txtblk[2000];
  char *exif_str;
  size_t len;

  exif_entry_get_value(entry, buf, sizeof(buf));
  snprintf(buf_txtblk, sizeof(buf_txtblk), "<hilight>%s</hilight> : %s<br>",
	 exif_tag_get_name(entry->tag),
	 exif_entry_get_value(entry, buf, sizeof(buf)));

  if (!mod->exif.str)
      len = strlen(buf_txtblk) + 1;
  else
      len = strlen(mod->exif.str) + strlen(buf_txtblk) + 1;

  exif_str = (char*)calloc(len, sizeof(char));
  if (mod->exif.str)
      strcpy(exif_str, mod->exif.str);
  strcat(exif_str, buf_txtblk);
  free(mod->exif.str);
  mod->exif.str = exif_str;

}



static void _exif_data_foreach_func(ExifContent *content, void *callback_data)
{
  exif_content_foreach_entry(content, _exif_content_foreach_func, callback_data);
}


/** Run EXIF parsing test on the given file. */

static void _exif_parse_metadata(const char *filename)
{
  ExifData *d;
  Evas_Coord mw, mh;

  if (!filename) return;

  if (mod->exif.str)
      free(mod->exif.str);

  mod->exif.str = NULL;

  if (mod->exif.o_exif)
      evas_object_del(mod->exif.o_exif);

  mod->exif.o_exif = edje_object_add(mod->em->evas);
  edje_object_file_set(mod->exif.o_exif, enna_config_theme_get(), "exif/data");
  mod->exif.o_scroll = enna_scrollframe_add(mod->em->evas);
  edje_object_part_swallow(mod->o_preview, "enna.swallow.exif", mod->exif.o_scroll);
  d = exif_data_new_from_file(filename);
  exif_data_foreach_content(d, _exif_data_foreach_func, NULL);
  exif_data_unref(d);
  edje_object_part_text_set(mod->exif.o_exif, "enna.text.exif", mod->exif.str);
  edje_object_size_min_calc(mod->exif.o_exif, &mw, &mh);
  evas_object_resize(mod->exif.o_exif, mw, mh);
  evas_object_size_hint_min_set(mod->exif.o_exif, mw, mh);
  enna_scrollframe_child_set(mod->exif.o_scroll, mod->exif.o_exif);
}
#endif

static void _photo_info_fs()
{
    Evas_Object *o_edje;
    Evas_Object *o_pict;
    Evas_Coord x1,y1,w1,h1, x2,y2,w2,h2;
    Evas_Coord xi,yi,wi,hi, xf,yf,wf,hf;
    Edje_Message_Int_Set *msg;
    const char *filename;

    mod->state = WALL_PREVIEW;

    /* Prepare edje message */
    msg = calloc(1,sizeof(Edje_Message_Int_Set) - sizeof(int) + (4 * sizeof(int)));
    msg->count = 4;


    enna_wall_selected_geometry_get(mod->o_wall, &x2, &y2, &w2, &h2);
    filename = enna_wall_selected_filename_get(mod->o_wall);
    if (!filename) return;

    o_pict = enna_image_add(mod->em->evas);
    enna_image_file_set(o_pict, filename);
    enna_image_fill_inside_set(o_pict, 0);

    enna_image_preload(o_pict, 0);

    o_edje = edje_object_add(mod->em->evas);
    edje_object_file_set(o_edje, enna_config_theme_get(), "enna/picture/info");
    edje_object_part_swallow(o_edje, "enna.swallow.content", o_pict);

    /* Set Final state in fullscreen */
    edje_object_part_swallow(mod->o_edje, "enna.swallow.slideshow", o_edje);
    evas_object_geometry_get(mod->o_edje, &x1, &y1, &w1, &h1);
    hf = h1;
    wf = hf * (float)w2 / (float)h2;
    xf = w1 / 2 - wf / 2;
    yf = h1 / 2 - hf / 2;

    msg->val[0] = xf;
    msg->val[1] = yf;
    msg->val[2] = wf;
    msg->val[3] = hf;
    enna_image_load_size_set(o_pict, wf, hf);
    edje_object_message_send(o_edje, EDJE_MESSAGE_INT_SET, 2, msg);

    /* Set custom state : size and position of actual thumbnail */
    xi = x2 - x1;
    yi = y2 - y1;
    wi = w2;
    hi = h2;
    msg->val[0] = xi;
    msg->val[1] = yi;
    msg->val[2] = wi;
    msg->val[3] = hi;
    edje_object_message_send(o_edje, EDJE_MESSAGE_INT_SET, 1, msg);
    free(msg);

    mod->o_preview = o_edje;
#ifdef BUILD_LIBEXIF
    _exif_parse_metadata(filename);
#endif
    edje_object_signal_emit(mod->o_preview, "show", "enna");
    edje_object_signal_emit(mod->o_edje, "wall,hide", "enna");
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

static void
_picture_selected_cb (void *data, Evas_Object *obj, void *event_info)
{
    printf("_photo_info_fs\n");
    _photo_info_fs();
}


static void
_browser_root_cb (void *data, Evas_Object *obj, void *event_info)
{
    mod->state = MENU_VIEW;
    evas_object_smart_callback_del(mod->o_browser, "root", _browser_root_cb);
    evas_object_smart_callback_del(mod->o_browser, "selected", _browser_selected_cb);
    evas_object_smart_callback_del(mod->o_browser, "browse_down", _browser_browse_down_cb);

    /* Delete objects */
    ENNA_OBJECT_DEL(mod->o_browser);
    evas_object_smart_callback_del(mod->o_wall, "selected", _picture_selected_cb);
    ENNA_OBJECT_DEL(mod->o_wall);
    edje_object_signal_emit(mod->o_edje, "wall,hide", "enna");
    edje_object_signal_emit(mod->o_edje, "browser,hide", "enna");

    mod->o_browser = NULL;
    _create_menu();
}

static void
_browser_browse_down_cb (void *data, Evas_Object *obj, void *event_info)
{
    evas_object_smart_callback_del(mod->o_wall, "selected", _picture_selected_cb);
    ENNA_OBJECT_DEL(mod->o_wall);
}

static void
_browser_selected_cb (void *data, Evas_Object *obj, void *event_info)
{
    Enna_Vfs_File *f;
    Eina_List *l;
    Browser_Selected_File_Data *ev = event_info;
    int count = 0;

    if (!ev || !ev->file) return;

    if (ev->file->is_directory)
    {
	ENNA_OBJECT_DEL(mod->o_wall);
	mod->o_wall = enna_wall_add(mod->em->evas);
	evas_object_smart_callback_add(mod->o_wall, "selected", _picture_selected_cb, NULL);

	evas_object_show(mod->o_wall);

	EINA_LIST_FOREACH(ev->files, l, f)
	{
	    if (!ecore_file_is_dir(f->uri + 7))
	    {
		enna_wall_picture_append(mod->o_wall, f->uri);
	    }
	    else
	    {
		count++;
	    }
	}
	edje_object_part_swallow(mod->o_edje, "enna.swallow.wall", mod->o_wall);
	edje_object_signal_emit(mod->o_edje, "wall,show", "enna");
	enna_wall_select_nth(mod->o_wall, 0, 0);

	if (!count)
	{
	    edje_object_signal_emit(mod->o_edje, "browser,hide", "enna");
	    mod->state = WALL_VIEW;
	}

    }
    else
    {


    }

    free(ev);
}

static void _browse(void *data, void *data2)
{
    Enna_Class_Vfs *vfs = data;

    mod->o_browser = enna_browser_add(mod->em->evas);
    enna_browser_show_file_set(mod->o_browser, 0);
    evas_object_smart_callback_add(mod->o_browser, "root", _browser_root_cb, NULL);
    evas_object_smart_callback_add(mod->o_browser, "selected", _browser_selected_cb, NULL);
    evas_object_smart_callback_add(mod->o_browser, "browse_down", _browser_browse_down_cb, NULL);


    mod->state = BROWSER_VIEW;

    evas_object_show(mod->o_browser);
    edje_object_part_swallow(mod->o_edje, "enna.swallow.browser", mod->o_browser);
    enna_browser_root_set(mod->o_browser, vfs);

    edje_object_signal_emit(mod->o_edje, "menu,hide", "enna");
    edje_object_signal_emit(mod->o_edje, "browser,show", "enna");
    edje_object_signal_emit(mod->o_edje, "wall,show", "enna");

    evas_object_del(mod->o_menu);
    mod->o_menu = NULL;
}

static void
_create_menu()
{
    Evas_Object *o;
    Eina_List *l, *categories;


    /* Create List */
    o = enna_list_add(mod->em->evas);
    edje_object_signal_emit(mod->o_edje, "menu,show", "enna");

    categories = enna_vfs_get(ENNA_CAPS_PHOTO);
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
    mod->o_menu = o;
    edje_object_part_swallow(mod->o_edje, "enna.swallow.menu", o);
    edje_object_signal_emit(mod->o_edje, "menu,show", "enna");
}

static void _create_gui(void)
{

    /* Set default state */
    mod->state = MENU_VIEW;

    /* Create main edje object */
    mod->o_edje = edje_object_add(mod->em->evas);
    edje_object_file_set(mod->o_edje, enna_config_theme_get(), "module/photo");

    _create_menu();

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
}

static void _class_hide(int dummy)
{
    edje_object_signal_emit(mod->o_edje, "module,hide", "enna");
}

static void _class_event(void *event_info)
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
	    _browse(enna_list_selected_data_get(mod->o_menu), NULL);
	    break;
	default:
	    enna_list_event_key_down(mod->o_menu, event_info);
	}
	break;
    case BROWSER_VIEW:
	switch (key)
	{
	case ENNA_KEY_RIGHT:
	case ENNA_KEY_LEFT:
	    mod->state = WALL_VIEW;
	    edje_object_signal_emit(mod->o_edje, "browser,hide", "enna");
	    break;
	default:
	    enna_browser_event_feed(mod->o_browser, event_info);
	}
	break;
    case WALL_VIEW:
	switch (key)
	{
	case ENNA_KEY_CANCEL:

	    edje_object_signal_emit(mod->o_edje, "browser,show", "enna");
	    mod->state = BROWSER_VIEW;
	    break;

	case ENNA_KEY_OK:
	case ENNA_KEY_SPACE:
	    _photo_info_fs();
	    break;
	case ENNA_KEY_RIGHT:
	case ENNA_KEY_LEFT:
	case ENNA_KEY_UP:
	case ENNA_KEY_DOWN:
	    enna_wall_event_feed(mod->o_wall, ev);
	default:
	    break;

	}
	break;
    case WALL_PREVIEW:
	switch (key)
	{
	case ENNA_KEY_CANCEL:
	    _photo_info_delete();
	    break;
#ifdef BUILD_LIBEXIF
	case ENNA_KEY_UP:
	    edje_object_signal_emit(mod->o_preview, "show,exif", "enna");
	    break;
	case ENNA_KEY_DOWN:
	    edje_object_signal_emit(mod->o_preview, "hide,exif", "enna");
	    break;
#endif
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
    ENNA_MODULE_ACTIVITY,
    "activity_photo"
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
    ENNA_OBJECT_DEL(mod->o_wall);
    ENNA_OBJECT_DEL(mod->o_menu);
    ENNA_OBJECT_DEL(mod->o_browser);
    free(mod);
}
