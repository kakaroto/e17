/**
 * smart template.  i stole this from rephorm and made it check the
 * data_get calls for some validity.
 *
 * There's example structs in the #if0 block you can easily copy to a
 * header(mysmartobj.h)
 *
 */
#include <Evas.h>
#include <Edje.h>
#include <Etox.h>
#include <Ecore.h>
#include <Esmart/Esmart_Container.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "eke_gui_edje_item.h"

static void eke_gui_edje_item_feed_container_scroll_cb(void *data, Evas_Object *o, 
                                    const char *em, const char *src);

/* smart object handlers */
static Evas_Smart * _eke_gui_edje_item_object_smart_get();
static Evas_Object * eke_gui_edje_item_object_new(Evas *evas);
void _eke_gui_edje_item_object_add(Evas_Object *o);
void _eke_gui_edje_item_object_del(Evas_Object *o);
void _eke_gui_edje_item_object_layer_set(Evas_Object *o, int l);
void _eke_gui_edje_item_object_raise(Evas_Object *o);
void _eke_gui_edje_item_object_lower(Evas_Object *o);
void _eke_gui_edje_item_object_stack_above(Evas_Object *o, Evas_Object *above);
void _eke_gui_edje_item_object_stack_below(Evas_Object *o, Evas_Object *below);
void _eke_gui_edje_item_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
void _eke_gui_edje_item_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
void _eke_gui_edje_item_object_show(Evas_Object *o);
void _eke_gui_edje_item_object_hide(Evas_Object *o);
void _eke_gui_edje_item_object_color_set(Evas_Object *o, int r, int g, int b, int a);
void _eke_gui_edje_item_object_clip_set(Evas_Object *o, Evas_Object *clip);
void _eke_gui_edje_item_object_clip_unset(Evas_Object *o);

static void
_open_feed(void *data, Evas_Object *obj, const char *em, const char *src) 
{
    if(data) {
        char buf[PATH_MAX];
        char *browser;

        browser = getenv("BROWSER");

        snprintf(buf, PATH_MAX, "%s \"%s\"", 
                    (browser ? browser : "firefox"), (char*)data);
        ecore_exe_run(buf, NULL);
    }

    return;
    obj = NULL;
    em = NULL;
    src = NULL;
}

void
eke_gui_edje_item_size_min_get(Evas_Object *o, Evas_Coord *w, Evas_Coord *h)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
      edje_object_size_min_get(data->obj, w, h);
  }
        
}

void
eke_gui_edje_item_init(Evas_Object *o, const char *label, const char *date,
                                            const char *link, const char *body)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    if(edje_object_part_exists(data->obj, "label"))
    {
        if(label) {
            edje_object_part_text_set(data->obj, "label", label);
        } else {
            edje_object_part_text_set(data->obj, "label", "");
        }
    }
    if(edje_object_part_exists(data->obj, "link"))
    {
        if(link) {
            edje_object_part_text_set(data->obj, "link", link);
        } else {
            edje_object_part_text_set(data->obj, "link", "");
        }
    }
    if(edje_object_part_exists(data->obj, "date"))
    {
        if(date) {
            edje_object_part_text_set(data->obj, "date", date);
        } else {
            edje_object_part_text_set(data->obj, "date", "");
        }
    }
    if(edje_object_part_exists(data->obj, "body"))
    {
        if(body) {
            Evas_Object *desc, *container;
            Evas_Coord w, h, ew, eh;
            char fmt[2048];
            Evas *evas;
            const char *file;

            evas = evas_object_evas_get(o);

            if ((desc = edje_object_part_swallow_get(data->obj, "body"))) 
                evas_object_del(desc);
            
            desc = evas_object_textblock_add(evas);
            edje_object_file_get(data->obj, &file, NULL);
            snprintf(fmt, 2048, "font_source=%s font=fonts/Vera size=10 color=#000000ff", file);
            evas_object_textblock_format_insert(desc, fmt);

            evas_object_textblock_text_insert(desc, body);
            evas_object_show(desc);

            container = esmart_container_new(evas);
            esmart_container_padding_set(container, 0, 0, 0, 0);
            esmart_container_spacing_set(container, 0);
            esmart_container_move_button_set(container, 2);
            esmart_container_fill_policy_set(container,
                                        CONTAINER_FILL_POLICY_FILL_X);
            esmart_container_alignment_set(container, CONTAINER_ALIGN_TOP);
            esmart_container_direction_set(container,
                                        CONTAINER_DIRECTION_VERTICAL);
            evas_object_show(container);

            edje_object_part_geometry_get(data->obj, "body", NULL, NULL, &w, &h);
            evas_object_resize(desc, w, h);

            edje_object_part_swallow(data->obj, "body", container);

            /* hide the scrollbar if we have less text then visible space */
            evas_object_textblock_format_size_get(desc, &ew, &eh);

            if (eh <= h)
                edje_object_signal_emit(data->obj, "body,scroll,hide", "");

            esmart_container_element_append(container, desc);

            edje_object_signal_callback_add(data->obj, "drag",
                                    "feed.body.item.scroll",
                                    eke_gui_edje_item_feed_container_scroll_cb,
                                    container);
        }
    }

    if (link)
        edje_object_signal_callback_add(data->obj, "eke,link,open", "",
                                                    _open_feed, (void*)link);
  }
}

Evas_Object*
eke_gui_edje_item_new(Evas *e, const char *file, const char *group)
{
  Evas_Object *result = NULL;
  Eke_Gui_Edje_Item *data = NULL;
  Evas_Coord w = (Evas_Coord)0.0, h = (Evas_Coord)0.0;

  if((result = eke_gui_edje_item_object_new(e)))
  {
    if((data = evas_object_smart_data_get(result)))
    {
        data->obj = edje_object_add(e);
        if(edje_object_file_set(data->obj, file, group) == 1)
        {
            evas_object_move(data->obj, -9999, -9999);
            edje_object_size_min_get(data->obj, &w, &h);
            evas_object_resize(data->obj, w, h);
        } else {
            fprintf(stderr, "edje_file_set_error %d\n", 
                    edje_object_load_error_get(data->obj));
                evas_object_del(data->obj);
                evas_object_del(result);
                result = NULL;
        }
    }
  }
  return(result);
}

static void
eke_gui_edje_item_feed_container_scroll_cb(void *data, Evas_Object *o, 
                                    const char *em, const char *src)
{
    double sx = 0.0, sy = 0.0;
    Evas_Coord container_length = 0.0;
    Evas_Object *container = NULL;
    Evas_Coord cw = (Evas_Coord)0.0, ch = (Evas_Coord)0.0;
    Evas_List *list;

    if ((container = (Evas_Object*)data)) {
        Evas_Object *textblock;

        edje_object_part_geometry_get(o, "body", NULL, NULL, &cw, &ch);

        /* container_length dosen't seem be be right if i do
         * esmart_container_elements_length_get(container) so i'm doing this
         * the hard way... */
        list = esmart_container_elements_get(container);
        textblock = evas_list_nth(list, 0);
        evas_object_textblock_format_size_get(textblock, NULL, &container_length);

        edje_object_part_drag_value_get(o, src, &sx, &sy);
        switch (esmart_container_direction_get(container)) {
            case CONTAINER_DIRECTION_HORIZONTAL:
                if(container_length > cw) {
                    container_length -= cw;
                    esmart_container_scroll_offset_set(container,
                                              - (int) (sx * container_length));
                }
                break;

            case CONTAINER_DIRECTION_VERTICAL:
                if(container_length > ch) {
                    container_length -= ch;
                    esmart_container_scroll_offset_set(container,
                                              - (int) (sy * container_length));
                }
                break;

            default:
                fprintf(stderr, "Unknown Container Orientation\n");
                break;
        }
    }

    return;
    em = NULL;
}

/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/


/*** external API ***/

static Evas_Object *
eke_gui_edje_item_object_new(Evas *evas)
{
  Evas_Object *eke_gui_edje_item_object;

  eke_gui_edje_item_object = evas_object_smart_add(evas, _eke_gui_edje_item_object_smart_get());

  return eke_gui_edje_item_object;
}

/*** smart object handler functions ***/

static Evas_Smart *
_eke_gui_edje_item_object_smart_get()
{
  static Evas_Smart *smart = NULL;
  if(smart) return(smart);

  smart = evas_smart_new ("eke_gui_edje_item_object",
                          _eke_gui_edje_item_object_add,
                          _eke_gui_edje_item_object_del,
                          _eke_gui_edje_item_object_layer_set,
                          _eke_gui_edje_item_object_raise,
                          _eke_gui_edje_item_object_lower,
                          _eke_gui_edje_item_object_stack_above,
                          _eke_gui_edje_item_object_stack_below,
                          _eke_gui_edje_item_object_move,
                          _eke_gui_edje_item_object_resize,
                          _eke_gui_edje_item_object_show,
                          _eke_gui_edje_item_object_hide,
                          _eke_gui_edje_item_object_color_set,
                          _eke_gui_edje_item_object_clip_set,
                          _eke_gui_edje_item_object_clip_unset,
                          NULL
                          );

  return smart; 
}

void
_eke_gui_edje_item_object_add(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data = NULL;
    
  data = malloc(sizeof(Eke_Gui_Edje_Item));
  memset(data, 0, sizeof(Eke_Gui_Edje_Item));
  evas_object_smart_data_set(o, data);
}


void
_eke_gui_edje_item_object_del(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
    Evas_Object *obj = NULL;

  if((data = evas_object_smart_data_get(o)))
  {
      if(data->obj) 
      {
        if((obj = edje_object_part_swallow_get(data->obj, "body"))) 
        {
            evas_object_del(obj);
        }
        evas_object_del(data->obj);
      }
    free(data);
  }
}

void
_eke_gui_edje_item_object_layer_set(Evas_Object *o, int l)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_layer_set(data->obj, l);
  }
}

void
_eke_gui_edje_item_object_raise(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_raise(data->obj);
  }
}

void
_eke_gui_edje_item_object_lower(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_lower(data->obj);
  }
}

void
_eke_gui_edje_item_object_stack_above(Evas_Object *o, Evas_Object *above)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_stack_above(data->obj, above);
  }
}

void
_eke_gui_edje_item_object_stack_below(Evas_Object *o, Evas_Object *below)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_stack_below(data->obj, below);
  }
}

void
_eke_gui_edje_item_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_move(data->obj, x, y);
  }
}

void
_eke_gui_edje_item_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  Eke_Gui_Edje_Item *data;

  if((data = evas_object_smart_data_get(o)))
  {
    Evas_List *list;
    Evas_Object *textblock, *container;;
    Evas_Coord eh, ph;
   
    evas_object_resize(data->obj, w, h);
    edje_object_part_geometry_get(data->obj, "body", NULL, NULL, NULL, &ph);

    container = edje_object_part_swallow_get(data->obj, "body");
    list = esmart_container_elements_get(container);
    textblock = evas_list_nth(list, 0);
    evas_object_textblock_format_size_get(textblock, NULL, &eh);

    if (eh > ph)
        edje_object_signal_emit(data->obj, "body,scroll,show", "");
    else
        edje_object_signal_emit(data->obj, "body,scroll,hide", "");
  }
}

void
_eke_gui_edje_item_object_show(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
      evas_object_show(data->obj);
  }
}

void
_eke_gui_edje_item_object_hide(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_hide(data->obj);
  }
}

void
_eke_gui_edje_item_object_color_set(Evas_Object *o, int r, int g, int b, int a)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_color_set(data->obj, r, g, b, a);
  }
}

void
_eke_gui_edje_item_object_clip_set(Evas_Object *o, Evas_Object *clip)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_clip_set(data->obj, clip);
  }
}

void
_eke_gui_edje_item_object_clip_unset(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_clip_unset(data->obj);
  }
}
