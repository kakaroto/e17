#include "iconbar.h"
#include "icon.h"
#include "util.h"
#include "prefs.h"
#include <limits.h>
/* #include "icon_editor.h" */

void iconbar_icon_move(Icon *icon);
void iconbar_icons_fix(Iconbar *ib);
void iconbar_icons_load(Iconbar *ib);
void write_out_order(void *data);

static int clock_timer(void *data);
static void cb_iconbar(void *data, Evas_Object *o, const char *sig, const char *src);
static void cb_window(void *data, Evas_Object *o, const char *sig, const char *src);

Evas_Object *
iconbar_new(Evas *evas)
{
  Evas_Object *iconbar;

  iconbar = evas_object_smart_add(evas, iconbar_smart_get());
  
  return iconbar;
}

Evas_Object *
iconbar_gui_get(Evas_Object *o)
{
    Iconbar *ib = NULL;

    if((ib = evas_object_smart_data_get(o)))
    {
	return(ib->gui);
    }
    return(NULL);
}

/* set the path for data (bits, order, icons) */
void
iconbar_path_set(Evas_Object *obj, char *path)
{
  char buf[2048];
  const char *layout = NULL;
  Iconbar *ib = evas_object_smart_data_get(obj);

  if(ib->path) free(ib->path);
  ib->path = (char *)strdup(path);

  snprintf(buf, sizeof(buf) - 1, "%s/iconbar.eet", ib->path);
  if((ib->gui = edje_object_add(evas_object_evas_get(ib->obj))))
  {
    if(!edje_object_file_set(ib->gui, buf, "iconbar"))
    {
	evas_object_del(ib->gui);
	ib->gui = NULL;
	if(ib->path) free(ib->path);
	return;
    }
  }
  else
  {
    printf("Bad Edje File Supplied!\n");
    return;
  }

  evas_object_move(ib->cont, 10, 10);
  evas_object_resize(ib->cont, 20, 200);
  evas_object_show(ib->cont);
  if((layout = edje_file_data_get(buf, "container_layout")))
    esmart_container_layout_plugin_set(ib->cont, layout);
  edje_object_part_swallow(ib->gui, "icons", ib->cont); //was clip
  esmart_container_callback_order_change_set(ib->cont, write_out_order, ib);

  edje_object_signal_callback_add(ib->gui, "mouse,*", "*", cb_iconbar, ib);
  edje_object_signal_callback_add(ib->gui, "scroll,*", "*", cb_iconbar, ib);
  edje_object_signal_callback_add(ib->gui, "window,*", "*", cb_window, ib);
  edje_object_signal_callback_add(ib->gui, "exec*", "*", cb_exec, ib);


  snprintf(buf, PATH_MAX, "%s/fonts", path);
  evas_font_path_append(evas_object_evas_get(obj), buf);
  iconbar_icons_load(ib);
  evas_object_layer_set(ib->cont, 100);
  evas_object_show(ib->cont);
	
  ib->clock_timer = ecore_timer_add(0.25, clock_timer, ib);
}

char *
iconbar_path_get(Evas_Object *obj)
{
  Iconbar *ib = evas_object_smart_data_get(obj);
  if (ib)
    return ib->path;
  else
    return NULL;
}


void
iconbar_icons_load(Iconbar *ib)
{
  DIR *dirp;
  struct dirent *dp;
  char dir[PATH_MAX];
  Evas_List *icons = NULL, *new = NULL;

  snprintf(dir, sizeof(dir), "%s/icons", ib->path);
  dirp = opendir(dir);

  /* create a list of all icons in "icons" directory */
  if (dirp)
  {
    while ((dp = readdir(dirp)))
    {
      Icon *ic;
      char buf[PATH_MAX];

      if (dp->d_name[0] == '.') continue;
      snprintf(buf, sizeof(buf), "%s/%s", dir, dp->d_name);

      ic = iconbar_icon_new(ib, buf);
      icons = evas_list_append(icons, ic);
    }
    closedir(dirp);
  }

  /* add them to the container as specified in config.db */
  {
    Evas_List *l, *ll;
    char buf[PATH_MAX];
    
    for(ll = iconbar_config_icons_get(); ll; ll = ll->next)
    {
        for (l = icons; l; l = evas_list_next(l))
        {
          Icon *ic;
          char *p;
	    
	  snprintf(buf, PATH_MAX, "%s", (char*)ll->data);
          ic = evas_list_data(l);
          if (!strcmp(ic->file, (char*)ll->data))
          {
            new = evas_list_append(new, ic);
          }
	  else if((p = strrchr(ic->file, '/')))
	  {
            p++;
	    if (!strcmp(p, buf))
		new = evas_list_append(new, ic);
          }
	}
    }


    /* add the ones not mentioned to the end */
    for (l = icons; l; l = evas_list_next(l))
    {
      if (!evas_list_find(new, evas_list_data(l)))
      {
        new = evas_list_append(new, evas_list_data(l));
        
      }
    }

    for (l = new; l; l = evas_list_next(l))
    {
      Icon *ic = evas_list_data(l);

      printf("append icon: %s\n", ic->file);
      esmart_container_element_append(ib->cont, ic->image);
    }

    evas_list_free(icons);
    evas_list_free(new);
  }

  write_out_order(ib);
}

void
write_out_order(void *data)
{
  Iconbar *ib = NULL;
  Evas_List *l = NULL, *ll = NULL;
    
  if((ib = (Iconbar*)data))
  {
	for (l = esmart_container_elements_get(ib->cont); l; l = l->next)
	{
	    Evas_Object *obj = l->data;
	    Icon *ic = evas_object_data_get(obj, "Icon");
	    char *p;

	    p = strrchr(ic->file, '/');
	    if (p)
	    {
		p++;
		ll = evas_list_append(ll, strdup(p));
	    }
	}
	iconbar_config_icons_set(ll);
  }
}


/*********************** callbacks ***************************/
static void
cb_iconbar(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Iconbar *ib;

  ib = (Iconbar *)data;
  if (!strcmp(sig, "scroll,1,start"))
  {
      esmart_container_scroll_start(ib->cont, -1);
  }
  else if (!strcmp(sig, "scroll,2,start"))
  {
      esmart_container_scroll_start(ib->cont, 1);
  }
  else if (!strcmp(sig, "scroll,1,stop"))
  {
      esmart_container_scroll_stop(ib->cont);
  }
  else if (!strcmp(sig, "scroll,2,stop"))
  {
      esmart_container_scroll_stop(ib->cont);
  }
  if (!strcmp(sig, "mouse,clicked,1"))
  {
    if (!strcmp(src, "base"))
    {
      int layer = evas_object_layer_get(ib->gui);
      evas_object_layer_set(ib->gui, layer - 1);
      evas_object_layer_set(ib->gui, layer);

    }
  }
}

static void
cb_window(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Iconbar *ib;

  ib = (Iconbar *)data;
  if (!strcmp(sig, "window,raise"))
  {
      iconbar_config_raise_lower_set(1);
  }
  else if (!strcmp(sig, "window,lower"))
  {
      iconbar_config_raise_lower_set(0);
  }
  else if (!strcmp(sig, "window,sticky,on"))
  {
      iconbar_config_sticky_set(1);
  }
  else if (!strcmp(sig, "window,sticky,off"))
  {
      iconbar_config_sticky_set(0);
  }
  else if (!strcmp(sig, "window,withdrawn,on"))
  {
      iconbar_config_withdrawn_set(1);
  }
  else if (!strcmp(sig, "window,withdrawn,off"))
  {
      iconbar_config_withdrawn_set(0);
  }
  else if (!strcmp(sig, "window,borderless,on"))
  {
      iconbar_config_borderless_set(1);
  }
  else if (!strcmp(sig, "window,borderless,off"))
  {
      iconbar_config_borderless_set(0);
  }
}


static int
clock_timer(void *data)
{
    char buf[PATH_MAX];
    Iconbar *ib = NULL; 
    struct tm *_tm = NULL;
    time_t _time = time(NULL);

    if((ib = (Iconbar*)data))
    {
      _tm = localtime(&_time);
      strftime(buf, PATH_MAX, iconbar_config_time_format_get(), _tm);
      edje_object_part_text_set(ib->gui, "clock", buf);
      return(1);
    }
    return(0);
}

/*
 * Make sure the container, icons, etc have the proper stacking 
 * This is a bit hackish, and probably due to a container bug
 */
void
iconbar_fix(Evas_Object *obj)
{
  Evas_Object *gui = iconbar_gui_get(obj);
  int layer = evas_object_layer_get(gui);

  evas_object_layer_set(gui, layer - 1);
  evas_object_layer_set(gui, layer);
}
