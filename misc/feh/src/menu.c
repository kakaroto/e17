/* menu.c
 *
 * Copyright (C) 2000 Tom Gilbert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "feh.h"
#include "winwidget.h"
#include "filelist.h"
#include "options.h"

Window menu_cover = 0;
feh_menu *menu_root = NULL;
feh_menu *menu_main = NULL;
feh_menu *menu_close = NULL;
feh_menu *menu_bg = NULL;
static feh_menu_list *menus = NULL;

static char *TILED_BG = "Set as Tiled Background";
static char *SCALED_BG = "Set as Scaled Background";


static void feh_menu_cb_about(feh_menu * m, feh_menu_item * i, void *data);
static void feh_menu_cb_close(feh_menu * m, feh_menu_item * i, void *data);
static void feh_menu_cb_exit(feh_menu * m, feh_menu_item * i, void *data);
static void feh_menu_cb_reload(feh_menu * m, feh_menu_item * i, void *data);
static void feh_menu_cb_remove(feh_menu * m, feh_menu_item * i, void *data);
static void feh_menu_cb_delete(feh_menu * m, feh_menu_item * i, void *data);
static void feh_menu_cb_background_set_tiled(feh_menu * m, feh_menu_item * i,

                                             void *data);
static void feh_menu_cb_background_set_scaled(feh_menu * m, feh_menu_item * i,

                                              void *data);
static void feh_menu_cb_background_set_centered(feh_menu * m,

                                                feh_menu_item * i,
                                                void *data);
static void feh_menu_cb_background_set_tiled_no_file(feh_menu * m,
                                                     feh_menu_item * i,

                                                     void *data);
static void feh_menu_cb_background_set_scaled_no_file(feh_menu * m,
                                                      feh_menu_item * i,

                                                      void *data);
static void feh_menu_cb_background_set_centered_no_file(feh_menu * m,
                                                        feh_menu_item * i,

                                                        void *data);

/* FIXME if someone can tell me which option is causing indent to be
   braindead here, I will buy them a beer */
static void feh_menu_cb_sort_filename(feh_menu * m, feh_menu_item * i,

                                      void *data);
static void feh_menu_cb_sort_imagename(feh_menu * m, feh_menu_item * i,

                                       void *data);
static void feh_menu_cb_sort_filesize(feh_menu * m, feh_menu_item * i,

                                      void *data);
static void feh_menu_cb_sort_randomize(feh_menu * m, feh_menu_item * i,

                                       void *data);
static void feh_menu_cb_jump_to(feh_menu * m, feh_menu_item * i, void *data);
static feh_menu *feh_menu_func_gen_jump(feh_menu * m, feh_menu_item * i,

                                        void *data);
static feh_menu *feh_menu_func_gen_info(feh_menu * m, feh_menu_item * i,

                                        void *data);
static void feh_menu_func_free_info(feh_menu * m, void *data);


feh_menu *
feh_menu_new(void)
{
   feh_menu *m;
   XSetWindowAttributes attr;
   feh_menu_list *l;
   static Imlib_Image bg = NULL;

   D_ENTER;

   m = (feh_menu *) emalloc(sizeof(feh_menu));

   attr.backing_store = NotUseful;
   attr.override_redirect = True;
   attr.colormap = cm;
   attr.border_pixel = 0;
   attr.background_pixmap = None;
   attr.save_under = False;
   attr.do_not_propagate_mask = True;

   m->win =
      XCreateWindow(disp, root, 1, 1, 1, 1, 0, depth, InputOutput, vis,
                    CWOverrideRedirect | CWSaveUnder | CWBackingStore |
                    CWColormap | CWBackPixmap | CWBorderPixel |
                    CWDontPropagate, &attr);
   XSelectInput(disp, m->win,
                ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
                LeaveWindowMask | PointerMotionMask | ButtonMotionMask);

   m->name = NULL;
   m->fehwin = NULL;
   m->pmap = 0;
   m->x = 0;
   m->y = 0;
   m->w = 0;
   m->h = 0;
   m->visible = 0;
   m->items = NULL;
   m->next = NULL;
   m->updates = NULL;
   m->needs_redraw = 1;
   m->func_free = NULL;
   m->data = NULL;
   m->calc = 0;
   m->bg = NULL;

   l = emalloc(sizeof(feh_menu_list));
   l->menu = m;
   l->next = menus;
   menus = l;

   imlib_context_set_progress_function(NULL);
   if (!bg)
      feh_load_image_char(&bg, opt.menu_bg);
   if (bg)
      m->bg = feh_imlib_clone_image(bg);

   D_RETURN(m);
}

void
feh_menu_free(feh_menu * m)
{
   feh_menu_item *i;
   feh_menu_list *l, *pl = NULL;

   D_ENTER;

   if (m->name)
      free(m->name);
   XDestroyWindow(disp, m->win);
   if (m->pmap)
      XFreePixmap(disp, m->pmap);
   if (m->updates)
      imlib_updates_free(m->updates);
   for (i = m->items; i;)
   {
      feh_menu_item *ii;

      ii = i;
      i = i->next;
      if (ii->func_free)
         (ii->func_free) (ii->data);
      if (ii->text)
         free(ii->text);
      if (ii->submenu)
         free(ii->submenu);
      free(ii);
   }

   for (l = menus; l; l = l->next)
   {
      if (l->menu == m)
      {
         if (pl)
            pl->next = l->next;
         else
            menus = l->next;
         free(l);
         break;
      }
      pl = l;
   }
   free(m);

   D_RETURN_;
}

feh_menu_item *
feh_menu_find_selected(feh_menu * m)
{
   feh_menu_item *i;

   D_ENTER;

   D(("menu %p\n", m));

   for (i = m->items; i; i = i->next)
   {
      if (MENU_ITEM_IS_SELECTED(i))
         D_RETURN(i);
   }
   D_RETURN(NULL);
}

feh_menu_item *
feh_menu_find_at_xy(feh_menu * m, int x, int y)
{
   feh_menu_item *i;

   D_ENTER;
   D(("looking for menu item at %d,%d\n", x, y));
   for (i = m->items; i; i = i->next)
   {
      if (XY_IN_RECT(x, y, i->x, i->y, i->w, i->h))
      {
         D(("Found an item\n"));
         D_RETURN(i);
      }
   }
   D(("didn't find an item\n"));
   D_RETURN(NULL);
}

void
feh_menu_deselect_selected(feh_menu * m)
{
   feh_menu_item *i;

   D_ENTER;

   if (!m)
      D_RETURN_;

   i = feh_menu_find_selected(m);
   if (i)
   {
      D(("found a selected menu, deselecting it\n"));
      MENU_ITEM_SET_NORMAL(i);
      m->updates =
         imlib_update_append_rect(m->updates, i->x, i->y, i->w, i->h);
      m->needs_redraw = 1;
   }
   D_RETURN_;
}

void
feh_menu_select(feh_menu * m, feh_menu_item * i)
{
   D_ENTER;
   MENU_ITEM_SET_SELECTED(i);
   m->updates = imlib_update_append_rect(m->updates, i->x, i->y, i->w, i->h);
   m->needs_redraw = 1;
   if (m->next)
   {
      feh_menu_hide(m->next);
      m->next = NULL;
   }
   if (i->submenu)
   {
      feh_menu *mm;

      mm = feh_menu_find(i->submenu);
      if (mm)
         feh_menu_show_at_submenu(mm, m, i);
      else if (i->func_gen_sub)
         feh_menu_show_at_submenu(i->func_gen_sub(m, i, i->data), m, i);
   }
   D_RETURN_;
}

void
feh_menu_show_at(feh_menu * m, int x, int y)
{
   D_ENTER;

   if (m->calc)
      feh_menu_calc_size(m);
   if (!menu_cover)
   {
      XSetWindowAttributes attr;

      D(("creating menu cover window\n"));
      attr.override_redirect = True;
      attr.do_not_propagate_mask = True;
      menu_cover =
         XCreateWindow(disp, root, 0, 0, scr->width, scr->height, 0, 0,
                       InputOnly, vis, CWOverrideRedirect | CWDontPropagate,
                       &attr);
      XSelectInput(disp, menu_cover,
                   ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
                   LeaveWindowMask | PointerMotionMask | ButtonMotionMask);

      XRaiseWindow(disp, menu_cover);
      XMapWindow(disp, menu_cover);
      menu_root = m;
      XUngrabPointer(disp, CurrentTime);
      XSetInputFocus(disp, menu_cover, RevertToPointerRoot, CurrentTime);
   }
   m->visible = 1;
   XMoveWindow(disp, m->win, x, y);
   m->x = x;
   m->y = y;
   XRaiseWindow(disp, m->win);
   feh_menu_redraw(m);
   XMapWindow(disp, m->win);
   D_RETURN_;
}

void
feh_menu_show_at_xy(feh_menu * m, winwidget winwid, int x, int y)
{
   D_ENTER;

   if (m->calc)
      feh_menu_calc_size(m);
   m->fehwin = winwid;
   if ((x + m->w) > scr->width)
      x = scr->width - m->w;
   if ((y + m->h) > scr->height)
      y = scr->height - m->h;
   if (x < 0)
      x = 0;
   if (y < 0)
      y = 0;
   feh_menu_move(m, x, y);
   feh_menu_show(m);
   D_RETURN_;
}

void
feh_menu_show_at_submenu(feh_menu * m, feh_menu * parent_m, feh_menu_item * i)
{
   int mx, my;

   D_ENTER;

   if (m->calc)
      feh_menu_calc_size(m);
   mx = parent_m->x + parent_m->w;
   my = parent_m->y + i->y - FEH_MENU_PAD_TOP;
   m->fehwin = parent_m->fehwin;
   parent_m->next = m;
   feh_menu_move(m, mx, my);
   feh_menu_show(m);
   D_RETURN_;
}

void
feh_menu_move(feh_menu * m, int x, int y)
{
   int dx, dy;

   D_ENTER;

   if (!m)
      D_RETURN_;
   dx = x - m->x;
   dy = y - m->y;
   if (m->visible)
      XMoveWindow(disp, m->win, x, y);
   m->x = x;
   m->y = y;
   D_RETURN_;
}

void
feh_menu_hide(feh_menu * m)
{
   D_ENTER;

   if (!m->visible)
      D_RETURN_;
   if (m->next)
   {
      feh_menu_hide(m->next);
      m->next = NULL;
   }
   if (m == menu_root)
   {
      if (menu_cover)
      {
         D(("DESTROYING menu cover\n"));
         XDestroyWindow(disp, menu_cover);
         menu_cover = 0;
      }
   }
   if (m == menu_root)
      menu_root = NULL;
   m->visible = 0;
   XUnmapWindow(disp, m->win);
   feh_menu_deselect_selected(m);
   if (m->func_free)
      m->func_free(m, m->data);
   D_RETURN_;
}

void
feh_menu_show(feh_menu * m)
{
   D_ENTER;
   if (!m)
      D_RETURN_;
   feh_menu_show_at(m, m->x, m->y);
   D_RETURN_;
}

feh_menu_item *
feh_menu_add_entry(feh_menu * m, char *text, Imlib_Image icon, char *submenu,
                   menu_func func, void *data, void (*func_free) (void *data))
{
   feh_menu_item *mi, *ptr;

   D_ENTER;

   mi = (feh_menu_item *) emalloc(sizeof(feh_menu_item));
   mi->state = MENU_ITEM_STATE_NORMAL;
   mi->icon = icon;
   mi->text = estrdup(text);
   mi->submenu = estrdup(submenu);
   mi->func = func;
   mi->func_free = func_free;
   mi->data = data;
   mi->func_gen_sub = NULL;
   mi->next = NULL;

   if (!m->items)
      m->items = mi;
   else
   {
      for (ptr = m->items; ptr; ptr = ptr->next)
      {
         if (!ptr->next)
         {
            ptr->next = mi;
            break;
         }
      }
   }
   m->calc = 1;
   D_RETURN(mi);
}


void
feh_menu_entry_get_size(feh_menu * m, feh_menu_item * i, int *w, int *h)
{
   Imlib_Font fn;
   int tw, th;

   D_ENTER;

   if (i->text)
   {
      fn = imlib_load_font(opt.menu_font);
      if (fn)
      {
         feh_imlib_get_text_size(fn, i->text, &tw, &th, IMLIB_TEXT_TO_RIGHT);
         feh_imlib_free_font(fn);
      }
      *w =
         tw + FEH_MENUITEM_PAD_LEFT + FEH_MENUITEM_PAD_RIGHT +
         FEH_MENU_FONT_SHADOW_OFF_X;
      *h =
         th + FEH_MENUITEM_PAD_TOP + FEH_MENUITEM_PAD_BOTTOM +
         FEH_MENU_FONT_SHADOW_OFF_Y;
   }
   else
   {
      *w = FEH_MENUITEM_PAD_LEFT + FEH_MENUITEM_PAD_RIGHT;
      *h = FEH_MENUITEM_PAD_TOP + FEH_MENUITEM_PAD_BOTTOM;
   }

   D_RETURN_;
   m = NULL;
}

void
feh_menu_calc_size(feh_menu * m)
{
   int prev_w, prev_h;
   feh_menu_item *i;
   int j = 0, count = 0, max_w = 0, max_h = 0, icon_w = 0, next_w = 0;

   D_ENTER;

   prev_w = m->w;
   prev_h = m->h;
   m->calc = 0;

   for (i = m->items; i; i = i->next)
   {
      int w, h;

      feh_menu_entry_get_size(m, i, &w, &h);
      if (w > max_w)
         max_w = w;
      if (h > max_h)
         max_h = h;
      if (i->submenu)
      {
         next_w = FEH_MENU_SUBMENU_W;
         if (FEH_MENU_SUBMENU_H > max_h)
            max_h = FEH_MENU_SUBMENU_H;
      }
      count++;
   }

   for (i = m->items; i; i = i->next)
   {
      if (i->icon)
      {
         Imlib_Image im;

         im = i->icon;
         if (im)
         {
            int iw, ih, ow, oh;

            iw = feh_imlib_image_get_width(im);
            ih = feh_imlib_image_get_height(im);
            if (ih <= max_h)
            {
               ow = iw;
               oh = ih;
            }
            else
            {
               ow = (iw * max_h) / ih;
               oh = max_h;
            }
            if (ow > icon_w)
               icon_w = ow;
            feh_imlib_free_image(im);
         }
      }
   }
   m->h = FEH_MENU_PAD_TOP;
   for (i = m->items; i; i = i->next)
   {
      i->x = FEH_MENU_PAD_LEFT;
      i->y = m->h;
      i->w = max_w + icon_w + next_w;
      i->icon_x = FEH_MENUITEM_PAD_LEFT;
      i->text_x = i->icon_x + icon_w;
      i->sub_x = i->text_x + max_w;
      if (i->text)
         i->h = max_h;
      else
         i->h = FEH_MENU_SEP_MAX_H;
      m->h += i->h;
      j++;
   }
   m->h += FEH_MENU_PAD_BOTTOM;
   m->w = next_w + icon_w + max_w + FEH_MENU_PAD_LEFT + FEH_MENU_PAD_RIGHT;

   if ((prev_w != m->w) || (prev_h != m->h))
   {
      if (m->pmap)
         XFreePixmap(disp, m->pmap);
      m->pmap = 0;
      m->needs_redraw = 1;
      XResizeWindow(disp, m->win, m->w, m->h);
      m->updates = imlib_update_append_rect(m->updates, 0, 0, m->w, m->h);
   }
   D(("menu size calculated. w=%d h=%d\n", m->w, m->h));

   /* Make sure bg is same size */
   if (m->bg)
   {
      int bg_w, bg_h;

      bg_w = feh_imlib_image_get_width(m->bg);
      bg_h = feh_imlib_image_get_height(m->bg);

      if (m->w != bg_w || m->h != bg_h)
      {
         Imlib_Image newim = imlib_create_image(m->w, m->h);

         D(("resizing bg to %dx%d\n", m->w, m->h));

         feh_imlib_blend_image_onto_image(newim, m->bg, 0, 0, 0, bg_w, bg_h,
                                          0, 0, m->w, m->h, 0, 0, 1);
         feh_imlib_free_image_and_decache(m->bg);
         m->bg = newim;
      }
   }

   D_RETURN_;
}

void
feh_menu_draw_item(feh_menu * m, feh_menu_item * i, Imlib_Image im, int ox,
                   int oy)
{
   Imlib_Font fn;

   D_ENTER;

   D(
     ("drawing item %p (text %s) on menu %p (name %s)\n", i, i->text, m,
      m->name));

   if (i->text)
   {
      D(("text item\n"));
      fn = imlib_load_font(opt.menu_font);
      if (fn)
      {
         if (MENU_ITEM_IS_SELECTED(i))
         {
            D(("selected item\n"));
            /* draw selected image */
            feh_menu_item_draw_at(i->x, i->y, i->w, i->h, im, ox, oy, 1);
         }
         else
         {
            D(("unselected item\n"));
            /* draw unselected image */
            feh_menu_item_draw_at(i->x, i->y, i->w, i->h, im, ox, oy, 0);
         }

         /* draw text */
         feh_imlib_text_draw(im, fn,
                             i->x - ox + i->text_x +
                             FEH_MENU_FONT_SHADOW_OFF_X,
                             i->y - oy + FEH_MENUITEM_PAD_TOP +
                             FEH_MENU_FONT_SHADOW_OFF_Y, i->text,
                             IMLIB_TEXT_TO_RIGHT, 0, 0, 0, 60);

         feh_imlib_text_draw(im, fn, i->x - ox + i->text_x,
                             i->y - oy + FEH_MENUITEM_PAD_TOP, i->text,
                             IMLIB_TEXT_TO_RIGHT, 0, 0, 0, 255);
         feh_imlib_free_font(fn);
      }
      else
         weprintf("couldn't load font %s\n", opt.menu_font);
      if (i->icon)
      {
         Imlib_Image im2;

         D(("icon item\n"));

         im2 = i->icon;
         if (im2)
         {
            int iw, ih, ow, oh;

            iw = feh_imlib_image_get_width(im2);
            ih = feh_imlib_image_get_height(im2);
            if (ih <= (i->h - FEH_MENUITEM_PAD_TOP - FEH_MENUITEM_PAD_BOTTOM))
            {
               ow = iw;
               oh = ih;
            }
            else
            {
               ow =
                  (iw *
                   (i->h - FEH_MENUITEM_PAD_TOP -
                    FEH_MENUITEM_PAD_BOTTOM)) / ih;
               oh = i->h - FEH_MENUITEM_PAD_TOP - FEH_MENUITEM_PAD_BOTTOM;
            }
            feh_imlib_blend_image_onto_image(im, im2, 0, 0, 0, iw, ih,
                                             i->x + i->icon_x - ox,
                                             i->y + FEH_MENUITEM_PAD_TOP +
                                             (((i->
                                                h - FEH_MENUITEM_PAD_TOP -
                                                FEH_MENUITEM_PAD_BOTTOM) -
                                               oh) / 2) - oy, ow, oh, 1, 1,
                                             1);
            feh_imlib_free_image(im2);
         }
      }
      if (i->submenu)
      {
         D(("submenu item\n"));
         if (MENU_ITEM_IS_SELECTED(i))
         {
            D(("selected item\n"));
            feh_menu_draw_submenu_at(i->x + i->sub_x,
                                     i->y + FEH_MENUITEM_PAD_TOP +
                                     ((i->
                                       h - FEH_MENUITEM_PAD_TOP -
                                       FEH_MENUITEM_PAD_BOTTOM -
                                       FEH_MENU_SUBMENU_H) / 2),
                                     FEH_MENU_SUBMENU_W, FEH_MENU_SUBMENU_H,
                                     im, ox, oy, 1);
         }
         else
         {
            D(("unselected item\n"));
            feh_menu_draw_submenu_at(i->x + i->sub_x,
                                     i->y + FEH_MENUITEM_PAD_TOP +
                                     ((i->
                                       h - FEH_MENUITEM_PAD_TOP -
                                       FEH_MENUITEM_PAD_BOTTOM -
                                       FEH_MENU_SUBMENU_H) / 2),
                                     FEH_MENU_SUBMENU_W, FEH_MENU_SUBMENU_H,
                                     im, ox, oy, 0);
         }
      }
   }
   else
   {
      D(("separator item\n"));
      feh_menu_draw_separator_at(i->x, i->y, i->w, i->h, im, ox, oy);
   }
   D_RETURN_;
   m = NULL;
}

void
feh_menu_redraw(feh_menu * m)
{
   Imlib_Updates u, uu;

   D_ENTER;

   if ((!m->needs_redraw) || (!m->visible) || (!m->updates))
      D_RETURN_;
   m->needs_redraw = 0;
   if (!m->pmap)
      m->pmap = XCreatePixmap(disp, m->win, m->w, m->h, depth);
   XSetWindowBackgroundPixmap(disp, m->win, m->pmap);

   u = imlib_updates_merge_for_rendering(m->updates, m->w, m->h);
   m->updates = NULL;
   if (u)
   {
      D(("I have updates to render\n"));
      for (uu = u; u; u = imlib_updates_get_next(u))
      {
         int x, y, w, h;
         Imlib_Image im;

         imlib_updates_get_coordinates(u, &x, &y, &w, &h);
         D(("update coords %d,%d %d*%d\n", x, y, w, h));
         im = imlib_create_image(w, h);
         feh_imlib_image_fill_rectangle(im, 0, 0, w, h, 0, 0, 0, 0);
         if (im)
         {
            feh_menu_draw_to_buf(m, im, x, y);
            feh_imlib_render_image_on_drawable(m->pmap, im, x, y, 1, 0, 0);
            feh_imlib_free_image(im);
            XClearArea(disp, m->win, x, y, w, h, False);
         }
      }
      imlib_updates_free(uu);
   }
   D_RETURN_;
}

feh_menu *
feh_menu_find(char *name)
{
   feh_menu_list *l;

   D_ENTER;
   for (l = menus; l; l = l->next)
   {
      if ((l->menu->name) && (!strcmp(l->menu->name, name)))
         D_RETURN(l->menu);
   }
   D_RETURN(NULL);
}

void
feh_menu_draw_to_buf(feh_menu * m, Imlib_Image im, int ox, int oy)
{
   feh_menu_item *i;
   int w, h;

   D_ENTER;
   w = feh_imlib_image_get_width(im);
   h = feh_imlib_image_get_height(im);

   feh_menu_draw_menu_bg(m, im, ox, oy);

   for (i = m->items; i; i = i->next)
   {
      if (RECTS_INTERSECT(i->x, i->y, i->w, i->h, ox, oy, w, h))
         feh_menu_draw_item(m, i, im, ox, oy);
   }
   D_RETURN_;
}

void
feh_menu_draw_menu_bg(feh_menu * m, Imlib_Image im, int ox, int oy)
{
   int w, h;

   D_ENTER;

   w = feh_imlib_image_get_width(im);
   h = feh_imlib_image_get_height(im);

   if (m->bg)
      feh_imlib_blend_image_onto_image(im, m->bg, 0, ox, oy, w, h, 0, 0, w, h,
                                       0, 0, 0);
   else
      feh_imlib_image_fill_rectangle(im, 0, 0, w, h, 205, 203, 176, 255);

   D_RETURN_;
}

void
feh_menu_draw_submenu_at(int x, int y, int w, int h, Imlib_Image dst, int ox,
                         int oy, int selected)
{
   int x1, y1, x2, y2;

   D_ENTER;
   x -= ox;
   y -= oy;
   x1 = x;
   y1 = y + 2;
   x2 = x + w - 3;
   y2 = y + (h / 2);
   feh_imlib_image_draw_line(dst, x1, y1, x2, y2, 0, 0, 0, 0, 255);
   x1 = x;
   y1 = y + h - 2;
   feh_imlib_image_draw_line(dst, x1, y1, x2, y2, 0, 0, 0, 0, 255);
   D_RETURN_;
   selected = 0;
}


void
feh_menu_draw_separator_at(int x, int y, int w, int h, Imlib_Image dst,
                           int ox, int oy)
{
   D_ENTER;
   feh_imlib_image_fill_rectangle(dst, x - ox + 2, y - oy + 2, w - 4, h - 4,
                                  0, 0, 0, 255);
   D_RETURN_;
}

void
feh_menu_item_draw_at(int x, int y, int w, int h, Imlib_Image dst, int ox,
                      int oy, int selected)
{
   D_ENTER;
   imlib_context_set_image(dst);
   if (selected)
      feh_imlib_image_fill_rectangle(dst, x - ox, y - oy, w, h, 255, 255, 255,
                                     178);
   D_RETURN_;
}


void
feh_raise_all_menus(void)
{
   feh_menu_list *l;

   D_ENTER;

   for (l = menus; l; l = l->next)
   {
      if (l->menu->visible)
         XRaiseWindow(disp, l->menu->win);
   }
   D_RETURN_;
}

void
feh_redraw_menus(void)
{
   feh_menu_list *l;

   D_ENTER;

   for (l = menus; l; l = l->next)
   {
      if (l->menu->needs_redraw)
         feh_menu_redraw(l->menu);
   }

   D_RETURN_;
}

feh_menu *
feh_menu_get_from_window(Window win)
{
   feh_menu_list *l;

   D_ENTER;
   for (l = menus; l; l = l->next)
      if (l->menu->win == win)
         D_RETURN(l->menu);
   D_RETURN(NULL);
}

void
feh_menu_init(void)
{
   feh_menu *m;
   feh_menu_item *mi;

   D_ENTER;
   menu_main = feh_menu_new();
   menu_main->name = estrdup("MAIN");

   feh_menu_add_entry(menu_main, "File", NULL, "FILE", NULL, NULL, NULL);
   if (opt.slideshow || opt.multiwindow)
   {
#if 0
      feh_menu_item *mi;

      mi =
         feh_menu_add_entry(menu_main, "Jump to", NULL, "JUMP", NULL, NULL,
                            NULL);
      mi->func_gen_sub = feh_menu_func_gen_jump;
#endif

      feh_menu_add_entry(menu_main, "Sort Filelist", NULL, "SORT", NULL, NULL,
                         NULL);
      mi =
         feh_menu_add_entry(menu_main, "Image Info", NULL, "INFO", NULL, NULL,
                            NULL);
      mi->func_gen_sub = feh_menu_func_gen_info;
      feh_menu_add_entry(menu_main, NULL, NULL, NULL, NULL, NULL, NULL);
   }
   if (!opt.full_screen)
      feh_menu_add_entry(menu_main, "About " PACKAGE, NULL, NULL,
                         feh_menu_cb_about, NULL, NULL);
   if (opt.multiwindow)
      feh_menu_add_entry(menu_main, "Close", NULL, NULL, feh_menu_cb_close,
                         NULL, NULL);
   feh_menu_add_entry(menu_main, "Exit", NULL, NULL, feh_menu_cb_exit, NULL,
                      NULL);

   m = feh_menu_new();
   m->name = estrdup("FILE");
   if (opt.slideshow || opt.multiwindow)
   {
      feh_menu_add_entry(m, "Reload", NULL, NULL, feh_menu_cb_reload, NULL,
                         NULL);
      feh_menu_add_entry(m, "Remove from filelist", NULL, NULL,
                         feh_menu_cb_remove, NULL, NULL);
      feh_menu_add_entry(m, "Delete", NULL, "CONFIRM", NULL, NULL, NULL);
   }
   feh_menu_add_entry(m, "Background", NULL, "BACKGROUND", NULL, NULL, NULL);

   if (opt.slideshow || opt.multiwindow)
   {
      m = feh_menu_new();
      m->name = estrdup("CONFIRM");
      feh_menu_add_entry(m, "Confirm", NULL, NULL, feh_menu_cb_delete, NULL,
                         NULL);

      m = feh_menu_new();
      m->name = estrdup("SORT");

      feh_menu_add_entry(m, "By filename", NULL, NULL,
                         feh_menu_cb_sort_filename, NULL, NULL);
      feh_menu_add_entry(m, "By image name", NULL, NULL,
                         feh_menu_cb_sort_imagename, NULL, NULL);
      if (opt.preload || (opt.sort > SORT_FILENAME))
         feh_menu_add_entry(m, "By file size", NULL, NULL,
                            feh_menu_cb_sort_filesize, NULL, NULL);
      feh_menu_add_entry(m, "Randomize", NULL, NULL,
                         feh_menu_cb_sort_randomize, NULL, NULL);
   }

   menu_bg = feh_menu_new();
   menu_bg->name = estrdup("BACKGROUND");
   {
      int num_desks, i;
      char buf[30];

      num_desks = feh_wm_get_num_desks();
      if (num_desks > 1)
      {
         feh_menu_add_entry(menu_bg, "Set tiled", NULL, "TILED", NULL, NULL,
                            NULL);
         feh_menu_add_entry(menu_bg, "Set scaled", NULL, "SCALED", NULL, NULL,
                            NULL);
         feh_menu_add_entry(menu_bg, "Set centered", NULL, "CENTERED", NULL,
                            NULL, NULL);

         m = feh_menu_new();
         m->name = estrdup("TILED");
         for (i = 0; i < num_desks; i++)
         {
            snprintf(buf, sizeof(buf), "Desktop %d", i + 1);
            if (opt.slideshow || opt.multiwindow)
               feh_menu_add_entry(m, buf, NULL, NULL,
                                  feh_menu_cb_background_set_tiled, 
                                  (void *) i, NULL);
            else
               feh_menu_add_entry(m, buf, NULL, NULL,
                                  feh_menu_cb_background_set_tiled_no_file,
                                  (void *) i, NULL);
         }

         m = feh_menu_new();
         m->name = estrdup("SCALED");
         for (i = 0; i < num_desks; i++)
         {
            snprintf(buf, sizeof(buf), "Desktop %d", i + 1);

            if (opt.slideshow || opt.multiwindow)
               feh_menu_add_entry(m, buf, NULL, NULL,
                                  feh_menu_cb_background_set_scaled, 
                                  (void *) i, NULL);
            else
               feh_menu_add_entry(m, buf, NULL, NULL,
                                  feh_menu_cb_background_set_scaled_no_file,
                                 (void *) i, NULL);
         }

         m = feh_menu_new();
         m->name = estrdup("CENTERED");
         for (i = 0; i < num_desks; i++)
         {
            snprintf(buf, sizeof(buf), "Desktop %d", i + 1);
            if (opt.slideshow || opt.multiwindow)
               feh_menu_add_entry(m, buf, NULL, NULL,
                                  feh_menu_cb_background_set_centered,
                                  (void *) i, NULL);
            else
               feh_menu_add_entry(m, buf, NULL, NULL,
                                  feh_menu_cb_background_set_centered_no_file,
                                  (void *) i, NULL);
         }
      }
      else
      {
         if (opt.slideshow || opt.multiwindow)
         {
            feh_menu_add_entry(menu_bg, "Set tiled", NULL, NULL,
                               feh_menu_cb_background_set_tiled, NULL, NULL);
            feh_menu_add_entry(menu_bg, "Set scaled", NULL, NULL,
                               feh_menu_cb_background_set_scaled, NULL, NULL);
            feh_menu_add_entry(menu_bg, "Set centered", NULL, NULL,
                               feh_menu_cb_background_set_centered, NULL,
                               NULL);
         }
         else
         {
            feh_menu_add_entry(menu_bg, "Set tiled", NULL, NULL,
                               feh_menu_cb_background_set_tiled_no_file, NULL,
                               NULL);
            feh_menu_add_entry(menu_bg, "Set scaled", NULL, NULL,
                               feh_menu_cb_background_set_scaled_no_file,
                               NULL, NULL);
            feh_menu_add_entry(menu_bg, "Set centered", NULL, NULL,
                               feh_menu_cb_background_set_centered_no_file,
                               NULL, NULL);
         }
      }
   }

   menu_close = feh_menu_new();
   menu_close->name = estrdup("CLOSE");

   feh_menu_add_entry(menu_close, "Close", NULL, NULL, feh_menu_cb_close,
                      NULL, NULL);
   feh_menu_add_entry(menu_close, "Exit", NULL, NULL, feh_menu_cb_exit, NULL,
                      NULL);

   D_RETURN_;
}

static void
feh_menu_cb_background_set_tiled(feh_menu * m, feh_menu_item * i, void *data)
{
   char *path;

   D_ENTER;

   path = feh_absolute_path(m->fehwin->file->filename);
   feh_set_bg(path, m->fehwin->im, 0, 0, (int) data, 1);
   free(path);

   D_RETURN_;
}

static void
feh_menu_cb_background_set_scaled(feh_menu * m, feh_menu_item * i, void *data)
{
   char *path;

   D_ENTER;

   path = feh_absolute_path(m->fehwin->file->filename);
   feh_set_bg(path, m->fehwin->im, 0, 1, (int) data, 1);
   free(path);

   D_RETURN_;
}

static void
feh_menu_cb_background_set_centered(feh_menu * m, feh_menu_item * i,
                                    void *data)
{
   char *path;

   D_ENTER;

   path = feh_absolute_path(m->fehwin->file->filename);
   feh_set_bg(path, m->fehwin->im, 1, 0, (int) data, 1);
   free(path);

   D_RETURN_;
}

static void
feh_menu_cb_background_set_tiled_no_file(feh_menu * m, feh_menu_item * i,
                                         void *data)
{
   char *path;

   D_ENTER;

   feh_set_bg(NULL, m->fehwin->im, 0, 0, (int) data, 1);

   D_RETURN_;
}

static void
feh_menu_cb_background_set_scaled_no_file(feh_menu * m, feh_menu_item * i,
                                          void *data)
{
   char *path;

   D_ENTER;

   feh_set_bg(NULL, m->fehwin->im, 0, 1, (int) data, 1);

   D_RETURN_;
}

static void
feh_menu_cb_background_set_centered_no_file(feh_menu * m, feh_menu_item * i,
                                            void *data)
{
   char *path;

   D_ENTER;

   feh_set_bg(NULL, m->fehwin->im, 1, 0, (int) data, 1);

   D_RETURN_;
}

static void
feh_menu_cb_about(feh_menu * m, feh_menu_item * i, void *data)
{
   Imlib_Image im;
   winwidget winwid;

   D_ENTER;

   imlib_context_set_progress_function(NULL);
   if (feh_load_image_char(&im, PREFIX "/share/feh/images/about.png") != 0)
   {
      winwid =
         winwidget_create_from_image(im, "About " PACKAGE, WIN_TYPE_ABOUT);
      winwid->file = filelist_newitem(PREFIX "/share/feh/images/about.png");
      winwidget_show(winwid);
   }
   D_RETURN_;
   m = NULL;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_close(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   winwidget_destroy(m->fehwin);
   D_RETURN_;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_exit(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   winwidget_destroy_all();
   D_RETURN_;
   m = NULL;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_reload(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   feh_reload_image(m->fehwin);
   D_RETURN_;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_remove(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   feh_filelist_image_remove(m->fehwin, 0);
   D_RETURN_;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_delete(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   feh_filelist_image_remove(m->fehwin, 1);
   D_RETURN_;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_sort_filename(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   filelist = feh_list_sort(filelist, feh_cmp_filename);
   slideshow_change_image(m->fehwin, SLIDE_FIRST);
   D_RETURN_;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_sort_imagename(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   filelist = feh_list_sort(filelist, feh_cmp_name);
   slideshow_change_image(m->fehwin, SLIDE_FIRST);
   D_RETURN_;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_sort_filesize(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   filelist = feh_list_sort(filelist, feh_cmp_size);
   slideshow_change_image(m->fehwin, SLIDE_FIRST);
   D_RETURN_;
   i = NULL;
   data = NULL;
}

static void
feh_menu_cb_sort_randomize(feh_menu * m, feh_menu_item * i, void *data)
{
   D_ENTER;
   filelist = filelist_randomize(filelist);
   slideshow_change_image(m->fehwin, SLIDE_FIRST);
   D_RETURN_;
   i = NULL;
   data = NULL;
}


static feh_menu *
feh_menu_func_gen_jump(feh_menu * m, feh_menu_item * i, void *data)
{
   feh_menu *mm;
   feh_file *file;

   D_ENTER;

   mm = feh_menu_new();
   mm->name = estrdup("JUMP");

   for (file = filelist; file; file = file->next)
   {
      feh_menu_add_entry(mm, file->name, NULL, NULL, feh_menu_cb_jump_to,
                         file, NULL);
   }
   D_RETURN(mm);
   m = NULL;
   i = NULL;
   data = NULL;
}

static feh_menu *
feh_menu_func_gen_info(feh_menu * m, feh_menu_item * i, void *data)
{
   Imlib_Image im;
   feh_menu *mm;
   feh_file *file;
   char buffer[400];

   D_ENTER;

   file = m->fehwin->file;
   im = m->fehwin->im;
   if (!im)
      D_RETURN(NULL);

   mm = feh_menu_new();
   mm->name = estrdup("INFO");

   snprintf(buffer, sizeof(buffer), "Filename: %s", file->name);
   feh_menu_add_entry(mm, buffer, NULL, NULL, NULL, NULL, NULL);

   if (!file->info)
      feh_file_info_load(file, im);

   if (file->info)
   {
      snprintf(buffer, sizeof(buffer), "Size: %dKb", file->info->size / 1024);
      feh_menu_add_entry(mm, buffer, NULL, NULL, NULL, NULL, NULL);

      snprintf(buffer, sizeof(buffer), "Dimensions: %dx%d", file->info->width,
               file->info->height);
      feh_menu_add_entry(mm, buffer, NULL, NULL, NULL, NULL, NULL);

      snprintf(buffer, sizeof(buffer), "Type: %s", file->info->format);
      feh_menu_add_entry(mm, buffer, NULL, NULL, NULL, NULL, NULL);
   }

   mm->func_free = feh_menu_func_free_info;

   D_RETURN(mm);
   i = NULL;
   data = NULL;
}

static void
feh_menu_func_free_info(feh_menu * m, void *data)
{
   D_ENTER;
   feh_menu_free(m);
   D_RETURN_;
   m = NULL;
   data = NULL;
}

static void
feh_menu_cb_jump_to(feh_menu * m, feh_menu_item * i, void *data)
{
   feh_file *file;

   D_ENTER;
   file = (feh_file *) data;
   if (file->prev)
   {
      current_file = file->prev;
      slideshow_change_image(m->fehwin, SLIDE_NEXT);
   }
   else if (file->next)
   {
      current_file = file->next;
      slideshow_change_image(m->fehwin, SLIDE_PREV);
   }

   D_RETURN_;
   i = NULL;
   m = NULL;
}
