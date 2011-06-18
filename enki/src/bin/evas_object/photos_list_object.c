#include "photos_list_object.h"
#include "../main.h"

#define NB_ITEMS_MAX 30
#define LEFT_MARGE   7
#define RIGHT_MARGE  0

typedef struct _Smart_Data Smart_Data;
typedef struct _Pan        Pan;

struct PL_Child_Item
{
   Smart_Data           *sd;
   Eina_List           *l;

   PL_Header_Item      *header;

   void                *data;
   PL_Child_Item_Class *itc;

   Evas_Object         *o_item;
   Evas_Object         *o_icon;

   Eina_Bool            hide;
   int                  x, y;
   int                  resize;
};

typedef struct PL_Block
{
   Eina_List   *l;

   Eina_List   *headers;

   int          is_construct;
   int          y_start, y_end;

   int          hide_cumul_y;
   int          hide_nb_childs;

   //parent, photos_list_object
   Evas_Object *obj;
}PL_Block;

struct PL_Header_Item
{
   Smart_Data           *sd;
   Eina_List            *l;

   PL_Block             *block;

   Eina_List            *childs;
   int                   child_construct;

   void                 *data;
   PL_Header_Item_Class *itc;
   int                   is_construct;

   Evas_Object          *icon;

   int                   y_start, y_end;
   Eina_Bool             hide;
   int                   hide_nb_childs;
   int                   hide_cumul_y;

   int                   resize;
};

struct _Smart_Data
{
   Evas_Object *obj;
   Evas_Object *scroll, *pan_smart;
   Pan         *pan;

   Eina_List   *l_items;

   Eina_List   *selected;
   Eina_Bool    multiselect;
   Eina_Bool    open_event;

   int          child_w;
   int          child_h;
   int          header_h;
   int          total_h;

   int          freeze;

   int          header_resize;
   int          child_resize;
};

#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret)  \
  {                                                  \
     char *_e_smart_str;                             \
                                                     \
     if (!o) return ret;                             \
     smart = evas_object_smart_data_get(o);          \
     if (!smart) return ret;                         \
     _e_smart_str = (char *)evas_object_type_get(o); \
     if (!_e_smart_str) return ret;                  \
     if (strcmp(_e_smart_str, type)) return ret;     \
  }

#define E_SMART_OBJ_GET(smart, o, type)              \
  {                                                  \
     char *_e_smart_str;                             \
                                                     \
     if (!o) return;                                 \
     smart = evas_object_smart_data_get(o);          \
     if (!smart) return;                             \
     _e_smart_str = (char *)evas_object_type_get(o); \
     if (!_e_smart_str) return;                      \
     if (strcmp(_e_smart_str, type)) return;         \
  }

#define E_OBJ_NAME "photo_manager_object"

struct _Pan
{
   Evas_Object *clipped;
   Smart_Data  *sd;
};

static Evas_Smart_Class _pan_sc = {NULL};

static Evas_Smart *smart = NULL;

static void _smart_init(void);
static void _smart_add(Evas_Object *obj);
static void _smart_del(Evas_Object *obj);
static void _smart_move(Evas_Object *obj,
                        Evas_Coord   x,
                        Evas_Coord   y);
static void _smart_resize(Evas_Object *obj,
                          Evas_Coord   w,
                          Evas_Coord   h);
static void      _smart_show(Evas_Object *obj);
static void      _smart_hide(Evas_Object *obj);

static void      _recalc(Evas_Object *obj);
static void      _recalc_after(Evas_Object    *obj,
                               PL_Header_Item *prev);
static void      _update(Evas_Object *obj);
static void      _block_objs_del(PL_Block *block);
static void      _header_objs_del(PL_Header_Item *header);
static void      _smart_clip_set(Evas_Object *obj,
                                 Evas_Object *clip);
static void      _smart_clip_unset(Evas_Object *obj);

static Eina_Bool _header_is_after_displayed_area(Evas_Object    *obj,
                                                 PL_Header_Item *header);
static Eina_Bool _child_is_after_displayed_area(Evas_Object   *obj,
                                                PL_Child_Item *child);

static void _pan_add(Evas_Object *obj);
static void _pan_del(Evas_Object *obj);
static void _pan_resize(Evas_Object *obj,
                        Evas_Coord   w,
                        Evas_Coord   h);
static void _pan_move(Evas_Object *obj,
                      Evas_Coord   x,
                      Evas_Coord   y);

static void _photos_list_object_item_selected_set(PL_Child_Item *item,
                                                  Eina_Bool      selected);

static void _header_clear(PL_Header_Item *header);
static void _header_build(PL_Header_Item *header);
static void _child_clear(PL_Child_Item *child);
static void _child_build(PL_Child_Item *child);


void _item_select(void        *data,
                  Evas_Object *obj,
                  const char  *signal,
                  const char  *source);

static void
_clicked_right(void        *data,
               Evas_Object *_obj,
               void        *even_info)
{
   Smart_Data *sd;
   Evas_Object *obj;
   PL_Child_Item *item = data;

   obj = item->header->block->obj;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_smart_callback_call(obj, "clicked,right", item->data);
}

static void
_clicked_menu(void        *data,
              Evas_Object *obj,
              void        *even_info)
{
   PL_Child_Item *item = data;
   evas_object_smart_callback_call(item->header->block->obj, "clicked,menu", item->data);
}

void
_item_open(void        *data,
           Evas_Object *_obj,
           void        *event_info)
{
   Smart_Data *sd;
   Evas_Object *obj;
   PL_Child_Item *item = data;
   obj = item->header->block->obj;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->open_event = EINA_TRUE;
   evas_object_smart_callback_call(obj, "open", item->data);
}

void _item_select(void        *data,
             Evas_Object *_obj,
             const char  *signal,
             const char  *source)
{
   Evas_Object *obj;
   Smart_Data *sd;
   PL_Child_Item *item = data;
   obj = item->header->block->obj;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   //unselect
   if(eina_list_data_find(sd->selected, item))
       _photos_list_object_item_selected_set(item, EINA_FALSE);
   else
      _photos_list_object_item_selected_set(item, EINA_TRUE);
}

void _item_click_double(void        *data,
             Evas_Object *_obj,
             const char  *signal,
             const char  *source)
{
   Evas_Object *obj;
   Smart_Data *sd;
   PL_Child_Item *item = data;
   obj = item->header->block->obj;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   _photos_list_object_item_selected_set(item, EINA_FALSE);

   evas_object_smart_callback_call(obj, "click,double", item->data);
}

void _item_click_right(void        *data,
             Evas_Object *_obj,
             const char  *signal,
             const char  *source)
{
   Evas_Object *obj;
   Smart_Data *sd;
   PL_Child_Item *item = data;
   obj = item->header->block->obj;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   _photos_list_object_item_selected_set(item, EINA_TRUE);

   evas_object_smart_callback_call(obj, "click,right", item->data);
}

Evas_Object *
photos_list_object_add(Evas_Object *obj)
{
   _smart_init();
   return evas_object_smart_add(evas_object_evas_get(obj), smart);
}

void
photos_list_object_freeze(Evas_Object *obj,
                          int          freeze)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if(sd->freeze == freeze) return;
   sd->freeze = freeze;

   if(!freeze)
     {
        _recalc(obj);
        _update(obj);
     }
}

void
photos_list_object_sub_items_size_set(Evas_Object *obj,
                                      int          w,
                                      int          h)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->child_w = w;
   sd->child_h = h;

   sd->child_resize = 1;
   _recalc(obj);
   _update(obj);
}

void
photos_list_object_header_height_set(Evas_Object *obj,
                                     int          h)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->header_h = h;

   _recalc(obj);
   _update(obj);
}

void
photos_list_object_multiselect_set(Evas_Object *obj,
                                   Eina_Bool    b)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   sd->multiselect = b;
}

Eina_Bool
photos_list_object_multiselect_get(Evas_Object *obj)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return EINA_FALSE;

   return sd->multiselect;
}

Eina_List *
photos_list_object_selected_get(Evas_Object *obj)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return NULL;

   return sd->selected;
}

void *
photos_list_object_item_data_get(PL_Child_Item *item)
{
   if(!item) return NULL;
   return item->data;
}

static void
_photos_list_object_item_selected_set(PL_Child_Item *item,
                                      Eina_Bool      selected)
{
   Evas_Object *obj = item->header->block->obj;
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   Eina_List *l, *l_next;
   PL_Child_Item *_item;
   if (!sd) return;

   if(!selected && eina_list_data_find(sd->selected, item))
     {
        edje_object_signal_emit(item->o_item, "unselected", "");
        sd->selected = eina_list_remove(sd->selected, item);

        evas_object_smart_callback_call(obj, "unselect", item->data);
        return;
     }
   else if(selected && !eina_list_data_find(sd->selected, item))
     {
        if(!sd->multiselect)
          {
             EINA_LIST_FOREACH_SAFE(sd->selected, l, l_next, _item)
               {
                  if(item != _item)
                    {
                       edje_object_signal_emit(_item->o_item, "unselected", "");
                       sd->selected = eina_list_remove(sd->selected, _item);
                       evas_object_smart_callback_call(obj, "unselect", _item->data);
                    }
               }
          }
        edje_object_signal_emit(item->o_item, "selected", "");
        sd->selected = eina_list_prepend(sd->selected, item);
        evas_object_smart_callback_call(obj, "select", item->data);
     }
}

void
photos_list_object_item_selected_set(PL_Child_Item *item,
                                     Eina_Bool      selected)
{
   _photos_list_object_item_selected_set(item, selected);
}

PL_Header_Item *
photos_list_object_item_header_append(Evas_Object          *obj,
                                      PL_Header_Item_Class *itc,
                                      void                 *data)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return NULL;

   PL_Header_Item *header = calloc(1, sizeof(PL_Header_Item));
   header->sd = sd;
   header->itc = itc;
   header->data = data;
   header->resize = 1;

   PL_Block *block_last = eina_list_data_get(eina_list_last(sd->l_items));

   if(!block_last || eina_list_count(block_last->headers) >= NB_ITEMS_MAX)
     {
        block_last = calloc(1, sizeof(PL_Block));
        block_last->obj = obj;
        block_last->l = eina_list_append(NULL, block_last);
        sd->l_items = eina_list_merge(sd->l_items, block_last->l);
     }

   header->l = eina_list_append(NULL, header);

   block_last->headers = eina_list_merge(block_last->headers, header->l);
   header->block = block_last;

   _recalc_after(obj, header);
   if(!_header_is_after_displayed_area(obj, header))
     _update(obj);

   return header;
}

PL_Header_Item *
photos_list_object_item_header_append_relative(Evas_Object          *obj,
                                               PL_Header_Item_Class *itc,
                                               void                 *data,
                                               PL_Header_Item       *relative)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return NULL;

   PL_Header_Item *header = calloc(1, sizeof(PL_Header_Item));
   header->sd = sd;
   header->itc = itc;
   header->data = data;
   header->resize = 1;

   if(!relative)
     {
        PL_Block *block = eina_list_data_get(sd->l_items);

        if(!block || eina_list_count(block->headers) >= NB_ITEMS_MAX)
          {
             block = calloc(1, sizeof(PL_Block));
             block->obj = obj;
             block->l = eina_list_append(NULL, block);
             sd->l_items = eina_list_merge(block->l, sd->l_items);
          }

        header->l = eina_list_append(NULL, header);
        block->headers = eina_list_merge(header->l, block->headers);
        header->block = block;
     }
   else
     {
        PL_Block *block = relative->block;

        if(eina_list_count(block->headers) >= NB_ITEMS_MAX)
          {
             Eina_List *l;
             PL_Header_Item *h;
             Eina_List *right = NULL;
             PL_Block *l_block = block;

             PL_Block *n_block = calloc(1, sizeof(PL_Block));
             n_block->obj = obj;

             l_block->headers = eina_list_split_list(l_block->headers, relative->l, &right);
             n_block->headers = right;
             EINA_LIST_FOREACH(right, l, h)
               {
                  if(h == relative)
                    block = n_block;
                  h->block = n_block;
               }

             sd->l_items = eina_list_append_relative(sd->l_items, n_block, l_block);
             n_block->l = eina_list_data_find_list(sd->l_items, header);
          }

        block->headers = eina_list_append_relative(block->headers, header, relative);
        header->l = eina_list_data_find_list(block->headers, header);
        header->block = block;
     }

   _recalc_after(obj, header);
   if(!_header_is_after_displayed_area(obj, header))
     _update(obj);

   return header;
}

PL_Child_Item *
photos_list_object_item_append(Evas_Object         *obj,
                               PL_Child_Item_Class *itc,
                               PL_Header_Item      *header,
                               void                *data)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return NULL;

   PL_Child_Item *item = calloc(1, sizeof(PL_Child_Item));
   item->sd = sd;
   item->itc = itc;
   item->data = data;
   item->header = header;
   item->resize = 1;
   item->l = eina_list_append(NULL, item);

   header->childs = eina_list_merge(header->childs, item->l);

   _recalc_after(obj, header);
   if(!_child_is_after_displayed_area(obj, item))
     _update(obj);

   return item;
}

PL_Child_Item *
photos_list_object_item_append_relative(Evas_Object         *obj,
                                        PL_Child_Item_Class *itc,
                                        PL_Header_Item      *header,
                                        void                *data,
                                        PL_Child_Item       *relative)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return NULL;

   PL_Child_Item *item = calloc(1, sizeof(PL_Child_Item));
   item->sd = sd;
   item->itc = itc;
   item->data = data;
   item->header = header;
   item->resize = 1;

   if(relative)
     {
        header->childs = eina_list_append_relative(header->childs, item, relative);
        item->l = eina_list_data_find_list(header->childs, item);
     }
   else
     {
        item->l = eina_list_append(NULL, item);
        header->childs = eina_list_merge(item->l, header->childs);
     }

   _recalc_after(obj, header);
   if(!_child_is_after_displayed_area(obj, item))
     _update(obj);

   return item;
}

void
photos_list_object_item_update(PL_Child_Item *item)
{
   Evas_Object *obj = item->header->block->obj;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   item->resize = 1;
   _update(obj);
}

void
photos_list_object_header_update(PL_Header_Item *header)
{
   Evas_Object *obj = header->block->obj;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   header->resize = 1;
   _update(obj);
}

void
photos_list_object_item_del(PL_Child_Item *item)
{
   Evas_Object *obj = item->header->block->obj;
   PL_Header_Item *header = item->header;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if(eina_list_data_find(sd->selected, item))
     sd->selected = eina_list_remove(sd->selected, item);

   header->childs = eina_list_remove(header->childs, item);
   _child_clear(item);
   free(item);

   _recalc_after(header->block->obj, header);
   _update(obj);
}

void
photos_list_object_header_childs_del(PL_Header_Item *header)
{
   Evas_Object *obj = header->block->obj;
   PL_Child_Item *child;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   EINA_LIST_FREE(header->childs, child)
     {
        if(eina_list_data_find(sd->selected, child))
          sd->selected = eina_list_remove(sd->selected, child);
        _child_clear(child);
        free(child);
     }

   _recalc_after(header->block->obj, header);
   _update(obj);
}

void
photos_list_object_header_del(PL_Header_Item *header)
{
   Evas_Object *obj = header->block->obj;
   PL_Block *block_prev, *block = header->block;
   Eina_List *l_next;
   PL_Child_Item *child;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   l_next = eina_list_next(header->l);
   if(!l_next)
     {
        l_next = eina_list_prev(header->block->l);
        if(l_next)
          {
             block_prev = eina_list_data_get(l_next);
             l_next = block_prev->headers;
          }
     }

   block->headers = eina_list_remove(block->headers, header);
   EINA_LIST_FREE(header->childs, child)
     {
        if(eina_list_data_find(sd->selected, child))
          sd->selected = eina_list_remove(sd->selected, child);

        _child_clear(child);
        free(child);
     }
   _header_clear(header);
   free(header);

   if(l_next)
     _recalc_after(block->obj, eina_list_data_get(l_next));
   else
     _recalc(obj);
   _update(obj);
}

void
photos_list_object_top_goto(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   elm_scroller_region_bring_in(sd->scroll, 0, 0, -1, -1);
}

void
photos_list_object_header_bring_in(PL_Header_Item *header)
{
   Eina_List *l;
   PL_Block *block;
   PL_Header_Item *_header;
   Evas_Object *obj = header->block->obj;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   int hide_cumul_y = 0;
   if (!sd) return;

   int x_region, y_region, w_region, h_region;
   elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);

   EINA_LIST_FOREACH(sd->l_items, l, block)
     {
        if(block == header->block)
          break;
        hide_cumul_y += block->hide_cumul_y;
     }

   EINA_LIST_FOREACH(header->block->headers, l, _header)
     {
        if(_header == header)
          break;
        if(_header->hide)
          hide_cumul_y += _header->hide_cumul_y;
     }

   if(header->y_start + header->block->y_start - hide_cumul_y > y_region)
     elm_scroller_region_bring_in(sd->scroll, 0, header->y_start + header->block->y_start - hide_cumul_y + h_region, -1, -1);
   else
     elm_scroller_region_bring_in(sd->scroll, 0, header->y_start + header->block->y_start - hide_cumul_y, -1, -1);
}

void
photos_list_object_header_goto(PL_Header_Item *header)
{
   Eina_List *l;
   PL_Block *block;
   PL_Header_Item *_header;
   Evas_Object *obj = header->block->obj;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   int hide_cumul_y = 0;
   if (!sd) return;

   int x_region, y_region, w_region, h_region;
   elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);

   EINA_LIST_FOREACH(sd->l_items, l, block)
     {
        if(block == header->block)
          break;
        hide_cumul_y += block->hide_cumul_y;
     }

   EINA_LIST_FOREACH(header->block->headers, l, _header)
     {
        if(_header == header)
          break;
        if(_header->hide)
          hide_cumul_y += _header->hide_cumul_y;
     }

   if(header->y_start + header->block->y_start - hide_cumul_y > y_region)
     elm_scroller_region_show(sd->scroll, 0, header->y_start + header->block->y_start - hide_cumul_y + h_region, -1, -1);
   else
     elm_scroller_region_show(sd->scroll, 0, header->y_start + header->block->y_start - hide_cumul_y, -1, -1);
}

void
photos_list_object_item_bring_in(PL_Child_Item *child)
{
   Eina_List *l;
   PL_Block *block;
   PL_Header_Item *_header;
   PL_Header_Item *header;
   Evas_Object *obj = child->header->block->obj;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   int hide_cumul_y = 0;
   if (!sd) return;

   header = child->header;

   int x_region, y_region, w_region, h_region;
   elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);

   EINA_LIST_FOREACH(sd->l_items, l, block)
     {
        if(block == header->block)
          break;
        hide_cumul_y += block->hide_cumul_y;
     }

   EINA_LIST_FOREACH(header->block->headers, l, _header)
     {
        if(_header == header)
          break;
        if(_header->hide)
          hide_cumul_y += _header->hide_cumul_y;
     }

   if(header->y_start + header->block->y_start - hide_cumul_y + child->y > y_region)
     elm_scroller_region_bring_in(sd->scroll, 0, header->y_start + header->block->y_start - hide_cumul_y + child->y + h_region, -1, -1);
   else
     elm_scroller_region_bring_in(sd->scroll, 0, header->y_start + header->block->y_start - hide_cumul_y + child->y, -1, -1);
}

void
photos_list_object_hide_all(Evas_Object *obj)
{
   Eina_List *l, *_l, *__l;
   PL_Block *block;
   PL_Header_Item *header;
   PL_Child_Item *child;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   EINA_LIST_FOREACH(sd->l_items, l, block)
     {
        block->hide_cumul_y = block->y_end - block->y_start;
        block->hide_nb_childs = 0;
        EINA_LIST_FOREACH(block->headers, _l, header)
          {
             header->hide = EINA_TRUE;
             header->hide_cumul_y = header->y_end - header->y_start;
             header->hide_nb_childs = eina_list_count(header->childs);
             block->hide_nb_childs += eina_list_count(header->childs);
             EINA_LIST_FOREACH(header->childs, __l, child)
               child->hide = EINA_TRUE;
          }
     }
   _update(obj);
}

void
photos_list_object_show_all(Evas_Object *obj)
{
   Eina_List *l, *_l, *__l;
   PL_Block *block;
   PL_Header_Item *header;
   PL_Child_Item *child;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   EINA_LIST_FOREACH(sd->l_items, l, block)
     {
        if(block->hide_cumul_y > 0 || block->hide_nb_childs > 0)
          {
             EINA_LIST_FOREACH(block->headers, _l, header)
               {
                  header->hide = EINA_FALSE;
                  header->hide_cumul_y = 0;
                  if(header->hide_nb_childs > 0)
                    {
                       header->hide_nb_childs = 0;
                       EINA_LIST_FOREACH(header->childs, __l, child)
                         child->hide = EINA_FALSE;
                    }
               }
          }
        block->hide_cumul_y = 0;
        block->hide_nb_childs = 0;
     }
   _update(obj);
}

void
photos_list_object_header_show(PL_Header_Item *header)
{
   Eina_List *l;
   PL_Child_Item *child;
   PL_Block *block;
   Smart_Data *sd = evas_object_smart_data_get(header->block->obj);
   if (!sd) return;

   if(!header->hide) return;

   header->hide = EINA_FALSE;
   block = header->block;
   block->hide_cumul_y -= header->hide_cumul_y;
   block->hide_nb_childs -= eina_list_count(header->childs);
   header->hide_cumul_y = 0;
   header->hide_nb_childs = 0;

   EINA_LIST_FOREACH(header->childs, l, child)
     child->hide = EINA_FALSE;

   _update(header->block->obj);
}

void
photos_list_object_item_show(PL_Child_Item *child)
{
   Smart_Data *sd = evas_object_smart_data_get(child->header->block->obj);

   if(!child->hide) return;

   int x_region, y_region, w_region, h_region;
   elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);
   //calc how many sub-items can be displayed on 1 line
   int sub_items_per_line = (w_region - LEFT_MARGE - RIGHT_MARGE) / sd->child_w;
   if(!sub_items_per_line) sub_items_per_line = 1;

   child->hide = EINA_FALSE;

   if(child->header->hide)
     child->header->hide_cumul_y -= sd->header_h;
   child->header->hide = EINA_FALSE;
   child->header->hide_nb_childs--;
   child->header->block->hide_nb_childs--;

   if((eina_list_count(child->header->childs) - child->header->hide_nb_childs) % sub_items_per_line == 1)
     {
        child->header->hide_cumul_y -= sd->child_h;
        child->header->block->hide_cumul_y -= sd->child_h;
     }

   _update(child->header->block->obj);
}

void
photos_list_object_item_hide(PL_Child_Item *child)
{
   Smart_Data *sd = evas_object_smart_data_get(child->header->block->obj);

   if(child->hide) return;

   int x_region, y_region, w_region, h_region;
   elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);
   //calc how many sub-items can be displayed on 1 line
   int sub_items_per_line = (w_region - LEFT_MARGE - RIGHT_MARGE) / sd->child_w;
   if(!sub_items_per_line) sub_items_per_line = 1;

   child->hide = EINA_TRUE;

   child->header->hide_nb_childs++;
   child->header->block->hide_nb_childs++;

   if(eina_list_count(child->header->childs) == child->header->hide_nb_childs)
     {
        child->header->hide_cumul_y += sd->header_h;
        child->header->hide = EINA_TRUE;
     }

   if((eina_list_count(child->header->childs) - child->header->hide_nb_childs) % sub_items_per_line == 0)
     {
        child->header->hide_cumul_y += sd->child_h;
        child->header->block->hide_cumul_y += sd->child_h;
     }

   _update(child->header->block->obj);
}

void
photos_list_object_child_move_after(PL_Child_Item *item,
                                    PL_Child_Item *relative)
{
   PL_Header_Item *header = item->header;

   header->childs = eina_list_remove(header->childs, item);
   if(!relative)
     header->childs = eina_list_prepend(header->childs, item);
   else
     header->childs = eina_list_append_relative(header->childs, item, relative);

   _recalc_after(header->block->obj, header);
   if(!_child_is_after_displayed_area(header->block->obj, item)
      || !_child_is_after_displayed_area(header->block->obj, relative))
     _update(header->block->obj);
}

void
photos_list_object_header_move_after(PL_Header_Item *header,
                                     PL_Header_Item *relative)
{
   Smart_Data *sd;
   PL_Block *block = header->block;
   Evas_Object *obj = block->obj;
   sd = evas_object_smart_data_get(obj);

   block->headers = eina_list_remove(block->headers, header);

   if(!relative)
     {
        block = eina_list_data_get(sd->l_items);

        if(!block || eina_list_count(block->headers) >= NB_ITEMS_MAX)
          {
             block = calloc(1, sizeof(PL_Block));
             block->obj = obj;
             block->l = eina_list_append(NULL, block);
             sd->l_items = eina_list_merge(block->l, sd->l_items);
          }

        header->l = eina_list_append(NULL, header);
        block->headers = eina_list_merge(header->l, block->headers);
        header->block = block;
     }
   else
     {
        block = relative->block;

        if(eina_list_count(block->headers) >= NB_ITEMS_MAX)
          {
             Eina_List *l;
             PL_Header_Item *h;
             Eina_List *right = NULL;
             PL_Block *l_block = block;

             PL_Block *n_block = calloc(1, sizeof(PL_Block));
             n_block->obj = obj;

             l_block->headers = eina_list_split_list(l_block->headers, relative->l, &right);
             n_block->headers = right;
             EINA_LIST_FOREACH(right, l, h)
               {
                  if(h == relative)
                    block = n_block;
                  h->block = n_block;
               }

             sd->l_items = eina_list_append_relative(sd->l_items, n_block, l_block);
             n_block->l = eina_list_data_find_list(sd->l_items, header);
          }

        block->headers = eina_list_append_relative(block->headers, header, relative);
        header->l = eina_list_data_find_list(block->headers, header);
        header->block = block;
     }

   _recalc(obj);
   _update(obj);
}

const Evas_Object *
photos_list_object_item_object_get(PL_Child_Item *item)
{
   return item->o_icon;
}

const Evas_Object *
photos_list_object_header_object_get(PL_Header_Item *header)
{
   return header->icon;
}

static void
_recalc(Evas_Object *obj)
{
   Smart_Data *sd;
   Eina_List *_l, *l, *l2;
   PL_Header_Item *header;
   PL_Block *block;
   PL_Child_Item *child;
   int x_cumul = 0;
   int y_cumul = 0;
   int id_item;
   int sub_items_per_line;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   if(sd->freeze) return;

   int x_region, y_region, w_region, h_region;
   elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);
   //calc how many sub-items can be displayed on 1 line
   sub_items_per_line = (w_region - LEFT_MARGE - RIGHT_MARGE) / sd->child_w;
   if(!sub_items_per_line) sub_items_per_line = 1;

   EINA_LIST_FOREACH(sd->l_items, _l, block)
     {
        block->hide_cumul_y = 0;
        block->y_start = y_cumul;
        block->hide_nb_childs = 0;
        EINA_LIST_FOREACH(block->headers, l, header)
          {
             header->y_start = y_cumul - block->y_start;
             y_cumul += sd->header_h;
             header->hide_cumul_y = 0;
             header->hide_nb_childs = 0;

             id_item = 0;
             x_cumul = LEFT_MARGE;
             EINA_LIST_FOREACH(header->childs, l2, child)
               {
                  child->x = x_cumul;
                  child->y = y_cumul - header->y_start - block->y_start;
                  if(id_item == sub_items_per_line - 1)
                    {
                       id_item = 0;
                       x_cumul = LEFT_MARGE;
                       y_cumul += sd->child_h;
                    }
                  else
                    {
                       id_item++;
                       x_cumul += sd->child_w;
                    }

                  if( !header->hide && child->hide )
                    {
                       header->hide_nb_childs++;
                       block->hide_nb_childs++;
                       if((eina_list_count(child->header->childs)
                           - child->header->hide_nb_childs) % sub_items_per_line == 0)
                         {
                            header->hide_cumul_y += sd->child_h;
                            block->hide_cumul_y += sd->child_h;
                         }
                    }
               }
             if(eina_list_count(header->childs) % sub_items_per_line > 0)
               y_cumul += sd->child_h;
             header->y_end = y_cumul - block->y_start;

             if(header->hide)
               {
                  header->hide_cumul_y = header->y_end - header->y_start;
                  header->hide_nb_childs = eina_list_count(header->childs);
                  block->hide_cumul_y += header->hide_cumul_y;
                  block->hide_nb_childs += eina_list_count(header->childs);
               }
          }
        block->y_end = y_cumul;
     }

   sd->total_h = y_cumul;
   evas_object_size_hint_min_set(sd->pan_smart, w_region, y_cumul);
   evas_object_size_hint_max_set(sd->pan_smart, w_region, y_cumul);
}

static void
_recalc_after(Evas_Object    *obj,
              PL_Header_Item *header)
{
   Smart_Data *sd;
   Eina_List *_l, *l, *l2;
   PL_Header_Item *_header;
   PL_Block *block, *block2;
   PL_Child_Item *child;
   int x_cumul = 0;
   int y_cumul = 0;
   int id_header;
   int sub_headers_per_line;
   Eina_List *l_prev;
   int diff;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   if(sd->freeze) return;

   int x_region, y_region, w_region, h_region;
   elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);
   //calc how many sub-headers can be displayed on 1 line
   sub_headers_per_line = (w_region - LEFT_MARGE - RIGHT_MARGE) / sd->child_w;
   if(!sub_headers_per_line) sub_headers_per_line = 1;

   l_prev = eina_list_prev(header->block->l);
   if(l_prev)
     {
        block2 = eina_list_data_get(l_prev);
        y_cumul = block2->y_end;
     }
   else
     y_cumul = 0;

   header->block->y_start = y_cumul;

   header->block->hide_nb_childs = 0;
   header->block->hide_cumul_y = 0;
   EINA_LIST_FOREACH(header->block->headers, l, _header)
     {
        _header->y_start = y_cumul - _header->block->y_start;
        y_cumul += sd->header_h;
        _header->hide_cumul_y = 0;
        _header->hide_nb_childs = 0;

        id_header = 0;
        x_cumul = LEFT_MARGE;
        EINA_LIST_FOREACH(_header->childs, l2, child)
          {
             child->x = x_cumul;
             child->y = y_cumul - _header->y_start - _header->block->y_start;
             if(id_header == sub_headers_per_line - 1)
               {
                  id_header = 0;
                  x_cumul = LEFT_MARGE;
                  y_cumul += sd->child_h;
               }
             else
               {
                  id_header++;
                  x_cumul += sd->child_w;
               }
             if( !_header->hide && child->hide )
               {
                  _header->hide_nb_childs++;
                  _header->block->hide_nb_childs++;
                  if((eina_list_count(child->header->childs)
                      - child->header->hide_nb_childs) % sub_headers_per_line == 0)
                    {
                       _header->hide_cumul_y += sd->child_h;
                       _header->block->hide_cumul_y += sd->child_h;
                    }
               }
          }
        if(eina_list_count(_header->childs) % sub_headers_per_line > 0)
          y_cumul += sd->child_h;
        _header->y_end = y_cumul - _header->block->y_start;

        if(_header->hide)
          {
             _header->hide_cumul_y = _header->y_end - _header->block->y_start;
             _header->hide_nb_childs = eina_list_count(_header->childs);
             _header->block->hide_cumul_y += _header->hide_cumul_y;
             _header->block->hide_nb_childs += eina_list_count(_header->childs);
          }
     }

   diff = y_cumul - header->block->y_end;
   header->block->y_end = y_cumul;

   EINA_LIST_FOREACH(eina_list_next(header->block->l), _l, block)
     {
        block->y_start += diff;
        block->y_end += diff;
        y_cumul = block->y_end;
     }

   sd->total_h = y_cumul;
   evas_object_size_hint_min_set(sd->pan_smart, w_region, y_cumul);
   evas_object_size_hint_max_set(sd->pan_smart, w_region, y_cumul);
}

static Eina_Bool
_header_is_after_displayed_area(Evas_Object    *obj,
                                PL_Header_Item *header)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return EINA_FALSE;

   /*int x_region, y_region, w_region, h_region;
      elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);

      if(y_region + h_region < header->y_start + header->block->y_start)
      return EINA_TRUE;
      else
      return EINA_FALSE;
    */
   return EINA_FALSE;
}

static Eina_Bool
_child_is_after_displayed_area(Evas_Object   *obj,
                               PL_Child_Item *child)
{
   Smart_Data *sd;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return EINA_FALSE;

   /*int x_region, y_region, w_region, h_region;
      elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);

      if(y_region + h_region < child->y + child->header->y_start + child->header->block->y_start)
      return EINA_TRUE;
      else
      return EINA_FALSE;
    */
   return EINA_FALSE;
}

static void _header_clear(PL_Header_Item *header)
{
   if(header->icon)
      evas_object_del(header->icon);
   header->icon = NULL;
}

static void _header_build(PL_Header_Item *header)
{
   if(header->itc->func.icon_get)
      header->icon = header->itc->func.icon_get(header->data, header->sd->obj);
   evas_object_smart_member_add(header->icon, header->sd->pan_smart);
}

static void _child_clear(PL_Child_Item *child)
{
   if(child->o_icon)
     evas_object_del(child->o_icon);
   child->o_icon = NULL;

   if(child->o_item)
     evas_object_del(child->o_item);
   child->o_item = NULL;
}

static void _child_build(PL_Child_Item *child)
{
   if(child->itc->func.icon_get)
      child->o_icon = child->itc->func.icon_get(child->data, child->sd->obj);

   child->o_item = edje_object_add(evas_object_evas_get(child->sd->obj));
   evas_object_smart_member_add(child->o_item, child->sd->pan_smart);
   edje_object_file_set(child->o_item, Theme, "photo_list/item");
   edje_object_part_swallow(child->o_item, "object.content", child->o_icon);
   evas_object_show(child->o_item);
   edje_object_signal_callback_add(child->o_item, "select", "",
                                  _item_select, child);
   edje_object_signal_callback_add(child->o_item, "click,double", "",
                                  _item_click_double, child);
   edje_object_signal_callback_add(child->o_item, "click,right", "",
                                  _item_click_right, child);

   if(eina_list_data_find(child->sd->selected, child))
      evas_object_smart_callback_call(child->o_icon, "select,extern", NULL);
}


static void
_update(Evas_Object *obj)
{
   Smart_Data *sd;
   Eina_List *_l, *l, *l2;
   PL_Header_Item *header;
   PL_Block *block;
   PL_Child_Item *child;
   int hide_diff = 0;
   int left_marge = 0;
   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if(sd->freeze) return;
   int x_sc, y_sc, w_sc, h_sc;
   evas_object_geometry_get(sd->scroll, &x_sc, &y_sc, &w_sc, &h_sc);
   int x_region, y_region, w_region, h_region;
   elm_scroller_region_get(sd->scroll, &x_region, &y_region, &w_region, &h_region);
   int sub_items_per_line = (w_region - LEFT_MARGE - RIGHT_MARGE) / sd->child_w;

   left_marge = (w_sc - sub_items_per_line * sd->child_w) / 2;

   //printf("%d %d %d %d\n", x_region, y_region, w_region, h_region);

   EINA_LIST_FOREACH(sd->l_items, _l, block)
     {
        if(block->y_end > y_region - hide_diff && block->y_start - hide_diff < y_region + h_region)
          {
             block->is_construct = 1;
             EINA_LIST_FOREACH(block->headers, l, header)
               {
                  if(!header->hide
                     && header->y_end + block->y_start - hide_diff > y_region
                     && header->y_start + block->y_start - hide_diff < y_region + h_region)
                    {
                       header->is_construct = 1;

                       //something is display, test if the header is display
                       if(header->y_start + block->y_start + sd->header_h - hide_diff > y_region
                          && header->y_start + block->y_start - hide_diff < y_region + h_region)
                         {
                            if(sd->header_resize || header->resize || !header->icon)
                              {
                               _header_clear(header);
                               _header_build(header);
                                 header->resize = 1;
                              }
                            if(sd->header_resize || header->resize)
                              {
                                 evas_object_resize(header->icon, w_region, sd->header_h);
                                 header->resize = 0;
                              }
                            evas_object_move(header->icon, x_sc, header->y_start + block->y_start - y_region + y_sc - hide_diff);
                         }
                       else
                         {
                          _header_clear(header);
                         }

                       int x_cumul = left_marge;
                       int y_cumul = sd->header_h;
                       int id_item = 0;
                       int hide_diff_child = hide_diff;
                       EINA_LIST_FOREACH(header->childs, l2, child)
                         {
                            int x, y;
                            if(!child->hide)
                              {
                                 x = x_cumul;
                                 y = y_cumul;
                                 if(id_item == sub_items_per_line - 1)
                                   {
                                      id_item = 0;
                                      x_cumul = left_marge;
                                      y_cumul += sd->child_h;
                                   }
                                 else
                                   {
                                      id_item++;
                                      x_cumul += sd->child_w;
                                   }

                                 if(y + sd->child_h + header->y_start + block->y_start - hide_diff > y_region
                                    && y + header->y_start + block->y_start - hide_diff < y_region + h_region)
                                   {
                                      if(sd->child_resize || child->resize || !child->o_item)
                                        {
                                           _child_clear(child);
                                           _child_build(child);

                                           child->resize = 1;
                                        }
                                      if(sd->child_resize || child->resize)
                                        {
                                           evas_object_resize(child->o_item, sd->child_w, sd->child_h);
                                           child->resize = 0;
                                        }
                                      evas_object_move(child->o_item, x_sc + x,
                                                       y + header->y_start + block->y_start - y_region + y_sc - hide_diff_child);
                                   }
                                 else
                                   {
                                      _child_clear(child);
                                   }
                              }
                            else
                              {
                                 _child_clear(child);
                              }
                         }
                    }
                  else
                    {
                       if(header->is_construct)
                         _header_objs_del(header);
                    }
                  hide_diff += header->hide_cumul_y;
               }
          }
        else
          {
             if(block->is_construct)
               _block_objs_del(block);
             hide_diff += block->hide_cumul_y;
          }
     }
   sd->header_resize = 0;
   sd->child_resize = 0;

   evas_object_size_hint_min_set(sd->pan_smart, w_region, sd->total_h - hide_diff);
   evas_object_size_hint_max_set(sd->pan_smart, w_region, sd->total_h - hide_diff);
}

static void
_block_objs_del(PL_Block *block)
{
   Eina_List *l;
   PL_Header_Item *header;
   EINA_LIST_FOREACH(block->headers, l, header)
     {
        if(header->is_construct)
          _header_objs_del(header);
     }
   block->is_construct = 0;
}

static void
_header_objs_del(PL_Header_Item *header)
{
   Eina_List *l;
   PL_Child_Item *child;
   EINA_LIST_FOREACH(header->childs, l, child)
     {
        _child_clear(child);
     }
   _header_clear(header);
   header->is_construct = 0;
}

/*******************************************/
/* Internal smart object required routines */
/*******************************************/

static void
_scroll_cb(void        *data,
           Evas_Object *obj,
           void        *event_info)
{
   Evas_Object *_obj = (Evas_Object *)data;
   _update(_obj);
}

static void
_smart_init(void)
{
   if (smart) return;
   {
      static const Evas_Smart_Class sc =
      {
         E_OBJ_NAME,
         EVAS_SMART_CLASS_VERSION,
         _smart_add,
         _smart_del,
         _smart_move,
         _smart_resize,
         _smart_show,
         _smart_hide,
         NULL,
         _smart_clip_set,
         _smart_clip_unset,
         NULL,
         NULL,
         NULL,
         NULL
      };
      smart = evas_smart_class_new(&sc);
   }
}

static void
_smart_add(Evas_Object *obj)
{
   Smart_Data *sd;
   static Evas_Smart *pan_smart = NULL;

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   sd->obj = obj;
   evas_object_smart_data_set(obj, sd);

   sd->scroll = elm_scroller_add(obj);
   elm_scroller_bounce_set(sd->scroll, 0, 1);
   evas_object_smart_callback_add(sd->scroll, "scroll", _scroll_cb, obj);
   evas_object_smart_member_add(sd->scroll, obj);

   if (!pan_smart)
     {
        static Evas_Smart_Class sc;

        evas_object_smart_clipped_smart_set(&_pan_sc);
        sc = _pan_sc;
        sc.name = "photos_list_object_pan";
        sc.version = EVAS_SMART_CLASS_VERSION;
        sc.add = _pan_add;
        sc.del = _pan_del;
        sc.move = _pan_move;
        sc.resize = _pan_resize;
        pan_smart = evas_smart_class_new(&sc);
     }
   if (pan_smart)
     {
        sd->pan_smart = evas_object_smart_add(evas_object_evas_get(obj), pan_smart);
        sd->pan = evas_object_smart_data_get(sd->pan_smart);
        sd->pan->sd = sd;
        elm_scroller_content_set(sd->scroll, sd->pan_smart);
     }

   sd->header_h = 90;
}

static void
_smart_del(Evas_Object *obj)
{
   PL_Block *block;
   PL_Header_Item *header;
   PL_Child_Item *child;

   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   EINA_LIST_FREE(sd->l_items, block)
     {
        EINA_LIST_FREE(block->headers, header)
          {
             EINA_LIST_FREE(header->childs, child)
               {
                  _child_clear(child);
                  free(child);
               }
             _header_clear(header);
             free(header);
          }
        free(block);
     }

   evas_object_del(sd->pan_smart);
   evas_object_del(sd->scroll);
   if(sd) free(sd);
}

static void
_smart_move(Evas_Object *obj,
            Evas_Coord   x,
            Evas_Coord   y)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_move(sd->scroll, x, y);
   _update(obj);
}

static void
_smart_resize(Evas_Object *obj,
              Evas_Coord   w,
              Evas_Coord   h)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_resize(sd->scroll, w, h);
   sd->header_resize = 1;
   _recalc(obj);
   _update(obj);
}

static void
_smart_show(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_show(sd->scroll);
}

static void
_smart_hide(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_hide(sd->scroll);
}

static void
_smart_clip_set(Evas_Object *obj,
                Evas_Object *clip)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_set(sd->scroll, clip);
}

static void
_smart_clip_unset(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_unset(sd->scroll);
}

static void
_pan_add(Evas_Object *obj)
{
   Pan *sd;

   _pan_sc.add(obj);
   sd = calloc(1, sizeof(Pan));
   if (!sd) return;
   sd->clipped = evas_object_smart_clipped_clipper_get(obj);
   evas_object_smart_data_set(obj, sd);
}

static void
_pan_del(Evas_Object *obj)
{
   Pan *sd = evas_object_smart_data_get(obj);

   if (!sd) return;
   _pan_sc.del(obj);
}

static void
_pan_move(Evas_Object *obj,
          Evas_Coord   x,
          Evas_Coord   y)
{
   Pan *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   Evas_Object *clipped = evas_object_smart_clipped_clipper_get(obj);
   evas_object_move(clipped, x, y);
}

static void
_pan_resize(Evas_Object *obj,
            Evas_Coord   w,
            Evas_Coord   h)
{
   Pan *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   Evas_Object *clipped = evas_object_smart_clipped_clipper_get(obj);
   evas_object_resize(clipped, w, h);
}

