#ifndef ELMXX_GEN_LIST_H
#define ELMXX_GEN_LIST_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:

 */
class GenList : public Object
{
public:  
  static GenList *factory (Evasxx::Object &parent);
  
  void clear ();
  
  void setMultiSelect (bool multi);
  
  bool getMultiSelect ();
  
  void setHorizontalMode (Elm_List_Mode mode);
  
  Elm_List_Mode getHorizontalMode ();
  
  void setAlwaysSelectMode (bool alwaysSelect);
  
  bool getAlwaysSelectMode ();
  
  void setNoSelectMode (bool noSelect);
  
  bool getNoSelectMode ();
  
  void setCompressMode (bool compress);
  
  bool getCompressMode ();
  
  void setBounce (bool hBounce, bool vBounce);
  
  void getBounce (bool &hBounceOut, bool &vBounceOut);
  
  void setHomogeneous (bool homogeneous);
  
  bool getHomogeneous ();
  
  void setBlockCount  (int n);
  
  int getBlockCound ();


private:
  GenList (); // forbid standard constructor
  GenList (const GenList&); // forbid copy constructor
  GenList (Evasxx::Object &parent); // private construction -> use factory ()
  ~GenList (); // forbid direct delete -> use Object::destroy()
};

#if 0


   /* operations to add items */
   EAPI Elm_Genlist_Item *elm_genlist_item_append(Evas_Object *obj, const Elm_Genlist_Item_Class *itc, const void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item_Flags flags, Evas_Smart_Cb func, const void *func_data);
   EAPI Elm_Genlist_Item *elm_genlist_item_prepend(Evas_Object *obj, const Elm_Genlist_Item_Class *itc, const void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item_Flags flags, Evas_Smart_Cb func, const void *func_data);
   EAPI Elm_Genlist_Item *elm_genlist_item_insert_before(Evas_Object *obj, const Elm_Genlist_Item_Class *itc, const void *data, Elm_Genlist_Item *before, Elm_Genlist_Item_Flags flags, Evas_Smart_Cb func, const void *func_data);
   EAPI Elm_Genlist_Item *elm_genlist_item_insert_after(Evas_Object *obj, const Elm_Genlist_Item_Class *itc, const void *data, Elm_Genlist_Item *after, Elm_Genlist_Item_Flags flags, Evas_Smart_Cb func, const void *func_data);
   /* operations to retrieve existing items */
   EAPI Elm_Genlist_Item *elm_genlist_selected_item_get(const Evas_Object *obj);
   EAPI const Eina_List  *elm_genlist_selected_items_get(const Evas_Object *obj);
   EAPI Eina_List        *elm_genlist_realized_items_get(const Evas_Object *obj);
   EAPI Elm_Genlist_Item *elm_genlist_at_xy_item_get(const Evas_Object *obj, Evas_Coord x, Evas_Coord y, int *posret);
   EAPI Elm_Genlist_Item *elm_genlist_first_item_get(const Evas_Object *obj);
   EAPI Elm_Genlist_Item *elm_genlist_last_item_get(const Evas_Object *obj);
   /* available item styles:
    * default
    * default_style - The text part is a textblock
    * double_label
    * icon_top_text_bottom
    */

   /* Genlist Item operation */
   EAPI Elm_Genlist_Item *elm_genlist_item_next_get(const Elm_Genlist_Item *item);
   EAPI Elm_Genlist_Item *elm_genlist_item_prev_get(const Elm_Genlist_Item *item);
   EAPI Evas_Object      *elm_genlist_item_genlist_get(const Elm_Genlist_Item *item);
   EAPI Elm_Genlist_Item *elm_genlist_item_parent_get(const Elm_Genlist_Item *it);
   EAPI void              elm_genlist_item_subitems_clear(Elm_Genlist_Item *item);
   EAPI void              elm_genlist_item_selected_set(Elm_Genlist_Item *item, Eina_Bool selected);
   EAPI Eina_Bool         elm_genlist_item_selected_get(const Elm_Genlist_Item *item);
   EAPI void              elm_genlist_item_expanded_set(Elm_Genlist_Item *item, Eina_Bool expanded);
   EAPI Eina_Bool         elm_genlist_item_expanded_get(const Elm_Genlist_Item *item);
   EAPI void              elm_genlist_item_disabled_set(Elm_Genlist_Item *item, Eina_Bool disabled);
   EAPI Eina_Bool         elm_genlist_item_disabled_get(const Elm_Genlist_Item *item);
   EAPI void              elm_genlist_item_display_only_set(Elm_Genlist_Item *it, Eina_Bool display_only);
   EAPI Eina_Bool         elm_genlist_item_display_only_get(const Elm_Genlist_Item *it);
   EAPI void              elm_genlist_item_show(Elm_Genlist_Item *item);
   EAPI void              elm_genlist_item_bring_in(Elm_Genlist_Item *item);
   EAPI void              elm_genlist_item_top_show(Elm_Genlist_Item *item);
   EAPI void              elm_genlist_item_top_bring_in(Elm_Genlist_Item *item);
   EAPI void		  elm_genlist_item_middle_show(Elm_Genlist_Item *it);
   EAPI void		  elm_genlist_item_middle_bring_in(Elm_Genlist_Item *it);
   EAPI void              elm_genlist_item_del(Elm_Genlist_Item *item);
   EAPI const void       *elm_genlist_item_data_get(const Elm_Genlist_Item *item);
   EAPI void              elm_genlist_item_data_set(Elm_Genlist_Item *it, const void *data);
   EAPI const Evas_Object *elm_genlist_item_object_get(const Elm_Genlist_Item *it);
   EAPI void              elm_genlist_item_update(Elm_Genlist_Item *item);
 
#endif // 0

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_H
