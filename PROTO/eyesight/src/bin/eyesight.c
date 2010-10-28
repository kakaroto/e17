#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "Eyesight.h"

static int page_nbr = 0;

static void _cb_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Key_Up *ev;

  ev = (Evas_Event_Key_Up *)event_info;
  if (strcmp(ev->keyname, "q") == 0)
    {
      ecore_main_loop_quit();
      return;
    }
  if (strcmp(ev->keyname, "Right") == 0) page_nbr++;
  if (strcmp(ev->keyname, "Left") == 0) page_nbr--;
  if (page_nbr < 0) page_nbr = 0;
  if (page_nbr >= eyesight_object_page_count(obj)) page_nbr = eyesight_object_page_count(obj) - 1;
  if (page_nbr != eyesight_object_page_get(obj))
    {
      eyesight_object_page_set(obj, page_nbr);
      eyesight_object_page_render(obj);
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

static void _display_toc(Eina_List *items)
{
  Eina_List *l;
  void      *data;

  if (!items) return;

  indent += 2;

  EINA_LIST_FOREACH(items, l, data)
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
  Evas_Object *o;
  int w, h;
  char *backend = NULL;
  Eina_List *links = NULL;

  if (argc < 2)
    {
      printf("argc < 2\n");
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

  o = eyesight_object_add(evas);
  if (!eyesight_object_init(o, backend))
    {
      printf("erreur init backend\n");
      ecore_evas_shutdown();
      return -1;
    }
  if (!eyesight_object_file_set(o, argv[1]))
    {
      printf("erreur file_set\n");
      ecore_evas_shutdown();
      return -1;
    }
  printf ("page count: %d\n", eyesight_object_page_count(o));
  evas_object_move (o, 0, 0);
  eyesight_object_page_render (o);
  evas_object_geometry_get(o, NULL, NULL, &w, &h);
  printf ("%d %d\n", w, h);
  evas_object_focus_set(o, EINA_TRUE);
  links = eyesight_object_page_links_get(o);
  evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_UP, _cb_up, NULL);
  evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _cb_move, links);
  evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _cb_mouse_up, links);
  evas_object_show (o);

  ecore_evas_resize(ee, w, h);
  ecore_evas_show(ee);

/*   _display_toc(eyesight_object_toc_get(o)); */
/*   _display_text(eyesight_object_page_text_find(o, "prix", EINA_FALSE, EINA_FALSE)); */
  _display_links(o, links);

  ecore_main_loop_begin();

  ecore_evas_shutdown();

  return 0;
}
