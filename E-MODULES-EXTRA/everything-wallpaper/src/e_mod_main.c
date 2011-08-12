#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"

#define IMPORT_STRETCH 0
#define IMPORT_TILE 1
#define IMPORT_CENTER 2
#define IMPORT_SCALE_ASPECT_IN 3
#define IMPORT_SCALE_ASPECT_OUT 4

typedef struct _Import Import;

struct _Import
{
  const char *file;
  int method;
  int external;
  int quality;

  Ecore_Exe *exe;
  Ecore_Event_Handler *exe_handler;
  char *tmpf;
  char *fdest;
};

static void _import_edj_gen(Import *import);
static Eina_Bool _import_cb_edje_cc_exit(void *data, int type, void *event);

static const Evry_API *evry = NULL;
static Evry_Module *evry_module = NULL;

static Import *import = NULL;
static Evry_Action *_act1 = NULL;
static Evry_Action *_act2 = NULL;
static char _module_icon[] = "preferences-desktop-wallpaper";

static int
_action(Evry_Action *act)
{
   if (!CHECK_TYPE(act->it1.item, EVRY_TYPE_FILE))
     return 0;

   GET_FILE(file, act->it1.item);

   if (!(evry->file_path_get(file)))
     return 0;

   if (import)
     {
	if (import->exe_handler)
	  ecore_event_handler_del(import->exe_handler);
	E_FREE(import);
     }

   import = E_NEW(Import, 1);
   import->method = EVRY_ITEM_DATA_INT_GET(act);
   import->file = file->path;
   import->quality = 100;
   import->external = 0;
   _import_edj_gen(import);

   return 1;
}

static int
_check(Evry_Action *act, const Evry_Item *it)
{
   GET_FILE(file, it);

   if (file->mime && (!strncmp(file->mime, "image/", 6)))
     return 1;

   return 0;
}

static int
_check_wallpaper(Evry_Action *act, const Evry_Item *it)
{
   char *suffix;

   if ((suffix = strrchr(it->label, '.')))
     {
	if (!strncmp(suffix, ".edj", 4))
	  return 1;
     }

   return 0;
}

static int
_act_wallpaper(Evry_Action *act)
{
   GET_FILE(file, act->it1.item);

   if (evry->file_path_get(file))
     {
	/* all desktops */
	while (e_config->desktop_backgrounds)
	  {
	     E_Config_Desktop_Background *cfbg;

	     cfbg = e_config->desktop_backgrounds->data;
	     e_bg_del(cfbg->container, cfbg->zone, cfbg->desk_x, cfbg->desk_y);
	  }

	e_bg_default_set(file->path);

	e_bg_update();
	e_config_save_queue();

	return 1;
     }

   return 0;
}

static void
_item_add(Evry_Item *it, const char *name, int method, const char *icon)
{
   Evry_Action *act;
   act = EVRY_ACTION_NEW(name, EVRY_TYPE_FILE, 0, icon, _action, NULL);
   EVRY_ITEM_DATA_INT_SET(act, method);

   it->items = eina_list_append(it->items, act);
}

static Eina_List *
_fetch(Evry_Action *act)
{
   Evry_Item *it = (Evry_Item *) act;

   it->items = NULL;

   _item_add(it, _("Stretch"), IMPORT_STRETCH, "enlightenment/wallpaper_stretch");
   _item_add(it, _("Center"),  IMPORT_CENTER,  "enlightenment/wallpaper_center");
   _item_add(it, _("Tile"),    IMPORT_TILE,    "enlightenment/wallpaper_tile");
   _item_add(it, _("Within"),  IMPORT_SCALE_ASPECT_IN,  "enlightenment/wallpaper_scale_aspect_in");
   _item_add(it, _("Fill"),    IMPORT_SCALE_ASPECT_OUT, "enlightenment/wallpaper_stretch");

   return it->items;
}

static int
_plugins_init(const Evry_API *api)
{
   evry = api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   _act1 = EVRY_ACTION_NEW("Set as Wallpaper",
			   EVRY_TYPE_FILE, 0,
			   _module_icon,
			   NULL, _check);
   _act1->fetch = _fetch;
   _act1->remember_context = EINA_TRUE;
   EVRY_ITEM(_act1)->browseable = EINA_TRUE;

   evry->action_register(_act1, 2);

   _act2 = EVRY_ACTION_NEW("Set as Wallpaper",
			   EVRY_TYPE_FILE, 0,
			   _module_icon,
			   _act_wallpaper, _check_wallpaper);
   _act2->remember_context = EINA_TRUE;
   evry->action_register(_act2, 2);

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   EVRY_ACTION_FREE(_act1);
   EVRY_ACTION_FREE(_act2);
}

/* taken from e_int_config_wallpaper_import.c */
static void
_import_edj_gen(Import *import)
{
   Ecore_Evas *ee = ecore_evas_buffer_new(100, 100);
   Evas *evas = ecore_evas_get(ee);
   Evas_Object *img;
   int fd, num = 1;
   int w = 0, h = 0;
   const char *file, *locale;
   char buf[4096], cmd[4096], tmpn[4096], ipart[4096], enc[128];
   char *imgdir = NULL, *fstrip;
   int cr = 255, cg = 255, cb = 255, ca = 255;
   FILE *f;
   size_t len, off;

   file = ecore_file_file_get(import->file);
   fstrip = ecore_file_strip_ext(file);
   if (!fstrip) return;
   len = e_user_dir_snprintf(buf, sizeof(buf), "backgrounds/%s.edj", fstrip);
   if (len >= sizeof(buf)) return;
   off = len - (sizeof(".edj") - 1);
   while (ecore_file_exists(buf))
     {
	snprintf(buf + off, sizeof(buf) - off, "-%d.edj", num);
	num++;
     }
   free(fstrip);
   strcpy(tmpn, "/tmp/e_bgdlg_new.edc-tmp-XXXXXX");
   fd = mkstemp(tmpn);
   if (fd < 0)
     {
	printf("Error Creating tmp file: %s\n", strerror(errno));
	return;
     }
   close(fd);

   f = fopen(tmpn, "w");
   if (!f)
     {
	printf("Cannot open %s for writing\n", tmpn);
	return;
     }

   imgdir = ecore_file_dir_get(import->file);
   if (!imgdir) ipart[0] = '\0';
   else
     {
	snprintf(ipart, sizeof(ipart), "-id %s", e_util_filename_escape(imgdir));
	free(imgdir);
     }

   img = evas_object_image_add(evas);
   evas_object_image_file_set(img, import->file, NULL);
   evas_object_image_size_get(img, &w, &h);
   evas_object_del(img);
   ecore_evas_free(ee);

   printf("w%d h%d\n", w, h);

   if (import->external)
     {
	fstrip = strdup(e_util_filename_escape(import->file));
	snprintf(enc, sizeof(enc), "USER");
     }
   else
     {
	fstrip = strdup(e_util_filename_escape(file));
	if (import->quality == 100)
	  snprintf(enc, sizeof(enc), "COMP");
	else
	  snprintf(enc, sizeof(enc), "LOSSY %i", import->quality);
     }
   switch (import->method)
     {
      case IMPORT_STRETCH:
	 fprintf(f,
		 "images { image: \"%s\" %s; }\n"
		 "collections {\n"
		 "group { name: \"e/desktop/background\";\n"
		 "data { item: \"style\" \"0\"; }\n"
		 "max: %i %i;\n"
		 "parts {\n"
		 "part { name: \"bg\"; mouse_events: 0;\n"
		 "description { state: \"default\" 0.0;\n"
		 "image { normal: \"%s\"; scale_hint: STATIC; }\n"
		 "} } } } }\n"
		 , fstrip, enc, w, h, fstrip);
	 break;
      case IMPORT_TILE:
	 fprintf(f,
		 "images { image: \"%s\" %s; }\n"
		 "collections {\n"
		 "group { name: \"e/desktop/background\";\n"
		 "data { item: \"style\" \"1\"; }\n"
		 "max: %i %i;\n"
		 "parts {\n"
		 "part { name: \"bg\"; mouse_events: 0;\n"
		 "description { state: \"default\" 0.0;\n"
		 "image { normal: \"%s\"; }\n"
		 "fill { size {\n"
		 "relative: 0.0 0.0;\n"
		 "offset: %i %i;\n"
		 "} } } } } } }\n"
		 , fstrip, enc, w, h, fstrip, w, h);
	 break;
      case IMPORT_CENTER:
	 fprintf(f,
		 "images { image: \"%s\" %s; }\n"
		 "collections {\n"
		 "group { name: \"e/desktop/background\";\n"
		 "data { item: \"style\" \"2\"; }\n"
		 "max: %i %i;\n"
		 "parts {\n"
		 "part { name: \"col\"; type: RECT; mouse_events: 0;\n"
		 "description { state: \"default\" 0.0;\n"
		 "color: %i %i %i %i;\n"
		 "} }\n"
		 "part { name: \"bg\"; mouse_events: 0;\n"
		 "description { state: \"default\" 0.0;\n"
		 "min: %i %i; max: %i %i;\n"
		 "image { normal: \"%s\"; }\n"
		 "} } } } }\n"
		 , fstrip, enc, w, h, cr, cg, cb, ca, w, h, w, h, fstrip);
	 break;
      case IMPORT_SCALE_ASPECT_IN:
	 locale = e_intl_language_get();
	 setlocale(LC_NUMERIC, "C");
	 fprintf(f,
		 "images { image: \"%s\" %s; }\n"
		 "collections {\n"
		 "group { name: \"e/desktop/background\";\n"
		 "data { item: \"style\" \"3\"; }\n"
		 "max: %i %i;\n"
		 "parts {\n"
		 "part { name: \"col\"; type: RECT; mouse_events: 0;\n"
		 "description { state: \"default\" 0.0;\n"
		 "color: %i %i %i %i;\n"
		 "} }\n"
		 "part { name: \"bg\"; mouse_events: 0;\n"
		 "description { state: \"default\" 0.0;\n"
		 "aspect: %1.9f %1.9f; aspect_preference: BOTH;\n"
		 "image { normal: \"%s\";  scale_hint: STATIC; }\n"
		 "} } } } }\n"
		 , fstrip, enc, w, h, cr, cg, cb, ca, (double)w / (double)h, (double)w / (double)h, fstrip);
	 setlocale(LC_NUMERIC, locale);
	 break;
      case IMPORT_SCALE_ASPECT_OUT:
	 locale = e_intl_language_get();
	 setlocale(LC_NUMERIC, "C");
	 fprintf(f,
		 "images { image: \"%s\" %s; }\n"
		 "collections {\n"
		 "group { name: \"e/desktop/background\";\n"
		 "data { item: \"style\" \"4\"; }\n"
		 "max: %i %i;\n"
		 "parts {\n"
		 "part { name: \"bg\"; mouse_events: 0;\n"
		 "description { state: \"default\" 0.0;\n"
		 "aspect: %1.9f %1.9f; aspect_preference: NONE;\n"
		 "image { normal: \"%s\";  scale_hint: STATIC; }\n"
		 "} } } } }\n"
		 , fstrip, enc, w, h, (double)w / (double)h, (double)w / (double)h, fstrip);
	 setlocale(LC_NUMERIC, locale);
	 break;
      default:
	 /* won't happen */
	 break;
     }
   free(fstrip);

   fclose(f);

   snprintf(cmd, sizeof(cmd), "edje_cc -v %s %s %s",
	    ipart, tmpn, e_util_filename_escape(buf));

   import->tmpf = strdup(tmpn);
   import->fdest = strdup(buf);
   import->exe_handler =
     ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
			     _import_cb_edje_cc_exit, import);
   import->exe = ecore_exe_run(cmd, NULL);
}

static Eina_Bool
_import_cb_edje_cc_exit(void *data, int type, void *event)
{
   Import *import;
   Ecore_Exe_Event_Del *ev;
   char *fdest;
   int r = 1;

   ev = event;
   import = data;

   if (!ev->exe) return EINA_TRUE;

   if (ev->exe != import->exe) return EINA_TRUE;

   if (ev->exit_code != 0)
     {
	e_util_dialog_show(_("Picture Import Error"),
			   _("Enlightenment was unable to import the picture<br>"
			     "due to conversion errors."));
	r = 0;
     }


   fdest = strdup(import->fdest);
   if (r)
     {
	e_bg_default_set(fdest);
	e_bg_update();
     }

   E_FREE(fdest);

   return EINA_FALSE;
}

/***************************************************************************/

EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, PACKAGE };

EAPI void *
e_modapi_init(E_Module *m)
{
   EVRY_MODULE_NEW(evry_module, evry, _plugins_init, _plugins_shutdown);
   
   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   EVRY_MODULE_FREE(evry_module);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/***************************************************************************/
