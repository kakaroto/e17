#include "config.h"

#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>

#include "game.h"
#include "shoot.h"
#include "explosion.h"
#include "ship.h"


struct _Ship
{
  Game *g;
  Evas_Object *o;
  Explosion *e;
  struct {
    Ship_Upgrade upgrade;
    Evas_Object *front_double;
    Evas_Object *front_triple;
    Evas_Object *horiz_left;
    Evas_Object *horiz_right;
    Evas_Object *side_left;
    Evas_Object *side_right;
    Evas_Object *rear_single;
    Evas_Object *rear_circle;
  } upg;
  Ecore_Timer *timer_up;
  Ecore_Timer *timer_down;
  Ecore_Timer *timer_left;
  Ecore_Timer *timer_right;
  Ecore_Timer *timer_shoot;
  double interval_move;
  double interval_shoot;
  Eina_Bool up_first : 1;
  Eina_Bool down_first : 1;
  Eina_Bool left_first : 1;
  Eina_Bool right_first : 1;
  Eina_Bool shoot_first : 1;
};


static void
_ship_move(Ship *s, int dx, int dy)
{
  Evas_Coord x;
  Evas_Coord y;

  evas_object_geometry_get(s->o, &x, &y, NULL, NULL);
  evas_object_move(s->o, x + dx, y + dy);

  if (s->upg.front_double && (s->upg.upgrade & SHIP_UPGRADE_FRONT_DOUBLE))
    {
      evas_object_geometry_get(s->upg.front_double, &x, &y, NULL, NULL);
      evas_object_move(s->upg.front_double, x + dx, y + dy);
    }

  if (s->upg.front_triple && (s->upg.upgrade & SHIP_UPGRADE_FRONT_TRIPLE))
    {
      evas_object_geometry_get(s->upg.front_triple, &x, &y, NULL, NULL);
      evas_object_move(s->upg.front_triple, x + dx, y + dy);
    }

  if (s->upg.rear_single && (s->upg.upgrade & SHIP_UPGRADE_REAR_SINGLE))
    {
      evas_object_geometry_get(s->upg.rear_single, &x, &y, NULL, NULL);
      evas_object_move(s->upg.rear_single, x + dx, y + dy);
    }

  if (s->upg.rear_circle && (s->upg.upgrade & SHIP_UPGRADE_REAR_CIRCLE))
    {
      evas_object_geometry_get(s->upg.rear_circle, &x, &y, NULL, NULL);
      evas_object_move(s->upg.rear_circle, x + dx, y + dy);
    }
}

static Eina_Bool
_ship_move_up_cb(void *data)
{
  Ship *s = (Ship *)data;

  _ship_move(s, 0, -2);
  return EINA_TRUE;
}

static Eina_Bool
_ship_move_down_cb(void *data)
{
  Ship *s = (Ship *)data;

  _ship_move(s, 0, 2);
  return EINA_TRUE;
}

static Eina_Bool
_ship_move_left_cb(void *data)
{
  Ship *s = (Ship *)data;

  _ship_move(s, -2, 0);
  return EINA_TRUE;
}

static Eina_Bool
_ship_move_right_cb(void *data)
{
  Ship *s = (Ship *)data;

  _ship_move(s, 2, 0);
  return EINA_TRUE;
}

static Eina_Bool
_ship_shoot_cb(void *data)
{
  Ship *s = (Ship *)data;
  Shoot *shoot;

  if (s->upg.upgrade & SHIP_UPGRADE_FRONT_SINGLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;
      Evas_Coord sw;
      Evas_Coord sh;

      evas_object_geometry_get(s->o, &x, &y, &w, &h);
      shoot = shoot_new(s->g, SHOOT_DIR_UP_SHIP, &sw, &sh);
      shoot_start(shoot, x + (w - sw) / 2, y - sh, 0.005);
    }

  if (s->upg.upgrade & SHIP_UPGRADE_FRONT_DOUBLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;
      Evas_Coord sw;
      Evas_Coord sh;

      evas_object_geometry_get(s->upg.front_double, &x, &y, &w, &h);
      shoot = shoot_new(s->g, SHOOT_DIR_UP, &sw, &sh);
      shoot_start(shoot, x, y - sh, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_UP, &sw, &sh);
      shoot_start(shoot, x + w - sw, y - sh, 0.005);
    }

  if (s->upg.upgrade & SHIP_UPGRADE_FRONT_TRIPLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;
      Evas_Coord sw;
      Evas_Coord sh;

      evas_object_geometry_get(s->upg.front_triple, &x, &y, &w, &h);
      shoot = shoot_new(s->g, SHOOT_DIR_UP, &sw, &sh);
      shoot_start(shoot, x + (w - sw) / 2, y - sh, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_TOP_LEFT, &sw, &sh);
      shoot_start(shoot, x, y - sh, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_TOP_RIGHT, &sw, &sh);
      shoot_start(shoot, x + w - sw, y - sh, 0.005);
    }

  if (s->upg.upgrade & SHIP_UPGRADE_REAR_SINGLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;
      Evas_Coord sw;
      Evas_Coord sh;

      evas_object_geometry_get(s->upg.rear_single, &x, &y, &w, &h);
      shoot = shoot_new(s->g, SHOOT_DIR_DOWN, &sw, &sh);
      shoot_start(shoot, x + (w - sw) / 2, y, 0.005);
    }

  if (s->upg.upgrade & SHIP_UPGRADE_REAR_CIRCLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;
      Evas_Coord sw;
      Evas_Coord sh;

      evas_object_geometry_get(s->upg.rear_circle, &x, &y, &w, &h);
      shoot = shoot_new(s->g, SHOOT_DIR_UP, &sw, &sh);
      shoot_start(shoot, x + (w - sw) / 2, y - sh, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_DOWN, &sw, &sh);
      shoot_start(shoot, x + (w - sw) / 2, y, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_TOP_LEFT, &sw, &sh);
      shoot_start(shoot, x, y - sh, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_TOP_RIGHT, &sw, &sh);
      shoot_start(shoot, x + w - sw, y - sh, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_LEFT, &sw, &sh);
      shoot_start(shoot, x - sw, y + (h - sh) / 2, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_RIGHT, &sw, &sh);
      shoot_start(shoot, x + sw, y + (h - sh) / 2, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_BOTTOM_LEFT, &sw, &sh);
      shoot_start(shoot, x, y + h, 0.005);
      shoot = shoot_new(s->g, SHOOT_DIR_BOTTOM_RIGHT, &sw, &sh);
      shoot_start(shoot, x + sw, y + h, 0.005);
    }

  return EINA_TRUE;
}

static void
_ship_key_down_cb(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Ship *s = (Ship *)data;
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down *)event_info;

  if (strcmp(ev->keyname, "space") == 0)
    {
      if (s->shoot_first)
        {
          s->shoot_first = EINA_FALSE;
          _ship_shoot_cb(s);
        }
      if (!s->timer_shoot)
        s->timer_shoot = ecore_timer_add(0.3, _ship_shoot_cb, s);
    }

  if (strcmp(ev->keyname, "Up") == 0)
    {
      if (s->up_first)
        {
          s->up_first = EINA_FALSE;
          _ship_move_up_cb(s);
        }
      if (!s->timer_up)
        s->timer_up = ecore_timer_add(0.01, _ship_move_up_cb, s);
    }

  if (strcmp(ev->keyname, "Down") == 0)
    {
      if (s->down_first)
        {
          s->down_first = EINA_FALSE;
          _ship_move_down_cb(s);
        }
      if (!s->timer_down)
        s->timer_down = ecore_timer_add(0.01, _ship_move_down_cb, s);
    }

  if (strcmp(ev->keyname, "Left") == 0)
    {
      if (s->left_first)
        {
          s->left_first = EINA_FALSE;
          _ship_move_left_cb(s);
        }
      if (!s->timer_left)
        s->timer_left = ecore_timer_add(0.01, _ship_move_left_cb, s);
    }

  if (strcmp(ev->keyname, "Right") == 0)
    {
      if (s->right_first)
        {
          s->right_first = EINA_FALSE;
          _ship_move_right_cb(s);
        }
      if (!s->timer_right)
        s->timer_right = ecore_timer_add(0.01, _ship_move_right_cb, s);
    }
}

static void
_ship_key_up_cb(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Ship *s = (Ship *)data;
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down *)event_info;

  if (strcmp(ev->keyname, "space") == 0)
    {
      s->shoot_first = EINA_TRUE;
      if (s->timer_shoot)
        {
          ecore_timer_del(s->timer_shoot);
          s->timer_shoot = NULL;
        }
    }

  if (strcmp(ev->keyname, "Up") == 0)
    {
      s->up_first = EINA_TRUE;
      if (s->timer_up)
        {
          ecore_timer_del(s->timer_up);
          s->timer_up = NULL;
        }
    }

  if (strcmp(ev->keyname, "Down") == 0)
    {
      s->down_first = EINA_TRUE;
      if (s->timer_down)
        {
          ecore_timer_del(s->timer_down);
          s->timer_down = NULL;
        }
    }

  if (strcmp(ev->keyname, "Left") == 0)
    {
      s->left_first = EINA_TRUE;
      if (s->timer_left)
        {
          ecore_timer_del(s->timer_left);
          s->timer_left = NULL;
        }
    }

  if (strcmp(ev->keyname, "Right") == 0)
    {
      s->right_first = EINA_TRUE;
      if (s->timer_right)
        {
          ecore_timer_del(s->timer_right);
          s->timer_right = NULL;
        }
    }
}


Ship *
ship_new(Game *g)
{
  Ship *s;
  Evas_Coord w;
  Evas_Coord h;
  Evas_Coord gw;
  Evas_Coord gh;

  s = (Ship *)calloc(1, sizeof(Ship));
  if (!s) return NULL;

  s->up_first = EINA_TRUE;
  s->down_first = EINA_TRUE;
  s->left_first = EINA_TRUE;
  s->right_first = EINA_TRUE;
  s->shoot_first = EINA_TRUE;
  s->g = g;
  game_size_get(g, &gw, &gh);

  s->o = edje_object_add(game_evas_get(g));
  if (!edje_object_file_set(s->o, "theme.edj", "ship"))
    goto free_s;
  edje_object_size_min_get(s->o, &w, &h);
  w *= 2;
  h *= 2;
  evas_object_resize(s->o, w, h);
  evas_object_move(s->o, (gw - w) / 2, gh - h);
  evas_object_layer_set(s->o, 10);
  evas_object_show(s->o);

  s->interval_move = atof(edje_object_data_get(s->o, "interval_move"));
  s->interval_shoot = atof(edje_object_data_get(s->o, "interval_shoot"));

  evas_object_event_callback_add(s->o, EVAS_CALLBACK_KEY_DOWN, _ship_key_down_cb, s);
  evas_object_event_callback_add(s->o, EVAS_CALLBACK_KEY_UP, _ship_key_up_cb, s);
  evas_object_focus_set(s->o, EINA_TRUE);

  s->upg.upgrade = SHIP_UPGRADE_FRONT_SINGLE;
  s->e = explosion_new(game_evas_get(g),
                       edje_object_data_get(s->o, "explosion"),
                       s->o);
  if (!s->e)
    goto free_s_o;

  return s;

 free_s_o:
  evas_object_del(s->o);
 free_s:
  free(s);
  return NULL;
}

void
ship_free(Ship *s)
{
  if (!s) return;

  evas_object_del(s->o);
  explosion_free(s->e);
  free(s);
}

void
ship_upgrade_set(Ship *s, Ship_Upgrade upg)
{
  if (!s) return;

  if (upg == SHIP_UPGRADE_FRONT_DOUBLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;

      s->upg.upgrade &= ~(SHIP_UPGRADE_FRONT_SINGLE | SHIP_UPGRADE_FRONT_TRIPLE);
      s->upg.upgrade |= upg;
      evas_object_geometry_get(s->o, &x, &y, &w, &h);

      if (s->upg.front_triple)
        evas_object_hide(s->upg.front_triple);

      if (!s->upg.front_double)
        s->upg.front_double = evas_object_rectangle_add(game_evas_get(s->g));
      evas_object_geometry_get(s->o, &x, &y, &w, &h);
      evas_object_resize(s->upg.front_double, 2 * w / 3, 20);
      evas_object_color_set(s->upg.front_double, 0, 0, 255, 255);
      evas_object_move(s->upg.front_double, x + (w / 6), y - 20);
      evas_object_layer_set(s->upg.front_double, 10);
      evas_object_show(s->upg.front_double);
    }
  else if (upg == SHIP_UPGRADE_FRONT_TRIPLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;

      s->upg.upgrade &= ~(SHIP_UPGRADE_FRONT_SINGLE | SHIP_UPGRADE_FRONT_DOUBLE);
      s->upg.upgrade |= SHIP_UPGRADE_FRONT_TRIPLE;

      if (s->upg.front_double)
        evas_object_hide(s->upg.front_double);

      if (!s->upg.front_triple)
        s->upg.front_triple = evas_object_rectangle_add(game_evas_get(s->g));
      evas_object_geometry_get(s->o, &x, &y, &w, &h);
      evas_object_resize(s->upg.front_triple, w / 2, 20);
      evas_object_color_set(s->upg.front_triple, 0, 255, 0, 255);
      evas_object_move(s->upg.front_triple, x + (w / 4), y - 20);
      evas_object_layer_set(s->upg.front_triple, 10);
      evas_object_show(s->upg.front_triple);
    }
  else if (upg == SHIP_UPGRADE_REAR_SINGLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;

      s->upg.upgrade &= ~SHIP_UPGRADE_REAR_CIRCLE;
      s->upg.upgrade |= SHIP_UPGRADE_REAR_SINGLE;

      if (s->upg.rear_circle)
        evas_object_hide(s->upg.rear_circle);

      if (!s->upg.rear_single)
        s->upg.rear_single = evas_object_rectangle_add(game_evas_get(s->g));

      evas_object_geometry_get(s->o, &x, &y, &w, &h);
      evas_object_resize(s->upg.rear_single, 1 * w / 5, 25);
      evas_object_color_set(s->upg.rear_single, 0, 255, 0, 255);
      evas_object_move(s->upg.rear_single, x + (2 * w / 5), y + h);
      evas_object_layer_set(s->upg.rear_single, 10);
      evas_object_show(s->upg.rear_single);
    }
  else if (upg == SHIP_UPGRADE_REAR_CIRCLE)
    {
      Evas_Coord x;
      Evas_Coord y;
      Evas_Coord w;
      Evas_Coord h;

      s->upg.upgrade &= ~SHIP_UPGRADE_REAR_SINGLE;
      s->upg.upgrade |= SHIP_UPGRADE_REAR_CIRCLE;

      if (s->upg.rear_single)
        evas_object_hide(s->upg.rear_single);

      if (!s->upg.rear_circle)
        s->upg.rear_circle = evas_object_rectangle_add(game_evas_get(s->g));

      evas_object_geometry_get(s->o, &x, &y, &w, &h);
      evas_object_resize(s->upg.rear_circle, 20, 20);
      evas_object_color_set(s->upg.rear_circle, 0, 0, 255, 255);
      evas_object_move(s->upg.rear_circle, x + (w - 20) / 2, y + h + 10);
      evas_object_layer_set(s->upg.rear_circle, 10);
      evas_object_show(s->upg.rear_circle);
    }
  else
    s->upg.upgrade |= upg;
}

void
ship_upgrade_unset(Ship *s, Ship_Upgrade upg)
{
  if (!s) return;

  if ((upg == SHIP_UPGRADE_FRONT_TRIPLE) || (upg == SHIP_UPGRADE_FRONT_DOUBLE))
    {
      s->upg.upgrade &= ~(SHIP_UPGRADE_FRONT_DOUBLE | SHIP_UPGRADE_FRONT_TRIPLE);
      s->upg.upgrade |= SHIP_UPGRADE_FRONT_SINGLE;
    }
  else if ((upg == SHIP_UPGRADE_REAR_CIRCLE) || (upg == SHIP_UPGRADE_REAR_SINGLE))
    s->upg.upgrade &= ~(SHIP_UPGRADE_REAR_CIRCLE | SHIP_UPGRADE_REAR_SINGLE);
  else
    s->upg.upgrade &= ~upg;
}

Eina_Bool
ship_explode(Ship *s, Evas_Coord hot_x, Evas_Coord hot_y)
{
  if (!s) return EINA_FALSE;

  if (explosion_launch(s->e, hot_x, hot_y))
    {
/*       alien_free(a); */
      return EINA_TRUE;
    }

  return EINA_FALSE;
}

void
ship_geometry_get(Ship *s, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
  evas_object_geometry_get(s->o, x, y, w, h);
}
