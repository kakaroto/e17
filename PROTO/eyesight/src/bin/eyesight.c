/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010-2011 Vincent Torri <vtorri at univ-evry dot fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "Eyesight.h"
#include "Eyesight_Module_Img.h"
#include "Eyesight_Module_Pdf.h"
#include "Eyesight_Module_Ps.h"
#include "Eyesight_Module_Txt.h"

#include "eyesight_popup.h"

static int page_nbr = 0;

static void _cb_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Popup *p;
  Evas_Event_Key_Up *ev;

  p = (Popup *)data;
  ev = (Evas_Event_Key_Up *)event_info;

  if (strcmp(ev->keyname, "q") == 0)
    {
      ecore_main_loop_quit();
      return;
    }

  if (strcmp(ev->keyname, "Right") == 0)
    {
      if (page_nbr < (eyesight_object_page_count(obj) - 1))
        page_nbr++;
    }

  if (strcmp(ev->keyname, "Left") == 0)
    {
      if (page_nbr > 0)
        page_nbr--;
    }

  if (page_nbr != eyesight_object_page_get(obj))
    {
      char buf[16];
      double t0, t1;

      eyesight_object_page_set(obj, page_nbr);
      t0 = ecore_time_get();
      eyesight_object_page_render(obj);
      t1 = ecore_time_get();
      printf("time (p=%d): %E\n", page_nbr, t1-t0);

      snprintf(buf, 15, "%d/%d", page_nbr + 1, eyesight_object_page_count(obj));
      popup_text_set(p, buf);
      popup_timer_start(p);
    }
}

static void _cb_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Move *ev;
  Eina_List *l;
  void      *d;

  if (!data)
    return;

  ev = (Evas_Event_Mouse_Move *)event_info;

  EINA_LIST_FOREACH((Eina_List *)data, l, d)
    {
      Eyesight_Link *link = (Eyesight_Link *)d;
      Eina_Rectangle rect;

      rect = link->rect;
      if (eina_rectangle_coords_inside(&rect, ev->cur.output.x, ev->cur.output.y))
        {
          printf("yes !\n");
        }
    }
}

static void _cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Up *ev;
  Eina_List *l;
  void      *d;

  if (!data)
    return;

  ev = (Evas_Event_Mouse_Up *)event_info;

  EINA_LIST_FOREACH((Eina_List *)data, l, d)
    {
      Eyesight_Link *link = (Eyesight_Link *)d;
      Eina_Rectangle rect;

      rect = link->rect;
      if (eina_rectangle_coords_inside(&rect, ev->output.x, ev->output.y))
        {
          if (link->action == EYESIGHT_LINK_ACTION_GOTO)
            {
              eyesight_object_page_set(obj, link->dest.action_goto.page);
              eyesight_object_page_render(obj);
            }
          if ((link->action == EYESIGHT_LINK_ACTION_URI) &&
              (link->dest.action_uri.uri))
            {
              char *startup = "xdg-open ";
              char *command;
              size_t l1;
              size_t l2;

              l1 = strlen(startup);
              l2 = strlen(link->dest.action_uri.uri);
              command = (char *)malloc(sizeof(char) * (l1 + l2 + 1));
              memcpy(command, startup, l1);
              memcpy(command + l1, link->dest.action_uri.uri, l2);
              command[l1+l2] = '\0';
              printf("command : %s\n", command);
              if (system(command) == -1)
                {
                  printf("Can not run : %s\n", command);
                }
              free(command);
            }
        }
    }
}

static int indent = -2;

static void _display_toc(const Eina_List *items)
{
  Eina_List *l;
  void      *data;

  if (!items) return;

  indent += 2;

  EINA_LIST_FOREACH((Eina_List *)items, l, data)
    {
      Eyesight_Index_Item *item = (Eyesight_Index_Item *)data;
      int i;
      for (i = 0; i < indent; i++) printf (" ");
      printf ("(%d) %s: %d\n",
              eyesight_index_item_page_get(item),
              eyesight_index_item_title_get(item),
              eyesight_index_item_action_kind_get(item));
      _display_toc(eyesight_index_item_children_get(item));
    }

  indent -= 2;
}

static void _display_text(Eina_List *rects)
{
  Eina_List *l;
  void      *data;

  if (!rects) return;

  EINA_LIST_FOREACH(rects, l, data)
    {
      Eina_Rectangle *rect = (Eina_Rectangle *)data;
      printf ("(%d, %d, %d, %d)\n",
              rect->x, rect->y, rect->w, rect->h);
    }

  indent -= 2;
}

static void _display_links(Evas_Object *obj, Eina_List *links)
{
  Eina_List *l;
  void      *data;

  if (!links) return;

  EINA_LIST_FOREACH(links, l, data)
    {
      Eyesight_Link *link = (Eyesight_Link *)data;
      Evas_Object *o;

      o = evas_object_rectangle_add(evas_object_evas_get(obj));
      evas_object_move (o, link->rect.x, link->rect.y);
      evas_object_resize (o, link->rect.w, link->rect.h);
      evas_object_color_set (o, 0, 0, 255, 128);
      evas_object_show (o);
      evas_object_pass_events_set(o, EINA_TRUE);
    }
}

int main(int argc, char *argv[])
{
  Ecore_Evas *ee;
  Evas *evas;
  Evas_Object *bg;
  Evas_Object *o;
  int w, h;
  char *backend = NULL;
  Eina_List *links = NULL;
  Eyesight_Backend eb;
  void *doc;
  Popup *p;

  if (argc < 1)
    {
      printf("Usage: %s filename\n", argv[0]);
      return -1;
    }

  backend = strrchr(argv[1], '.');
  if (!backend)
    {
      printf ("no suffix to file, exiting...\n");
      return -1;
    }
  backend++;
  if (strcmp(backend, "eps") == 0)
    backend = "ps";
  if ((strcmp(backend, "bmp") == 0) ||
      (strcmp(backend, "gif") == 0) ||
      (strcmp(backend, "jpg") == 0) ||
      (strcmp(backend, "jpeg") == 0) ||
      (strcmp(backend, "png") == 0) ||
      (strcmp(backend, "ppm") == 0) ||
      (strcmp(backend, "tga") == 0) ||
      (strcmp(backend, "tiff") == 0) ||
      (strcmp(backend, "xpm") == 0) ||
      (strcmp(backend, "cba") == 0) ||
      (strcmp(backend, "cbr") == 0) ||
      (strcmp(backend, "cbt") == 0) ||
      (strcmp(backend, "cbz") == 0) ||
      (strcmp(backend, "cb7") == 0))
    backend = "img";
  printf ("%s\n", backend);

  ecore_evas_init();

  ee = ecore_evas_new(NULL, 10, 10, 1, 1, NULL);
  if (!ee)
    return -1;

  evas = ecore_evas_get(ee);

  /* background */
  o = evas_object_rectangle_add(evas);
  evas_object_color_set(o, 255, 255, 255, 255);
  evas_object_move(o, 0, 0);
  evas_object_show(o);
  bg = o;

  o = eyesight_object_add(evas);
  if (!(eb = eyesight_object_init(o, backend)))
    {
      printf("erreur init backend\n");
      ecore_evas_shutdown();
      return -1;
    }
  if (!(doc = eyesight_object_file_set(o, argv[1])))
    {
      printf("erreur file_set\n");
      ecore_evas_shutdown();
      return -1;
    }

  /* Specific information */

  switch (eb)
    {
    case EYESIGHT_BACKEND_IMG:
      printf("Image format.............: ");
      switch (((Eyesight_Document_Img *)doc)->archive)
        {
        case EYESIGHT_IMG_ARCHIVE_NONE:
          printf ("single image\n");
          break;
        case EYESIGHT_IMG_ARCHIVE_CBA:
          printf ("comic book (ACE)\n");
          break;
        case EYESIGHT_IMG_ARCHIVE_CBR:
          printf ("comic book (RAR)\n");
          break;
        case EYESIGHT_IMG_ARCHIVE_CBT:
          printf ("comic book (TAR)\n");
          break;
        case EYESIGHT_IMG_ARCHIVE_CBZ:
          printf ("comic book (ZIP)\n");
          break;
        case EYESIGHT_IMG_ARCHIVE_CB7:
          printf ("comic book (7z)\n");
          break;
        }
      break;
    case EYESIGHT_BACKEND_PDF:
      printf("Version..................: %d.%d\n",
             ((Eyesight_Document_Pdf *)doc)->version_maj,
             ((Eyesight_Document_Pdf *)doc)->version_min);
      printf("Title....................: %s\n", ((Eyesight_Document_Pdf *)doc)->title);
      printf("Author...................: %s\n", ((Eyesight_Document_Pdf *)doc)->author);
      printf("Subject..................: %s\n", ((Eyesight_Document_Pdf *)doc)->subject);
      printf("Keywords.................: %s\n", ((Eyesight_Document_Pdf *)doc)->keywords);
      printf("Creator..................: %s\n", ((Eyesight_Document_Pdf *)doc)->creator);
      printf("Producer.................: %s\n", ((Eyesight_Document_Pdf *)doc)->producer);
      printf("Date creation............: %s\n", ((Eyesight_Document_Pdf *)doc)->date_creation);
      printf("Date modification........: %s\n", ((Eyesight_Document_Pdf *)doc)->date_modification);
      printf("Mode.....................: %d\n", ((Eyesight_Document_Pdf *)doc)->mode);
      printf("Layout...................: %d\n", ((Eyesight_Document_Pdf *)doc)->layout);
      printf("Locked...................: %s\n", ((Eyesight_Document_Pdf *)doc)->locked ? "yes" : "no");
      printf("Encrypted................: %s\n", ((Eyesight_Document_Pdf *)doc)->encrypted ? "yes" : "no");
      printf("Linearized...............: %s\n", ((Eyesight_Document_Pdf *)doc)->linearized ? "yes" : "no");
      printf("Printable................: %s\n", ((Eyesight_Document_Pdf *)doc)->printable ? "yes" : "no");
      printf("Changeable...............: %s\n", ((Eyesight_Document_Pdf *)doc)->changeable ? "yes" : "no");
      printf("Copyable.................: %s\n", ((Eyesight_Document_Pdf *)doc)->copyable ? "yes" : "no");
      printf("Notable..................: %s\n", ((Eyesight_Document_Pdf *)doc)->notable ? "yes" : "no");
      break;
    case EYESIGHT_BACKEND_PS:
      printf("Title....................: %s\n", ((Eyesight_Document_Ps *)doc)->title);
      printf("Author...................: %s\n", ((Eyesight_Document_Ps *)doc)->author);
      printf("For......................: %s\n", ((Eyesight_Document_Ps *)doc)->for_);
      printf("Format...................: %s\n", ((Eyesight_Document_Ps *)doc)->format);
      printf("Date creation............: %s\n", ((Eyesight_Document_Ps *)doc)->date_creation);
      printf("Language level...........: %d\n", ((Eyesight_Document_Ps *)doc)->language_level);
      printf("EPS......................: %s\n", ((Eyesight_Document_Ps *)doc)->is_eps ? "yes" : "no");
      break;
    case EYESIGHT_BACKEND_TXT:
      break;
    }

  printf ("page count: %d\n", eyesight_object_page_count(o));
  evas_object_move (o, 0, 0);
  eyesight_object_page_render (o);
  evas_object_geometry_get(o, NULL, NULL, &w, &h);
  printf ("%d %d\n", w, h);
  evas_object_focus_set(o, EINA_TRUE);
  links = eyesight_object_page_links_get(o);

  p = popup_new(evas);
  if (!popup_background_set(p, "gradient.png"))
    {
      popup_free(p);
      p = NULL;
    }

  evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_UP, _cb_up, p);
  evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _cb_move, links);
  evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _cb_mouse_up, links);
  evas_object_show (o);

  evas_object_resize(bg, w, h);
  ecore_evas_resize(ee, w, h);
  ecore_evas_show(ee);

  _display_toc(eyesight_object_toc_get(o));
/*   _display_text(eyesight_object_page_text_find(o, "prix", EINA_FALSE, EINA_FALSE)); */
  _display_links(o, links);

  ecore_main_loop_begin();

  popup_free(p);
  ecore_evas_shutdown();

  return 0;
}
