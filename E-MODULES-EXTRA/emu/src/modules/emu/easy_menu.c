#include "easy_menu.h"

static struct _Menu_Data *_easy_menu_add_menus_real(Easy_Menu * easy_menu,
                                                    char *input, char *end,
                                                    int *i, int level, void (*func) (void *data, E_Menu *m, E_Menu_Item *mi),
                                                    void *data);
static void _easy_menu_menu_cb_create(E_Menu *m, void *category_data, void *data);
static void _easy_menu_menu_cb_destroy(void *data);
static void _easy_menu_menu_cb_free(void *obj);

/**
 * Construct a menu.
 *
 * This is a recursive fyunction that constructs a full menu with sub menus.
 *
 * @param   input the start of the description of the menu.
 * @param   end the end of the description of the menu.
 * @param   i Used to track the currently parsed portion of the menu description during recursion.
 * @param   level the current menu level.
 * @param   func the pointer to your face.
 * @param   data the pointer to your face.
 */
EAPI Easy_Menu *
easy_menu_add_menus(char *name, char *category, char *input, int length, void (*func) (void *data, E_Menu *m, E_Menu_Item *mi),
                    void *data)
{
   Easy_Menu *menu = NULL;

   menu = E_NEW(Easy_Menu, 1);
   if (menu)
     {
        menu->buffer = malloc(length + 1);

        if (menu->buffer)
          {
             int i = 0;

             memcpy(menu->buffer, input, length);
             menu->name = strdup(name);
             if (category == NULL)
                category = "";
             menu->category = strdup(category);
             menu->menu = _easy_menu_add_menus_real(menu, menu->buffer, menu->buffer + length, &i, 0, func, data);

             if (menu->menu)
               {
                  e_object_data_set(E_OBJECT(menu->menu->menu), menu);
                  e_object_del_attach_func_set(E_OBJECT(menu->menu->menu), _easy_menu_menu_cb_free);

                  /* A category that is an empty string, is handled by the caller. */
                  if (strlen(menu->category) != 0)
                     menu->category_cb =
                        e_menu_category_callback_add(menu->category, _easy_menu_menu_cb_create, _easy_menu_menu_cb_destroy, menu);

                  menu->valid = 1;
               }
          }
        else
           E_FREE(menu);
     }

   return menu;
}

static struct _Menu_Data *
_easy_menu_add_menus_real(Easy_Menu * easy_menu, char *input, char *end, int *i, int level,
                          void (*func) (void *data, E_Menu *m, E_Menu_Item *mi), void *data)
{
   char *oldInput = input;
   struct _Menu_Data *menu = calloc(1, sizeof(struct _Menu_Data));
   struct _Menu_Item_Data *item = NULL;

   menu->menu = e_menu_new();
   while (input < end)
     {
        int count = 0;

        /* Skip spaces, but count them so we know what level this line is at. */
        while (*(input + count) == ' ')
           count++;
        if (count == level)
          {                     /* Good, we are on the level. */
             input += count;
             if (item == NULL)
               {                /* This is the first one on this level. */
                  item = calloc(1, sizeof(struct _Menu_Item_Data));
                  menu->items = item;
               }
             else
               {                /* Next! */
                  item->next = calloc(1, sizeof(struct _Menu_Item_Data));
                  item = item->next;
               }

             /* Setup the currently known item stuff. */
             item->item = e_menu_item_new(menu->menu);
             item->name = input;
             item->action = input;
             item->data = data;
             item->easy_menu = easy_menu;

             /* Parse the options. */
             while ((input < end) && (*input != '|') && (*input != '\0') && (*input != '\n'))
                input++;
             if ((input < end) && (*input == '|'))
               {                /* Found an action. */
                  *(input++) = '\0';
                  item->action = input;
                  /* Keep parsing. */
                  while ((input < end) && (*input != '|') && (*input != '\0') && (*input != '\n'))
                     input++;
                  if ((input < end) && (*input == '|'))
                    {           /* Found an edje. */
                       *(input++) = '\0';
                       item->edje = input;
                       /* Now we are just looking for the end of the edge. */
                       while ((input < end) && (*input != '\0') && (*input != '\n'))
                          input++;
                    }
               }
             *(input++) = '\0';
             if (menu->name == NULL)
               {                /* If we haven't done so already, setup the menu name and level. */
                  menu->level = level;
                  menu->name = item->name;
               }

             if ((item->name[0] == '-') && (item->name[1] == '\0'))
                e_menu_item_separator_set(item->item, 1);
             else
               {                /* Set up the item with our parsed data. */
                  e_menu_item_label_set(item->item, D_(item->name));
                  if (item->edje)
                     e_util_menu_item_edje_icon_set(item->item, item->edje);
                  if (item->action)
                     e_menu_item_callback_set(item->item, func, item);
               }
          }
        else if (count > level)
          {                     /* We have to add a sub menu here. */
             struct _Menu_Data *last_menu = menu;

             /* Seek out the end of the list, so we can add onto it. */
             while (last_menu->next != NULL)
                last_menu = last_menu->next;

             /* A recursing we will go. */
             last_menu->next = _easy_menu_add_menus_real(easy_menu, input, end, i, level + 1, func, data);
             e_menu_item_submenu_set(item->item, last_menu->next->menu);
             /* The recursion completed this much parsing for us, catch up. */
             input = input + (*i);
          }
        else if (count < level)
           /* Finished with this level, go back one.  */
           break;
     }

   /* Figure out how much we parsed, so the previous level can catch up. */
   (*i) = input - oldInput;
   return menu;
}

/**
 * Handle sub menu creation for a menu category.
 *
 * @param   m the menu.
 * @param   category_data unused.
 * @param   data the pointer you passed to e_menu_category_callback_add().
 * @ingroup Easy_Menu_Module_Menu_Group
 */
void
_easy_menu_menu_cb_create(E_Menu *m, void *category_data, void *data)
{
   Easy_Menu *menu;

   menu = data;
   if ((menu) && (m) && (menu->valid))
     {
        /* Since this is created relative to the passed in menu, we need to create it each time. */
        menu->category_data = category_data;
        menu->item = e_menu_item_new(m);
        if (menu->item)
          {
             /* Tack on the sub menu. */
             e_menu_item_label_set(menu->item, menu->name);
             e_menu_item_submenu_set(menu->item, menu->menu->menu);
          }
     }
}

/**
 * Handle sub menu destruction for a menu category.
 *
 * @param   data the pointer you passed to e_menu_category_callback_add().
 * @ingroup Easy_Menu_Module_Menu_Group
 */
static void
_easy_menu_menu_cb_destroy(void *data)
{
   Easy_Menu *menu;

   menu = data;
   if (menu)
     {
        menu->category_data = NULL;
        if (menu->item)
          {
             e_object_del(E_OBJECT(menu->item));
             menu->item = NULL;
          }
     }
}

/**
 * Handle menu freeing.
 *
 * @param   obj the pointer you passed to e_object_data_set().
 * @ingroup Easy_Menu_Module_Menu_Group
 */
static void
_easy_menu_menu_cb_free(void *obj)
{
   Easy_Menu *menu;

   menu = e_object_data_get(E_OBJECT(obj));
   if (menu)
     {
        int first = TRUE;

        menu->valid = 0;        /* TO be on the safe side. */

        /* Unlink it from the menu category. */
        if (menu->category_cb)
           e_menu_category_callback_del(menu->category_cb);

        if (menu->item)
          {
             e_object_del(E_OBJECT(menu->item));
             menu->item = NULL;
          }

        /* Go through the attached menu structure, freeing it all. */
        if (menu->menu)
          {
             struct _Menu_Data *next = NULL;
             struct _Menu_Data *men;

             men = menu->menu;
             while (men != NULL)
               {
                  struct _Menu_Item_Data *item = men->items;
                  struct _Menu_Item_Data *next_item = NULL;

                  next = men->next;
                  while (item != NULL)
                    {
                       next_item = item->next;
                       e_object_del(E_OBJECT(item->item));
                       free(item);
                       item = next_item;
                    }
                  if (first)    /* The first one is ourself, we are already being deleted, so don't delete us again. */
                     first = FALSE;
                  else
                     e_object_del(E_OBJECT(men->menu));
                  free(men);
                  men = next;
               }
             menu->menu = NULL;
          }

        E_FREE(menu->name);
        E_FREE(menu->category);
        E_FREE(menu->buffer);
        E_FREE(menu);
     }
}
