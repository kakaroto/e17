#include "iconbar.h"
#include "util.h"
#include <math.h>
#include <time.h>
/* smart object handlers */
void iconbar_add(Evas_Object *o);
void iconbar_del(Evas_Object *o);
void iconbar_layer_set(Evas_Object *o, int l);
void iconbar_raise(Evas_Object *o);
void iconbar_lower(Evas_Object *o);
void iconbar_stack_above(Evas_Object *o, Evas_Object *above);
void iconbar_stack_below(Evas_Object *o, Evas_Object *below);
void iconbar_move(Evas_Object *o, double x, double y);
void iconbar_resize(Evas_Object *o, double w, double h);
void iconbar_show(Evas_Object *o);
void iconbar_hide(Evas_Object *o);
void iconbar_color_set(Evas_Object *o, int r, int g, int b, int a);
void iconbar_clip_set(Evas_Object *o, Evas_Object *clip);
void iconbar_clip_unset(Evas_Object *o);

void iconbar_icon_move(Icon *icon);
void iconbar_icons_fix(Iconbar *ib);
void iconbar_icons_load(Iconbar *ib);
void write_out_order(void *data);


static int clock_timer(void *data);
static void cb_iconbar(void *data, Evas_Object *o, const char *sig, const char *src);
static void cb_icon(void *data, Evas_Object *o, const char *sig, const char *src);
static void cb_exec(void *data, Evas_Object *o, const char *sig, const char *src);

static void mouse_down(void *data, Evas *e, Evas_Object *o, void *event_info);
static void mouse_up(void *data, Evas *e, Evas_Object *o, void *event_info);
static void mouse_move(void *data, Evas *e, Evas_Object *o, void *event_info);



/* keep this global, so it only has to be created once */
Evas_Smart *smart;


Evas_Object *
iconbar_new(Evas *evas)
{
  Evas_Object *iconbar;

  iconbar = evas_object_smart_add(evas, iconbar_smart_get());
  
  return iconbar;
}

/* set the path for data (bits, order, icons) */
void
iconbar_path_set(Evas_Object *obj, char *path)
{
  Iconbar *ib = evas_object_smart_data_get(obj);
  char buf[2048];

  ib->path = (char *)strdup(path);

  snprintf(buf, sizeof(buf) - 1, "%s/iconbar.eet", ib->path);
  ib->gui = edje_object_add(evas_object_evas_get(ib->obj));
  edje_object_file_set(ib->gui, buf, "iconbar");

  if (!ib->gui)
  {
    printf("no bits!\n");
    return;
  }

  evas_object_move(ib->cont, 10, 10);
  evas_object_resize(ib->cont, 20, 200);
  evas_object_show(ib->cont);
  edje_object_part_swallow(ib->gui, "icons", ib->cont); //was clip
  e_container_callback_order_change_set(ib->cont, write_out_order, ib);

  edje_object_signal_callback_add(ib->gui, "mouse,*", "*", cb_iconbar, ib);
  edje_object_signal_callback_add(ib->gui, "scroll,*", "*", cb_iconbar, ib);
  edje_object_signal_callback_add(ib->gui, "exec*", "*", cb_exec, ib);


  snprintf(buf, PATH_MAX, "%s/", path);
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

Evas_Smart *
iconbar_smart_get()
{
  if (smart) return smart;

  smart = evas_smart_new ("iconbar",
                          iconbar_add,
                          iconbar_del,
                          iconbar_layer_set,
                          iconbar_raise,
                          iconbar_lower,
                          iconbar_stack_above,
                          iconbar_stack_below,
                          iconbar_move,
                          iconbar_resize,
                          iconbar_show,
                          iconbar_hide,
                          iconbar_color_set,
                          iconbar_clip_set,
                          iconbar_clip_unset,
                          NULL
                          );

  return smart;

}


/*********** smart object functions **************/
void
iconbar_add(Evas_Object *o)
{
  Iconbar *ib;
  Evas *evas;

  ib = (Iconbar *)malloc(sizeof(Iconbar));
  memset(ib, 0, sizeof(Iconbar));

  evas_object_smart_data_set(o, ib);
  ib->obj = o;
  
  evas = evas_object_evas_get(o);

  ib->cont = e_container_new(evas);
  e_container_direction_set(ib->cont, 1);
  e_container_spacing_set(ib->cont, 5);
  e_container_alignment_set(ib->cont, CONTAINER_ALIGN_CENTER);
  e_container_fill_policy_set(ib->cont, CONTAINER_FILL_POLICY_FILL_X |
                                        CONTAINER_FILL_POLICY_KEEP_ASPECT);
  e_container_move_button_set(ib->cont, 2);
}


void
iconbar_del(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
}

void
iconbar_layer_set(Evas_Object *o, int l)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_layer_set(ib->gui, l);
}

void
iconbar_raise(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_raise(ib->gui);
}

void
iconbar_lower(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_lower(ib->gui);
}

void
iconbar_stack_above(Evas_Object *o, Evas_Object *above)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_stack_above(ib->gui, above);
}

void
iconbar_stack_below(Evas_Object *o, Evas_Object *below)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_stack_below(ib->gui, below);
}

void
iconbar_move(Evas_Object *o, double x, double y)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_move(ib->gui, x, y);
//  iconbar_icons_fix(ib);
}

void
iconbar_resize(Evas_Object *o, double w, double h)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_resize(ib->gui, w, h);

  if (w > h)
  {
    e_container_direction_set(ib->cont, 0);
    e_container_fill_policy_set(ib->cont, CONTAINER_FILL_POLICY_FILL_Y |
                                          CONTAINER_FILL_POLICY_KEEP_ASPECT);
    e_container_padding_set(ib->cont, 5, 5, 11, 11);
  }
  else
  {
    e_container_direction_set(ib->cont, 1);
    e_container_fill_policy_set(ib->cont, CONTAINER_FILL_POLICY_FILL_X |
                                          CONTAINER_FILL_POLICY_KEEP_ASPECT);
    e_container_padding_set(ib->cont, 11, 11, 5, 5);
  }
}

void
iconbar_show(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  /* show the iconbar */
  if (ib->gui)
    evas_object_show(ib->gui);

  evas_object_show(ib->cont);
}

void
iconbar_hide(Evas_Object *o)
{
  Iconbar *ib;
//Evas_List *l;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_hide(ib->gui);

  evas_object_hide(ib->cont);
}

void
iconbar_color_set(Evas_Object *o, int r, int g, int b, int a)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_color_set(ib->gui, r, g, b, a);
}

void
iconbar_clip_set(Evas_Object *o, Evas_Object *clip)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_clip_set(ib->gui, clip);
}

void
iconbar_clip_unset(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_clip_unset(ib->gui);
}

/**************** helpers *************/

static int
positive_scroll_timer(void *data)
{
  Iconbar *ib;
  double l, r, t, b;
 
  ib = (Iconbar *)data;

  if (!ib->scroll_timer)
      return(0);
    
  ib->scroll += 8;

  if (e_container_direction_get(ib->cont))
  {
    e_container_padding_get(ib->cont, &l, &r, &t, &b);
    e_container_padding_set(ib->cont, l, r, ib->scroll, b);
  }
  else
  {
    e_container_padding_get(ib->cont, &l, &r, &t, &b);
    e_container_padding_set(ib->cont, ib->scroll, r, t, b);
  }
//  iconbar_icons_fix(ib);
  return(1);
}

static int
negative_scroll_timer(void *data)
{
  Iconbar *ib;
  double l, r, t, b;
 
  ib = (Iconbar *)data;
 
  if (!ib->scroll_timer)
      return(0);
    
  ib->scroll -= 8;

  if (e_container_direction_get(ib->cont))
  {
    e_container_padding_get(ib->cont, &l, &r, &t, &b);
    e_container_padding_set(ib->cont, l, r, ib->scroll, b);
  }
  else
  {
    e_container_padding_get(ib->cont, &l, &r, &t, &b);
    e_container_padding_set(ib->cont, ib->scroll, r, t, b);
  }


//  iconbar_icons_fix(ib);

  return(1);
}

Icon *
iconbar_icon_new(Iconbar *ib, char *path)
{
  Evas *evas;
  Icon *ic;
  double w, h;

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
  return ic;
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

  /* add them to the container as specified in order.txt */
  {
    FILE *f;
    char buf[PATH_MAX];
    double count;
    Evas_List *l;
    int i = 0;

    snprintf(buf, sizeof(buf), "%s/order.txt", ib->path);
    
    count = 0;
    f = fopen(buf, "r");

    if (f)
    {

      while (fgets(buf, sizeof(buf) - 1, f))
      {
        buf[strlen(buf) - 1] = 0;

        for (l = icons; l; l = evas_list_next(l))
        {
          Icon *ic;
          char *p;

          ic = evas_list_data(l);
          p = strrchr(ic->file, '/');
          if (p)
          {
            p++;
            if (!strcmp(p, buf))
            {
              new = evas_list_append(new, ic);
            }
          }
        }
      }
      fclose(f);
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
      e_container_element_append(ib->cont, ic->image);
    }

    evas_list_free(icons);
    evas_list_free(new);
  }

  write_out_order(ib);
}

void
write_out_order(void *data)
{
  FILE *f;
  char buf[PATH_MAX];
  Evas_List *l, *ll;
  Iconbar *ib = NULL;
    
  if((ib = (Iconbar*)data))
  {
    snprintf(buf, sizeof(buf), "%s/order.txt", ib->path);
    if ((f = fopen(buf, "w")))
    {
	printf("file opened ok\n");
	for (l = e_container_elements_get(ib->cont); l; l = l->next)
	{
	    Evas_Object *obj = l->data;
	    Icon *ic = evas_object_data_get(obj, "Icon");
	    char *p;

	    p = strrchr(ic->file, '/');
	    if (p)
	    {
		p++;
		fputs(p, f);
		fputs("\n", f);
		printf("write: %s\n", p);
	    }
	}
	fclose(f);
    }
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
      e_container_scroll_start(ib->cont, -1);
  }
  else if (!strcmp(sig, "scroll,2,start"))
  {
      e_container_scroll_start(ib->cont, 1);
  }
  else if (!strcmp(sig, "scroll,1,stop"))
  {
      e_container_scroll_stop(ib->cont);
  }
  else if (!strcmp(sig, "scroll,2,stop"))
  {
      e_container_scroll_stop(ib->cont);
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

static void
cb_exec(void *data, Evas_Object *o, const char *sig, const char *src)
{
  char *exec = (char *)(sig+5);
 
  if (!exec_run_in_dir(exec, get_user_home()))
  {
    printf("Error: failed to run \"%s\"\n", exec);
  }

}

static int
clock_timer(void *data)
{
    char buf[PATH_MAX];
    Iconbar *ib = NULL; 
    Evas_Object *o = NULL;
    struct tm *_tm = NULL;
    time_t _time = time(NULL);

    if((ib = (Iconbar*)data))
    {
      _tm = localtime(&_time);
      strftime(buf, PATH_MAX, "%l:%M %p", _tm);
      edje_object_part_text_set(ib->gui, "clock", buf);
      return(1);
    }
    return(0);
}
