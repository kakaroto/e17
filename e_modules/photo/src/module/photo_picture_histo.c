#include "Photo.h"

static void _cb_menu_deactivate_post(void *data, E_Menu *m);
static void _cb_menu_pre_select(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_menu_post_select(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_menu_select(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_popi_close(void *data);

/*
 * Public functions
 */

int photo_picture_histo_init(Photo_Item *pi)
{
   pi->histo.list = NULL;
   pi->histo.pos = 0;

   return 1;
}

void photo_picture_histo_shutdown(Photo_Item *pi)
{
   Evas_List *l;
   Picture *p;
   Photo_Item *pi2;
   int cur;

   if (pi->histo.list)
     {
        for (l=pi->histo.list; l; l=evas_list_next(l))
          {
             p = evas_list_data(l);
             cur = 0;
             while ( (pi2 = evas_list_nth(p->items_histo, cur)) )
               {
                  if (pi == pi2)
                    p->items_histo = evas_list_remove(p->items_histo, pi);
                  else
                    cur++;
               }
          }
        evas_list_free(pi->histo.list);
        pi->histo.list = NULL;
     }

   pi->histo.pos = 0;

   if (pi->histo.popi)
     {
       photo_popup_info_del(pi->histo.popi);
       pi->histo.popi = NULL;
     }
}

void photo_picture_histo_attach(Photo_Item *pi, Picture *picture)
{
   pi->histo.list = evas_list_prepend(pi->histo.list, picture);

   /* if full and we are not on the last histo item, remove last */
   if (evas_list_count(pi->histo.list) > PICTURE_HISTO_SIZE_MAX)
     {
        Evas_List *l, *l2;
        l = evas_list_last(pi->histo.list);
        l2 = evas_list_nth_list(pi->histo.list, pi->histo.pos);
        if (l != l2)
          pi->histo.list = evas_list_remove_list(pi->histo.list, l);
     }

   /* add the pi to the picture's items histo list if not already here */
   if (!evas_list_find(picture->items_histo, pi))
     picture->items_histo = evas_list_append(picture->items_histo, pi);
}

Picture *photo_picture_histo_change(Photo_Item *pi, int offset)
{
   Picture *picture;
   int new_pos;

   new_pos = pi->histo.pos + offset;

   DPIC(("HISTO change from %d to %d (off %d)", pi->histo.pos, new_pos, offset));

   picture = evas_list_nth(pi->histo.list, new_pos);
   if (picture)
     pi->histo.pos = new_pos;
   else
     DPIC(("HISTO change FAILED !!"));

   return picture;
}

void photo_picture_histo_picture_del(Picture *picture)
{
   Photo_Item *pi;
   Evas_List *l;

   for (l=picture->items_histo; l; l=evas_list_next(l))
     {
        pi = evas_list_data(l);
        while(evas_list_find(pi->histo.list, picture))
          pi->histo.list = evas_list_remove(pi->histo.list, picture);
     }

   evas_list_free(picture->items_histo);
}

void photo_picture_histo_menu_populate(Photo_Item *pi, E_Menu *mn)
{
   E_Menu_Item *mi;
   Picture *p;
   int pos;

   e_menu_post_deactivate_callback_set(mn, _cb_menu_deactivate_post, pi);

   pos = evas_list_count(pi->histo.list) - 1;
   while ( (p=evas_list_nth(pi->histo.list, pos)) )
     {
        mi = e_menu_item_new(mn);
        e_menu_item_label_set(mi, p->infos.name);
        e_menu_item_radio_group_set(mi, 1);
        e_menu_item_radio_set(mi, 1);
        if (pi->histo.pos == pos)
          e_menu_item_toggle_set(mi, 1);

        e_menu_item_submenu_pre_callback_set(mi, _cb_menu_pre_select, pi);
        e_menu_item_submenu_post_callback_set(mi, _cb_menu_post_select, pi);
        e_menu_item_callback_set(mi, _cb_menu_select, pi);

        pos--;
     }
}


/*
 * Private functions
 *
 */

static void
_cb_menu_deactivate_post(void *data, E_Menu *m)
{
   e_object_del(E_OBJECT(m));
}

static void
_cb_menu_pre_select(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Picture *p;
   Photo_Item *pi;
   char *text;
   int number;

   pi = data;

   number = (evas_list_count(pi->histo.list) - (e_menu_item_num_get(mi)+1));

   DPIC(("Select %d in histo list", number));

   p = evas_list_nth(pi->histo.list, number);
   if (!p) return;

   text = photo_picture_infos_get(p);

   if (pi->histo.popi)
     photo_popup_info_del(pi->histo.popi);

   pi->histo.popi = photo_popup_info_add(pi, p->infos.name, text, p,
                                         4,
                                         POPUP_INFO_PLACEMENT_CENTERED,
                                         _cb_popi_close, NULL);

   free(text);
}

static void
_cb_menu_post_select(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;

   pi = data;
   
   DPIC(("Histo menu : Post select callback"));

   if (pi->histo.popi)
     photo_popup_info_del(pi->histo.popi);
   pi->histo.popi = NULL;
}


static void
_cb_menu_select(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;
   int no;

   pi = data;

   no = e_menu_item_num_get(mi);
   photo_item_action_change(pi,
                            pi->histo.pos -
                            (evas_list_count(pi->histo.list) - (no+1)));
}

static void
_cb_popi_close(void *data)
{
   Photo_Item *pi;

   pi = data;
   pi->histo.popi = NULL;
}
