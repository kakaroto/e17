#ifndef ELMXX_GEN_LIST_H
#define ELMXX_GEN_LIST_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"
#include "Gen.h"
#include "GenDataModel.h"

namespace Elmxx {

/* forward declarations */
class GenItem;
class GenListColumnConstructor;
class GenListColumnSelector;

/*!
 * @defgroup Genlist Genlist
 *
 * The aim was to have  more expansive list that the simple list in
 * Elementary that could have more flexible items and allow many more entries
 * while still being fast and low on memory usage. At the same time it was
 * also made to be able to do tree structures. But the price to pay is more
 * complexity when it comes to usage. If all you want is a simple list with
 * icons and a single label, use the normal List object.
 *
 * Signals that you can add callbacks for are:
 *
 * clicked,double - This is called when a user has double-clicked an item. The
 * event_info parameter is the genlist item that was double-clicked.
 *
 * selected - This is called when a user has made an item selected. The
 * event_info parameter is the genlist item that was selected.
 *
 * unselected - This is called when a user has made an item unselected. The
 * event_info parameter is the genlist item that was unselected.
 *
 * expanded -  This is called when elm_genlist_item_expanded_set() is called
 * and the item is now meant to be expanded. The event_info parameter is the
 * genlist item that was indicated to expand. It is the job of this callback
 * to then fill in the child items.
 *
 * contracted - This is called when elm_genlist_item_expanded_set() is called
 * and the item is now meant to be contracted. The event_info parameter is
 * the genlist item that was indicated to contract. It is the job of this
 * callback to then delete the child items
 *
 * expand,request - This is called when a user has indicated they want to
 * expand a tree branch item. The callback should decide if the item can
 * expand (has any children) and then call elm_genlist_item_expanded_set()
 * appropriately to set the state. The event_info parameter is the genlist
 * item that was indicated to expand.
 *
 * contract,request - This is called when a user has indicated they want to
 * contract a tree branch item. The callback should decide if the item can
 * contract (has any children) and then call elm_genlist_item_expanded_set()
 * appropriately to set the state. The event_info parameter is the genlist
 * item that was indicated to contract.
 *
 * realized - This is called when the item in the list is created as a real
 * evas object. event_info parameter is the genlist item that was created.
 * The object may be deleted at any time, so it is up to the caller to
 * not use the object pointer from elm_genlist_item_object_get() in a way
 * where it may point to freed objects.
 *
 * drag,start,up - This is called when the item in the list has been dragged
 * (not scrolled) up.
 *
 * drag,start,down - This is called when the item in the list has been dragged
 * (not scrolled) down.
 *
 * drag,start,left - This is called when the item in the list has been dragged
 * (not scrolled) left.
 *
 * drag,start,right - This is called when the item in the list has been dragged
 * (not scrolled) right.
 *
 * drag,stop - This is called when the item in the list has stopped being
 * dragged.
 *
 * drag - This is called when the item in the list is being dragged.
 *
 * Genlist has a fairly large API, mostly because it's relatively complex,
 * trying to be both expansive, powerful and efficient. First we will begin
 * an overview o the theory behind genlist.
 *
 * Evas tracks every object you create. Every time it processes an event
 * (mouse move, down, up etc.) it needs to walk through objects and find out
 * what event that affects. Even worse every time it renders display updates,
 * in order to just calculate what to re-draw, it needs to walk through many
 * many many objects. Thus, the more objects you keep active, the more
 * overhead Evas has in just doing its work. It is advisable to keep your
 * active objects to the minimum working set you need. Also remember that
 * object creation and deletion carries an overhead, so there is a
 * middle-ground, which is not easily determined. But don't keep massive lists
 * of objects you can't see or use. Genlist does this with list objects. It
 * creates and destroys them dynamically as you scroll around. It groups them
 * into blocks so it can determine the visibility etc. of a whole block at
 * once as opposed to having to walk the whole list. This 2-level list allows
 * for very large numbers of items to be in the list (tests have used up to
 * 2,000,000 items). Also genlist employs a queue for adding items. As items
 * may be different sizes, every item added needs to be calculated as to its
 * size and thus this presents a lot of overhead on populating the list, this
 * genlist employs a queue. Any item added is queued and spooled off over
 * time, actually appearing some time later, so if your list has many members
 * you may find it takes a while for them to all appear, with your process
 * consuming a lot of CPU while it is busy spooling.
 *
 * Genlist also implements a tree structure, but it does so with callbacks to
 * the application, with the application filling in tree structures when
 * requested (allowing for efficient building of a very deep tree that could
 * even be used for file-management). See the above smart signal callbacks for
 * details.
 *
 * An item in the genlist world can have 0 or more text labels (they can be
 * regular text or textblock – that's up to the style to determine), 0 or
 * more icons (which are simply objects swallowed into the genlist item) and
 * 0 or more boolean states that can be used for check, radio or other
 * indicators by the edje theme style. An item may be one of several styles
 * (Elementary provides 2 by default - “default” and “double_label”, but this
 * can be extended by system or application custom themes/overlays/extensions).
 *
 * In order to implement the ability to add and delete items on the fly,
 * Genlist implements a class/callback system where the application provides
 * a structure with information about that type of item (genlist may contain
 * multiple different items with different classes, states and styles).
 * Genlist will call the functions in this struct (methods) when an item is
 * “realized” (that is created dynamically while scrolling). All objects will
 * simply be deleted  when no longer needed with evas_object_del(). The
 * Elm_Genlist_Item_Class structure contains the following members:
 *
 * item_style - This is a constant string and simply defines the name of the
 * item style. It must be specified and the default should be “default”.
 *
 * func.label_get - This function is called when an actual item object is
 * created. The data parameter is the data parameter passed to
 * elm_genlist_item_append() and related item creation functions. The obj
 * parameter is the genlist object and the part parameter is the string name
 * of the text part in the edje design that is listed as one of the possible
 * labels that can be set. This function must return a strudup()'ed string as
 * the caller will free() it when done.
 *
 * func.icon_get - This function is called when an actual item object is
 * created. The data parameter is the data parameter passed to
 * elm_genlist_item_append() and related item creation functions. The obj
 * parameter is the genlist object and the part parameter is the string name
 * of the icon part in the edje design that is listed as one of the possible
 * icons that can be set. This must return NULL for no object or a valid
 * object. The object will be deleted by genlist on shutdown or when the item
 * its unrealized.
 *
 * func.state_get - This function is called when an actual item object is
 * created. The data parameter is the data parameter passed to
 * elm_genlist_item_append() and related item creation functions. The obj
 * parameter is the genlist object and the part parameter is the string name
 * of the state part in the edje design that is listed as one of the possible
 * states that can be set. Return 0 for false or 1 for true. Genlist will
 * emit a signal to the edje object with “elm,state,XXX,active” “elm” when
 * true (the default is false), where XXX is the name of the part.
 *
 * func.del - This is called when elm_genlist_item_del() is called on an
 * item, elm_genlist_clear() is called on the genlist, or
 * elm_genlist_item_subitems_clear() is called to clear sub-items. This is
 * intended for use when actual genlist items are deleted, so any backing
 * data attached to the item (e.g. its data parameter on creation) can be
 * deleted.
 *
 * Items can be added by several calls. All of them return a Elm_Genlist_Item
 * handle that is an internal member inside the genlist. They all take a data
 * parameter that is meant to be used for a handle to the applications
 * internal data (eg the struct with the original item data). The parent
 * parameter is the parent genlist item this belongs to if it is a tree, and
 * NULL if there is no parent. The flags can be a bitmask of
 * ELM_GENLIST_ITEM_NONE and ELM_GENLIST_ITEM_SUBITEMS. If
 * ELM_GENLIST_ITEM_SUBITEMS is set then this item is displayed as a item
 * that is able to expand and have child items. The func parameter is a
 * convenience callback that is called when the item is selected and the data
 * parameter will be the func_data parameter, obj be the genlist object and
 * vent_info will be the genlist item.
 *
 * elm_genlist_item_append() appends an item to the end of the list, or if
 * there is a parent, to the end of all the child items of the parent.
 * elm_genlist_item_prepend() is the same but prepends to the beginning of
 * the list or children list. elm_genlist_item_insert_before() inserts at
 * item before another item and elm_genlist_item_insert_after() inserts after
 * the indicated item.
 *
 * The application can clear the list with elm_genlist_clear() which deletes
 * all the items in the list and elm_genlist_item_del() will delete a specific
 * item. elm_genlist_item_subitems_clear() will clear all items that are
 * children of the indicated parent item.
 *
 * If the application wants multiple items to be able to be selected,
 * elm_genlist_multi_select_set() can enable this. If the list is
 * single-selection only (the default), then elm_genlist_selected_item_get()
 * will return the selected item, if any, or NULL I none is selected. If the
 * list is multi-select then elm_genlist_selected_items_get() will return a
 * list (that is only valid as long as no items are modified (added, deleted,
 * selected or unselected).
 *
 * To help inspect list items you can jump to the item at the top of the list
 * with elm_genlist_first_item_get() which will return the item pointer, and
 * similarly elm_genlist_last_item_get() gets the item at the end of the list.
 * elm_genlist_item_next_get() and elm_genlist_item_prev_get() get the next
 * and previous items respectively relative to the indicated item. Using
 * these calls you can walk the entire item list/tree. Note that as a tree
 * the items are flattened in the list, so elm_genlist_item_parent_get() will
 * let you know which item is the parent (and thus know how to skip them if
 * wanted).
 *
 * There are also convenience functions. elm_genlist_item_genlist_get() will
 * return the genlist object the item belongs to.  elm_genlist_item_show()
 * will make the scroller scroll to show that specific item so its visible.
 * elm_genlist_item_data_get() returns the data pointer set by the item
 * creation functions.
 *
 * If an item changes (state of boolean changes, label or icons change),
 * then use elm_genlist_item_update() to have genlist update the item with
 * the new state. Genlist will re-realize the item thus call the functions
 * in the _Elm_Genlist_Item_Class for that item.
 *
 * To programmatically (un)select an item use elm_genlist_item_selected_set().
 * To get its selected state use elm_genlist_item_selected_get(). Similarly
 * to expand/contract and item and get its expanded state, use
 * elm_genlist_item_expanded_set() and elm_genlist_item_expanded_get(). And
 * again to make an item disabled (unable to be selected and appear
 * differently) use elm_genlist_item_disabled_set() to set this and
 * elm_genlist_item_disabled_get() to get the disabled state.
 *
 * In general to indicate how the genlist should expand items horizontally to
 * fill the list area, use elm_genlist_horizontal_mode_set(). Valid modes are
 * ELM_LIST_LIMIT and ELM_LIST_SCROLL . The default is ELM_LIST_SCROLL. This
 * mode means that if items are too wide to fit, the scroller will scroll
 * horizontally. Otherwise items are expanded to fill the width of the
 * viewport of the scroller. If it is ELM_LIST_LIMIT, Items will be expanded
 * to the viewport width and limited to that size. This can be combined with
 * a different style that uses edjes' ellipsis feature (cutting text off like
 * this: “tex...”).
 *
 * Items will only call their selection func and callback when first becoming
 * selected. Any further clicks will do nothing, unless you enable always
 * select with elm_genlist_always_select_mode_set(). This means even if
 * selected, every click will make the selected callbacks be called.
 * elm_genlist_no_select_mode_set() will turn off the ability to select
 * items entirely and they will neither appear selected nor call selected
 * callback functions.
 *
 * Remember that you can create new styles and add you own theme augmentation
 * per application with elm_theme_extension_add(). If you absolutely must
 * have a specific style that overrides any theme the user or system sets up
 * you can use elm_theme_overlay_add() to add such a file.
 */
class GenList : public Gen
{
public:  
  static GenList *factory (Evasxx::Object &parent);
  
  void setMultiSelect (bool multi);
  
  bool getMultiSelect ();
  
  void setHorizontal (Elm_List_Mode mode);
  
  Elm_List_Mode getHorizontal ();
  
  void setCompressMode (bool compress);
  
  bool getCompressMode ();
   
  void setHomogeneous (bool homogeneous);
  
  bool getHomogeneous ();
  
  void setBlockCount  (int n);
  
  int getBlockCound ();

  void setDataModel (GenDataModel &model);
  
  /* operations to add items */
  
  /*!
   * Append item to the end of the genlist
   * TODO: C++ comment!
   *
   * This appends the given item to the end of the list or the end of the
   * children if the parent is given.
   *
   * @param obj The genlist object
   * @param itc The item class for the item
   * @param data The item data
   * @param parent The parent item, or NULL if none
   * @param flags Item flags
   * @param func Convenience function called when item selected
   * @param func_data Data passed to @p func above.
   * @return A handle to the item added or NULL if not possible
   *
   */
  GenItem *append (GenListColumnConstructor *construction, const GenItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection);

  GenItem *prepend (GenListColumnConstructor *construction, const GenItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection);

  GenItem *insertBefore (GenListColumnConstructor *construction, const GenItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection);

  GenItem *insertAfter (GenListColumnConstructor *construction, const GenItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection);

  void del (GenItem &item);
  
  GenItem *getItemSelected () const;

  /*!
   * Get the item that is at the x, y canvas coords
   * TODO: C++ comment!
   *
   * This returns the item at the given coordinates (which are canvas relative
   * not object-relative). If an item is at that coordinate, that item handle
   * is returned, and if @p posret is not NULL, the integer pointed to is set
   * to a value of -1, 0 or 1, depending if the coordinate is on the upper
   * portion of that item (-1), on the middle section (0) or on the lower part
   * (1). If NULL is returned as an item (no item found there), then posret
   * may indicate -1 or 1 based if the coordinate is above or below all items
   * respectively in the genlist.
   *
   * @param it The item
   * @param x The input x coordinate
   * @param y The input y coordinate
   * @param posret The position relative to the item returned here
   * @return The item at the coordinates or NULL if none
   *
   */
  GenItem *getItemAtXY (const Eflxx::Point &pos, int &posret) const;

  GenItem *getItemFirst () const;

  GenItem *getItemLast () const;
  
  // TODO: which type is event_info here instead of void*?
  sigc::signal <void, GenListColumnSelector&, const Evasxx::Object&, void*> signalSelect;

private:
  GenList (); // forbid standard constructor
  GenList (const GenList&); // forbid copy constructor
  GenList (Evasxx::Object &parent); // private construction -> use factory ()
  ~GenList (); // forbid direct delete -> use Object::destroy()

  enum InsertOperation
  {
    Append,
    Prepend,
    InsertAfter,
    InsertBefore
  };

  GenItem *insertInternal (GenListColumnConstructor *construction, GenList::InsertOperation op, const GenItem *opItem, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection);
  
  static void gl_sel (void *data, Evas_Object *obj, void *event_info);

  void glSelected (GenListColumnSelector &selection, const Evasxx::Object &eo, void *event_info);
  
  GenDataModel *mModel;

  std::list <GenListColumnSelector*> mInternalSelList;
  std::list <GenListColumnConstructor*> mInternalConstructList;
};

#if 0
   /* operations to add items */
   EAPI Elm_Genlist_Item *elm_genlist_item_prepend(Evas_Object *obj, const Elm_Genlist_Item_Class *itc, const void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item_Flags flags, Evas_Smart_Cb func, const void *func_data);
   EAPI Elm_Genlist_Item *elm_genlist_item_insert_before(Evas_Object *obj, const Elm_Genlist_Item_Class *itc, const void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item *before, Elm_Genlist_Item_Flags flags, Evas_Smart_Cb func, const void *func_data);
   EAPI Elm_Genlist_Item *elm_genlist_item_insert_after(Evas_Object *obj, const Elm_Genlist_Item_Class *itc, const void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item *after, Elm_Genlist_Item_Flags flags, Evas_Smart_Cb func, const void *func_data);
   /* operations to retrieve existing items */


   EAPI const Eina_List  *elm_genlist_selected_items_get(const Evas_Object *obj);
   EAPI Eina_List        *elm_genlist_realized_items_get(const Evas_Object *obj);

   /* available item styles:
    * default
    * default_style - The text part is a textblock
    * double_label
    * icon_top_text_bottom
    */


#endif // 0

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_H
