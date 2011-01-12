/* gcc -g -Wall -W -Wextra -Wshadow -o etype main.c game.c ship.c shoot.c stars.c scrolling.c alien.c `pkg-config --cflags --libs ecore-evas` */

#include "config.h"

#include <stdlib.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "game.h"


static void
_app_quit_cb(Ecore_Evas *ee __UNUSED__)
{
  ecore_main_loop_quit();
}

/* static void */
/* _app_key_up_cb(void *data, Evas *e, void *event_info) */
/* { */
/*   Evas_Event_Key_Up *ev = (Evas_Event_Key_Up *)event_info; */

/*   printf("up\n"); */
/*   if (strcmp(ev->keyname, "q") == 0) */
/*     { */
/*       printf("q\n"); */
/*     } */

/*   if (strcmp(ev->keyname, "Q") == 0) */
/*     { */
/*       printf("Q\n"); */
/*     } */
/* } */


int main()
{
  Ecore_Evas *ee;
  Evas *evas;
  Game *g;
  Evas_Coord w;
  Evas_Coord h;

  if (!ecore_evas_init())
    return EXIT_FAILURE;

  if (!edje_init())
    goto shutdown_ecore_evas;

  ee = ecore_evas_new(NULL, 10, 10, 1, 1, NULL);
  if (!ee)
    goto shutdown_edje;
  ecore_evas_callback_delete_request_set(ee, _app_quit_cb);

  evas = ecore_evas_get(ee);
/*   evas_event_callback_add(evas, EVAS_CALLBACK_KEY_UP, _app_key_up_cb, NULL); */

  g = game_new(evas);
  if (!g) goto free_ee;

  game_size_get(g, &w, &h);
  ecore_evas_resize(ee, w, h);
  ecore_evas_show(ee);

  ecore_main_loop_begin();

  game_free(g);
  ecore_evas_free(ee);
  edje_shutdown();
  ecore_evas_shutdown();

  return EXIT_SUCCESS;

 free_ee:
  ecore_evas_free(ee);
 shutdown_edje:
  edje_shutdown();
 shutdown_ecore_evas:
  ecore_evas_shutdown();
  return EXIT_FAILURE;
}
