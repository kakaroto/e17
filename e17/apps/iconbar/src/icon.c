#include "icon.h"
#include "util.h"
/* #include "icon_editor.h" */

static void cb_icon(void *data, Evas_Object *o, const char *sig, const char *src);
static void cb_edit(void *data, Evas_Object *o, const char *sig, const char *src);

Icon *
iconbar_icon_new(Iconbar *ib, char *path)
{
  Evas *evas;
  Icon *ic;

  evas = evas_object_evas_get(ib->obj);
  ic = (Icon *)malloc(sizeof(Icon));
  memset(ic, 0, sizeof(Icon));

  ic->iconbar = ib;
  ic->file = (char *)strdup(path);
  
  /* load up the icon edje FIXME make sure it loaded... */
  
  ic->image = edje_object_add(evas);
  edje_object_file_set(ic->image, ic->file, "icon");
  evas_object_data_set(ic->image, "Icon", ic);
  evas_object_resize(ic->image, 32, 32);
  evas_object_show(ic->image);

  edje_object_signal_callback_add(ic->image, "exec*", "*", cb_exec, ic);
  edje_object_signal_callback_add(ic->image, "mouse*", "*", cb_icon, ic);
  edje_object_signal_callback_add(ic->image, "edit", "*", cb_edit, ib);
  return ic;
}

void
iconbar_icon_free(Icon *ic)
{
  if (!ic) return;
  evas_object_del(ic->image);
  if (ic->file) free(ic->file);
  free(ic);
}

static void
cb_icon(void *data, Evas_Object *o, const char *sig, const char *src)
{
  /* FIXME put icon dragging stuff in here */
  if (!strcmp(sig, "mouse,down,2"))
  {
  }
  else if (!strcmp(sig, "mouse,up,2"))
  {
  }
  else if (!strcmp(sig, "mouse,move"))
  {
  }
}

void
cb_exec(void *data, Evas_Object *o, const char *sig, const char *src)
{
  char *exec = (char *)(sig+5);
 
  if (!exec_run_in_dir(exec, get_user_home()))
  {
    printf("Error: failed to run \"%s\"\n", exec);
  }
}

static void
cb_edit(void *data, Evas_Object *o, const char *sig, const char *src)
{
/*
  Icon *icon = evas_object_data_get(o, "Icon");
  icon_editor_icon_set(icon);
  icon_editor_show();
*/
}

