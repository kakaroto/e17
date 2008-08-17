/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "trivials.h"
#include "config.h"
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "e_border.h"
#include "e_shelf.h"
#include <math.h>

/* Use TILING_DEBUG-define to toggle displaying lots of debugmessages */
#define TILING_DEBUG

/***************************************************************************/
/* actual module specifics */

E_Module *tiling_module = NULL;
Config *tiling_config;
E_Config_DD *tiling_config_edd,
	    *vdesk_edd;
static E_Border_Hook *hook = NULL;
static Ecore_Event_Handler *handler_hide = NULL,
			   *handler_desk_show = NULL,
			   *handler_desk_before_show = NULL,
			   *handler_mouse_move = NULL,
			   *handler_desk_set = NULL;
static E_Zone *current_zone = NULL;
static Tiling_Info *tinfo = NULL;
static E_Action *act_toggletiling = NULL,
		*act_togglefloat = NULL,
		*act_switchtiling = NULL,
		*act_moveleft = NULL,
		*act_moveright = NULL,
		*act_movetop = NULL,
		*act_movebottom = NULL;
static int currently_switching_desktop = 0;

/* This hash holds the Tiling_Info-pointers for each desktop */
static Evas_Hash *info_hash = NULL;

static void _e_mod_action_toggle_tiling_cb(E_Object *obj, const char *params);
static void _e_mod_action_toggle_floating_cb(E_Object *obj, const char *params);
static void _e_mod_action_switch_tiling_cb(E_Object *obj, const char *params);
static void _e_mod_action_move_left(E_Object *obj, const char *params);
static void _e_mod_action_move_right(E_Object *obj, const char *params);
static void _e_mod_action_move_top(E_Object *obj, const char *params);
static void _e_mod_action_move_bottom(E_Object *obj, const char *params);
static E_Border *get_first_window(E_Border *exclude, E_Desk *desk);
static void toggle_floating(E_Border *bd);
static int check_for_too_big_windows(int width, int height, E_Border *bd);
static void rearrange_windows(E_Border *bd, int remove_bd);
static void _desk_show(E_Desk *desk);

#define EVAS_LIST_FOREACH(list)	for (l = list; l; l = l->next) \
				  { \
				     E_Border *lbd = l->data; \
				     if (!lbd) continue;

#define TILE_LOOP_EBORDER_STACK	EVAS_LIST_FOREACH(e_border_client_list())

#define TILE_LOOP_DESKCHECK	if ((lbd->desk != bd->desk) || (lbd->zone != bd->zone)) continue;

#define TILE_LOOP_CHECKS(lbd)	((tinfo && evas_list_find(tinfo->floating_windows, lbd) == lbd) || \
				 (lbd->visible == 0) || \
				 (!tiling_config->tile_dialogs && \
                                  ((lbd->client.icccm.transient_for != 0) || \
                                  (lbd->client.netwm.type == ECORE_X_WINDOW_TYPE_DIALOG))))

#define TILE_STDLOOP		EVAS_LIST_FOREACH(e_border_focus_stack_get()) \
				TILE_LOOP_DESKCHECK \
				if (TILE_LOOP_CHECKS(lbd)) continue;

#define TILE_STDBLOOP		EVAS_LIST_FOREACH(tinfo->client_list) \
				TILE_LOOP_DESKCHECK \
				if (TILE_LOOP_CHECKS(lbd)) continue;

#define TILE_STDSLOOP		for (s = e_shelf_list(); s; s = s->next) \
                                  { \
                                    E_Shelf *sh = s->data; \
                                    if (!sh || \
					(sh->zone != bd->zone) || \
					!shelf_show_on_desk(sh, bd->desk) || \
					sh->cfg->overlap) continue; \


#define ORIENT_BOTTOM(x)	((x == E_GADCON_ORIENT_CORNER_BL) || \
				 (x == E_GADCON_ORIENT_CORNER_BR) || \
				 (x == E_GADCON_ORIENT_BOTTOM))

#define ORIENT_TOP(x)		((x == E_GADCON_ORIENT_CORNER_TL) || \
				 (x == E_GADCON_ORIENT_CORNER_TR) || \
				 (x == E_GADCON_ORIENT_TOP))

#define ORIENT_LEFT(x)		((x == E_GADCON_ORIENT_CORNER_LB) || \
				 (x == E_GADCON_ORIENT_CORNER_LT) || \
				 (x == E_GADCON_ORIENT_LEFT))

#define ORIENT_RIGHT(x)		((x == E_GADCON_ORIENT_CORNER_RB) || \
				 (x == E_GADCON_ORIENT_CORNER_RT) || \
				 (x == E_GADCON_ORIENT_RIGHT))

#define LOOP_END	}
/* The following is only there to fix vim's broken syntax highlighting which parses
 * angle brackets in multi-line #define statements as if they'd be real code...
 * Remove this as soon as they've fixed their bug */
#define LOOP_END_VIM_SUCKS }

#define ACTION_ADD(act, cb, title, value) if ((act = e_action_add(value))) \
                                            { \
					       act->func.go = cb; \
					       e_action_predef_name_set(D_("Tiling"), D_(title), value, NULL, NULL, 0); \
					    }

#define ACTION_DEL(act, title, value) if (act) \
                                        { \
                                          e_action_predef_name_del(D_("Tiling"), D_(title)); \
                                          e_action_del(value); \
                                          act = NULL; \
                                        }
#define FREE_HANDLER(x)		if (x) { ecore_event_handler_del(x); x = NULL; }

/* Generates a unique identifier for the given desk to be used in info_hash */
static char*
desk_hash_key(E_Desk *desk)
{
   if (!desk) return NULL;
   /* I think 64 chars should be enough for all localizations of "desk" */
   static char buffer[64];
   snprintf(buffer, 64, "%d%s", desk->zone->num, desk->name);
   return buffer;
}

static struct _Config_vdesk*
get_vdesk(int x, int y, int zone_num)
{
   Evas_List *l;

   DBG("getting vdesk x %d / y %d / zone_num %d\n", x, y, zone_num);

   for (l = tiling_config->vdesks; l; l = l->next)
     {
	struct _Config_vdesk *vd = l->data;
	if (!vd) continue;
	if (vd->x == x && vd->y == y && vd->zone_num == zone_num)
	  return vd;
     }

   return NULL;
}

static int
layout_for_desk(E_Desk *desk)
{
   if (tiling_config->tiling_mode == TILE_INDIVIDUAL)
     {
	struct _Config_vdesk *vd = get_vdesk(desk->x, desk->y, desk->zone->num);
	return (vd ? vd->layout : TILE_NONE);
     }
   return tiling_config->tiling_mode;
}

#ifdef TILING_DEBUG
static void
print_borderlist()
{
   if (!tinfo) return;
   Evas_List *l;
   int wc = 0;
   printf("\n\nTILING_DEBUG: Tiling-Borderlist for \"%s\":\n", desk_hash_key(tinfo->desk));
   for (l = tinfo->client_list; l; l = l->next, wc++)
     {
	E_Border *lbd = l->data;
	printf("  #%d = %p, %s, %s, %s, desk %s)\n", wc, lbd, lbd->client.icccm.name,
	      lbd->client.icccm.title, lbd->client.netwm.name, desk_hash_key(lbd->desk));
	printf("  current = %p, next = %p, prev = %p\n", l, l->next, l->prev);
	if (tinfo->mainbd == lbd)
	  printf("this is tinfo->mainbd!\n");
     }
   printf("TILING_DEBUG: End of Borderlist\n\n");
}
#endif

/* Moves the nth list-entry to the left */
static int
border_move_to_left(E_Border *bd, int times)
{
   Evas_List *n, *p;
   void *data;
   int c;

   if (!bd || !tinfo) return 0;
   if (!(n = evas_list_find_list(tinfo->client_list, bd))) return 0;
   if (!(p = n->prev)) return 0;
   data = n->data;
   for (c = 0; c < (times-1); c++)
     if (!(p = p->prev)) return 0;
   tinfo->client_list = evas_list_remove_list(tinfo->client_list, n);
   tinfo->client_list = evas_list_prepend_relative_list(tinfo->client_list, data, p);
   
   return 1;
}

/* Move to right is basically the same as move to left of (num+1) */
static int
border_move_to_right(E_Border *bd, int times)
{
   Evas_List *n, *p;
   void *data;
   int c;

   if (!bd || !tinfo) return 0;
   if (!(n = evas_list_find_list(tinfo->client_list, bd))) return 0;
   if (!(p = n->next)) return 0;
   data = n->data;
   for (c = 0; c < (times-1); c++)
     if (!(p = p->next)) return 0;
   tinfo->client_list = evas_list_remove_list(tinfo->client_list, n);
   tinfo->client_list = evas_list_append_relative_list(tinfo->client_list, data, p);
   return 1;

}

/* Returns the first window from focus-stack (or NULL), avoiding *exclude if specified */
static E_Border *
get_first_window(E_Border *exclude, E_Desk *desk)
{
   Evas_List *l;

   EVAS_LIST_FOREACH(e_border_focus_stack_get())
     {
	if (exclude &&
	      ((lbd == exclude) || (lbd->desk != exclude->desk))) continue;
	if (!exclude && desk && (lbd->desk != desk)) continue;
	if (TILE_LOOP_CHECKS(lbd)) continue;
	return lbd;
     } LOOP_END

   return NULL;
}

static void
toggle_floating(E_Border *bd)
{
   if (!bd || !tinfo) return;

   if (evas_list_find(tinfo->floating_windows, bd) == bd)
     {
	tinfo->floating_windows = evas_list_remove(tinfo->floating_windows, bd);
	if (!tiling_config->dont_touch_borders &&
	    tiling_config->tiling_border &&
	    (!bd->bordername || strcmp(bd->bordername, tiling_config->tiling_border)))
	  change_window_border(bd, tiling_config->tiling_border);
	e_border_idler_before();
	rearrange_windows(bd, 0);
     }
   else
     {
	int w = bd->w, h = bd->h;
	/* To give the user a bit of feedback we restore the original border */
	/* TODO: save the original border, don't just restore the default one */
	tinfo->floating_windows = evas_list_prepend(tinfo->floating_windows, bd);
	if (!tiling_config->dont_touch_borders &&
	    tiling_config->floating_border &&
	    (!bd->bordername || strcmp(bd->bordername, tiling_config->floating_border)))
	  change_window_border(bd, tiling_config->floating_border);
	e_border_idler_before();
	e_border_resize(bd, w, h);
     }
}

/* Checks for windows which are bigger than width/height in their min_w/min_h-
 * attributes and therefore need to be set to floating
 *
 * returns 1 if the given border was affected */
static int
check_for_too_big_windows(int width, int height, E_Border *bd)
{
   Evas_List *l;
   TILE_STDLOOP
     {
	if (lbd->client.icccm.min_w > width || lbd->client.icccm.min_h > height)
	  {
	     toggle_floating(lbd);
	     /* If this was the window this call was about, we don't need to change anything */
	     if (bd && (lbd == bd))
	       return 1;
	  }
     } LOOP_END
   return 0;
}

/* The main tiling happens here. Layouts are calculated and windows are moved/resized */
static void
rearrange_windows(E_Border *bd, int remove_bd)
{
   if (!bd || !tinfo || !tiling_config->tiling_enabled) return;
   if (tinfo->desk && (bd->desk != tinfo->desk))
     {
	/* We need to verify this because when having multiple zones (xinerama)
	 * it's possible that tinfo is initialized for zone 1 even though
	 * it should be zone 0 */
	if (get_current_desk() != tinfo->desk)
	  _desk_show(get_current_desk());
	else return;
     }

#ifdef TILING_DEBUG
   printf("TILING_DEBUG: rearrange_windows()\n");
   print_borderlist();
#endif

   /* Take care of our own tinfo->client_list */
   if (evas_list_find(tinfo->client_list, bd) != bd)
     {
	if (!remove_bd)
	  tinfo->client_list = evas_list_append(tinfo->client_list, bd);
     }
   else
     {
	if (remove_bd)
	  tinfo->client_list = evas_list_remove(tinfo->client_list, bd);
     }

   /* Check if the window is set floating */
   if (evas_list_find(tinfo->floating_windows, bd) == bd)
     {
	if (remove_bd)
	  tinfo->floating_windows = evas_list_remove(tinfo->floating_windows, bd);
	return;
     }

   /* Check if the window is a dialog and whether we should handle it */
   if (!tiling_config->tile_dialogs &&
	((bd->client.icccm.transient_for != 0) ||
	(bd->client.netwm.type == ECORE_X_WINDOW_TYPE_DIALOG)))
     return;

   Evas_List *l, *s;
   int window_count = (remove_bd ? 0 : 1);
   int layout = layout_for_desk(bd->desk);
   if (layout == TILE_NONE)
     return;
   else if (layout == TILE_BIGMAIN)
     {
	if (remove_bd && (bd == tinfo->mainbd))
	  /* If the main border is getting removed, we need to find another one */
	  tinfo->mainbd = get_first_window(bd, NULL);

	if (!remove_bd && !tinfo->mainbd)
	  tinfo->mainbd = bd;
     }

   /* Loop through all windows to count them */
   TILE_STDLOOP
     {
	if (!tiling_config->dont_touch_borders &&
	    tiling_config->tiling_border &&
	    !remove_bd &&
	    ((lbd->bordername && strcmp(lbd->bordername, tiling_config->tiling_border)) ||
	     !lbd->bordername))
	  change_window_border(lbd, tiling_config->tiling_border);
	if (lbd == bd) continue;
	if (lbd->visible == 0) continue;
	window_count++;
     } LOOP_END

   /* If there are no other windows, it's easy: just maximize */
   if (window_count == 1)
     {
	E_Border *lbd = (remove_bd ? get_first_window(bd, NULL) : bd);
	if (lbd)
	  {
	     int offset_top = 0, offset_left = 0;
	     /* However, we still need to check if any of the shelves produces an offset */
	     TILE_STDSLOOP
	       {
		  if (ORIENT_TOP(sh->gadcon->orient))
		    offset_top += sh->h;
		  else if (ORIENT_LEFT(sh->gadcon->orient))
		    offset_left += sh->w;
	       } LOOP_END
	     DBG("maximizing the window\n");
	     e_border_move(lbd, lbd->zone->x + offset_left, lbd->zone->y + offset_top);
	     e_border_unmaximize(lbd, E_MAXIMIZE_BOTH);
	     e_border_maximize(lbd, E_MAXIMIZE_EXPAND | E_MAXIMIZE_BOTH);
	     tinfo->single_win = lbd;
	     return;
	  }
     }
   else if (tinfo->single_win)
     {
	/* If we previously maximized a window, we need to unmaximize it or it takes
	 * up all the space in bigmain mode */
	DBG("unmaximizing\n");
	e_border_unmaximize(tinfo->single_win, E_MAXIMIZE_BOTH);
	tinfo->single_win = NULL;
     }

   switch (layout)
     {
      case TILE_GRID:
	   {
	      int gridrows;
	      if (tiling_config->grid_distribute_equally)
		{
		   int internal = 1;
		   if (window_count > 1)
		     while (((double)window_count / (double)internal) > (double)internal)
		       internal++;
		   gridrows = max(internal, 1);
		}
	      else
		gridrows = max(min(tiling_config->grid_rows, window_count), 1);
	      int c;
	      int wf = (bd->zone->w / gridrows), hf;
	      int wc = 0;
	      int highest_collision_vert = 0,
		  highest_collision_horiz = 0;
	      int windows_per_row = max((int)ceil((double)window_count / (double)gridrows), 1);
	      int shelf_collision_vert[gridrows],
		  shelf_collision_horiz[windows_per_row],
		  offset_top[gridrows],
		  offset_left[windows_per_row];
	      int sub_space_x = (tiling_config->space_between ? tiling_config->between_x : 0);
	      int sub_space_y = (tiling_config->space_between ? tiling_config->between_y : 0);
	      bzero(shelf_collision_vert, sizeof(int) * gridrows);
	      bzero(shelf_collision_horiz, sizeof(int) * windows_per_row);
	      bzero(offset_top, sizeof(int) * gridrows);
	      bzero(offset_left, sizeof(int) * windows_per_row);

	      /* Loop through all the shelves on this screen (=zone) to get their space */
	      /* NOTE:
	       * shelf detection in gridmode has a tolerance area of the height/width (depends
	       * on the orientation) of your shelves if you have more than one.
	       *
	       * If you have found a good way to deal with this problem in a simple manner,
	       * please send a patch :-).
	       */
	      TILE_STDSLOOP
		{
		   E_Gadcon_Orient orient = sh->gadcon->orient;
		   /* Every row between sh_min and sh_max needs to be flagged */
		   if (ORIENT_BOTTOM(orient) || ORIENT_TOP(orient)) {
			int sh_min = sh->x,
			    sh_max = sh->x + sh->w;
			for (c = 0; c < gridrows; c++)
			  {
			     int row_min = ((c % gridrows) * wf),
				 row_max = row_min + wf;
			     if (!shelf_collision_vert[c] &&
				   (between(row_min, sh_min, sh_max) ||
				    between(sh_min, row_min, row_max)))
			       {
				  shelf_collision_vert[c] = sh->h;
				  if (sh->h > highest_collision_vert)
				    highest_collision_vert = sh->h;
				  if (ORIENT_TOP(orient))
				    offset_top[c] = sh->h;
			       }
			  }
		   } else if (ORIENT_LEFT(orient) || ORIENT_RIGHT(orient)) {
			int sh_min = sh->y,
			    sh_max = sh->y + sh->h;
			for (c = 0; c < windows_per_row; c++)
			  {
			     int row_min = c * (bd->zone->h / windows_per_row),
				 row_max = row_min + (bd->zone->h / windows_per_row);
			     if (!shelf_collision_horiz[c] &&
				   (between(row_min, sh_min, sh_max) ||
				    between(sh_min, row_min, row_max)))
			       {
				  shelf_collision_horiz[c] = sh->w;
				  if (sh->w > highest_collision_horiz)
				    highest_collision_horiz = sh->w;
				  if (ORIENT_LEFT(orient))
				    offset_left[c] = sh->w;
			       }
			  }
		   }
		} LOOP_END

	      for (c = 0; c < gridrows; c++)
		shelf_collision_vert[c] = bd->zone->h - (sub_space_y * (windows_per_row-1)) - shelf_collision_vert[c];
	      for (c = 0; c < windows_per_row; c++)
		shelf_collision_horiz[c] = bd->zone->w - (sub_space_x * (gridrows-1)) - shelf_collision_horiz[c];

	      /* Check for too big windows. We're pessimistic for the height-value and use the
	       * one with the lowest available space (due to shelves) */
	      if (tiling_config->float_too_big_windows &&
		  check_for_too_big_windows(((bd->zone->w - highest_collision_horiz) /
			                       max((window_count < (gridrows+1) ? window_count : gridrows), 1)),
                                            ((bd->zone->h - highest_collision_vert) / windows_per_row),
					    bd))
		return;

	      TILE_STDBLOOP
		{
		   if (remove_bd && lbd == bd) continue;
		   int row_horiz = (wc % gridrows),
		       row_vert = (wc / gridrows);
		   wf = (shelf_collision_horiz[row_vert] / gridrows);
		   hf = (shelf_collision_vert[row_horiz] / windows_per_row);
		   move_resize(lbd,
			 (row_horiz * wf) + offset_left[row_vert] + (sub_space_x * row_horiz),
			 (row_vert * hf) + offset_top[row_horiz] + (sub_space_y * row_vert),
			 wf,
			 hf);
		   wc++;
		} LOOP_END
	      break;
	   }
      case TILE_BIGMAIN:
	   {
	      int wc = 0;
	      int bigw = bd->zone->w;
	      int bigh = bd->zone->h;
	      int offset_top = 0, offset_left = 0;
	      int smallh = bigh;
	      int sub_space_x = (tiling_config->space_between ? tiling_config->between_x : 0);
	      int sub_space_y = (tiling_config->space_between ? tiling_config->between_y : 0);

	      /* Loop through all the shelves on this screen (=zone) to get their space */
	      TILE_STDSLOOP
		{
		   /* Decide what to do based on the orientation of the shelf */
		   E_Gadcon_Orient orient = sh->gadcon->orient;
		   if (ORIENT_BOTTOM(orient) || ORIENT_TOP(orient))
		     {
			if (sh->x <= (bigw * tinfo->big_perc))
			   bigh -= sh->h;
			 if ((sh->x + sh->w) >= (bigw * tinfo->big_perc))
			   smallh -= sh->h;
			 if (ORIENT_TOP(orient))
			   offset_top = sh->h;
		     }
		   if (ORIENT_LEFT(orient) || ORIENT_RIGHT(orient))
		     bigw -= sh->w;
		   if (ORIENT_LEFT(orient))
		     offset_left = sh->w;
		} LOOP_END

	      int smallw = bigw;
	      bigw *= tinfo->big_perc;
	      smallw -= (bigw + sub_space_x);
	      smallh -= (sub_space_y * (window_count-2));
	      int hf = (smallh / (window_count-1));
	      if (tiling_config->float_too_big_windows &&
		  check_for_too_big_windows(bigw, hf, bd))
		return;

	      /* Handle Small windows */
	      TILE_STDBLOOP
		{
		   if (lbd == tinfo->mainbd) continue;
		   move_resize(lbd, sub_space_x + offset_left + bigw, (wc * hf) + offset_top + (wc * sub_space_y), smallw, hf);
		   wc++;
		} LOOP_END

	      if (tinfo->mainbd)
		{
		   tinfo->mainbd_width = bigw;
		   move_resize(tinfo->mainbd, offset_left, offset_top, bigw, bigh);
		}
	      break;
	   }
     }
   DBG("rearrange done\n\n");
}

static Tiling_Info *
_initialize_tinfo(E_Desk *desk)
{
   Evas_List *l;
   Tiling_Info *res;

   res = E_NEW(Tiling_Info, 1);
   res->mainbd_width = -1;
   res->desk = desk;
   res->big_perc = tiling_config->big_perc;
   res->need_rearrange = 0;
   info_hash = evas_hash_add(info_hash, desk_hash_key(desk), res);

   EVAS_LIST_FOREACH(e_border_client_list())
     {
	if (lbd->desk == desk)
	  res->client_list = evas_list_append(res->client_list, lbd);
     } LOOP_END

   return res;
}

static void
_desk_before_show(E_Desk *desk)
{
   if (tinfo->desk == desk)
     {
	DBG("desk before show: %s \n", desk->name);
	if (!evas_hash_modify(info_hash, desk_hash_key(desk), tinfo))
	  info_hash = evas_hash_add(info_hash, desk_hash_key(desk), tinfo);
     }
   tinfo = NULL;
}

static void
_desk_show(E_Desk *desk)
{
   tinfo = evas_hash_find(info_hash, desk_hash_key(desk));
   if (!tinfo)
     {
	/* We need to add a new Tiling_Info, so we weren't on that desk before.
	 * As e doesn't call call the POST_EVAL-hook (or e_desk_show which then
	 * indirectly calls the POST_EVAL) for each window on that desk but only
	 * for the focused, we need to get all borders on that desk. */
	DBG("need new info for %s\n", desk->name);
	tinfo = _initialize_tinfo(desk);
     }
   else
     {
	if (tinfo->need_rearrange)
	  {
	     DBG("need_rearrange\n");
	     E_Border *first;
	     if ((first = get_first_window(NULL, desk)))
	       rearrange_windows(first, 0);
	     tinfo->need_rearrange = 0;
	  }
     }
#ifdef TILING_DEBUG
   printf("TILING_DEBUG: desk show. %s\n", desk->name);
   print_borderlist();
   printf("TILING_DEBUG: desk show done\n");
#endif
}

/***************************************************************************/
/* Action callbacks */
/***************************************************************************/

static void
_e_mod_action_toggle_tiling_cb(E_Object *obj, const char *params)
{
   tiling_config->tiling_enabled = !tiling_config->tiling_enabled;
   e_mod_tiling_rearrange();
}

static void
_e_mod_action_toggle_floating_cb(E_Object *obj, const char *params)
{
   toggle_floating(e_border_focused_get());
}

static void
toggle_layout(int *layout)
{
   if (*layout == TILE_GRID)
     *layout = TILE_BIGMAIN;
   else if (*layout == TILE_BIGMAIN)
     *layout = TILE_NONE;
   else if (*layout == TILE_NONE)
     *layout = TILE_GRID;
}

static void
_e_mod_action_switch_tiling_cb(E_Object *obj, const char *params)
{
   if (tiling_config->tiling_mode != TILE_INDIVIDUAL)
     toggle_layout(&(tiling_config->tiling_mode));
   else
     {
	E_Desk *desk = get_current_desk();
	if (!desk) return;
	struct _Config_vdesk *vd = get_vdesk(desk->x, desk->y, desk->zone->num);
	if (!vd)
	  {
	     /* There was no config entry. Probably the vdesk-configuration changed but the user
	      * didn't open the tiling config yet. */
	     struct _Config_vdesk *vd = malloc(sizeof(struct _Config_vdesk));
	     if (!vd) return;
	     vd->x = desk->x;
	     vd->y = desk->y;
	     vd->layout = tiling_config->tiling_mode;
	     tiling_config->vdesks = evas_list_append(tiling_config->vdesks, vd);
	  }
	toggle_layout(&(vd->layout));
     }

   e_mod_tiling_rearrange();

   e_config_save_queue();
}

static void
_e_mod_action_move_left(E_Object *obj, const char *params)
{
   E_Border *bd = e_border_focused_get();
   if (!bd) return;

   switch (layout_for_desk(bd->desk))
     {
      case TILE_BIGMAIN:
	 tinfo->mainbd = bd;
	 rearrange_windows(bd, 0);
	 break;
      case TILE_GRID:
	 if (border_move_to_left(bd, 1))
	   rearrange_windows(bd, 0);
	 break;
     }
}

static void
_e_mod_action_move_right(E_Object *obj, const char *params)
{
   E_Border *bd = e_border_focused_get();
   if (border_move_to_right(bd, 1))
     rearrange_windows(bd, 0);
}

static void
_e_mod_action_move_top(E_Object *obj, const char *params)
{
   E_Border *bd = e_border_focused_get();
   if (!bd) return;

   switch (layout_for_desk(bd->desk))
     {
      case TILE_BIGMAIN:
	 if (border_move_to_left(bd, 1))
	   rearrange_windows(bd, 0);
	 break;
      case TILE_GRID:
	 if (border_move_to_left(bd, tiling_config->grid_rows))
	   rearrange_windows(bd, 0);
	 break;
     }
}

static void
_e_mod_action_move_bottom(E_Object *obj, const char *params)
{
   E_Border *bd = e_border_focused_get();
   if (!bd) return;

   switch (layout_for_desk(bd->desk))
     {
      case TILE_BIGMAIN:
	 if (border_move_to_right(bd, 1))
	   rearrange_windows(bd, 0);
	 break;
      case TILE_GRID:
	 if (border_move_to_right(bd, tiling_config->grid_rows))
	   rearrange_windows(bd, 0);
	 break;
     }
}

/***************************************************************************/
/* Hooks */
/***************************************************************************/

static void
_e_module_tiling_cb_hook(void *data, E_Border *bd)
{
   if (!bd || TILE_LOOP_CHECKS(bd) ||
       (!bd->changes.size && !bd->changes.pos && (evas_list_find(tinfo->client_list, bd) == bd)))
	return;
   DBG("cb-Hook for %p / %s / %s, size.changes = %d, position.changes = %d\n", bd,
	 bd->client.icccm.title, bd->client.netwm.name, bd->changes.size, bd->changes.pos);

   /* If the border changes size, we maybe want to adjust the layout */
   if (tinfo && bd->changes.size)
     {
        switch (layout_for_desk(bd->desk))
          {
           case TILE_BIGMAIN:
              /* Only the mainbd-window is resizable */
              if (bd != tinfo->mainbd || tinfo->mainbd_width == -1) break;
	      /* Don't take the size of a maximized window */
	      if (bd->maximized) break;
              /* If the difference is too small, do nothing */
	      if (between(tinfo->mainbd_width, (bd->w - 2), (bd->w + 2))) break;
#ifdef TILING_DEBUG
              printf("TILING_DEBUG: trying to change the tinfo->mainbd width to %d (it should be: %d), big_perc atm is %f\n", bd->w, tinfo->mainbd_width, tinfo->big_perc);
#endif
              /* x is the factor which is caused by shelves */
              double x = tinfo->mainbd_width / tinfo->big_perc / bd->desk->zone->w;
#ifdef TILING_DEBUG
              printf("TILING_DEBUG: x = %f -> big_perc = %f\n", x, bd->w / x / bd->desk->zone->w);
#endif
              tinfo->big_perc = bd->w / x / bd->desk->zone->w;
              break;
          }
     }
   rearrange_windows(bd, 0);
}

static int
_e_module_tiling_hide_hook(void *data, int type, void *event)
{
   DBG("hide-hook\n");
   E_Event_Border_Hide *ev = event;
   rearrange_windows(ev->border, 1);

   if (currently_switching_desktop) return 1;

   /* Ensure that the border is deleted from all available desks */
   static Tiling_Info *_tinfo = NULL;
   Evas_List *l, *ll, *lll;
   E_Zone *zone;
   E_Desk *desk;
   int i;

   for (l = e_manager_list(); l; l = l->next)
     for (ll = ((E_Manager*)(l->data))->containers; ll; ll = ll->next)
       for (lll = ((E_Container *)(ll->data))->zones; lll; lll = lll->next)
	 {
	    zone = lll->data;
	    for (i = 0; i < (zone->desk_x_count * zone->desk_y_count); i++)
	      {
		 desk = zone->desks[i];
		 if ((_tinfo = evas_hash_find(info_hash, desk_hash_key(desk))) == NULL)
		   continue;
		 if (evas_list_find(_tinfo->client_list, ev->border) == ev->border)
		   _tinfo->client_list = evas_list_remove(_tinfo->client_list, ev->border);
	      }
	 }

   return 1;
}

static int
_e_module_tiling_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev = event;
   _desk_show(ev->desk);
   currently_switching_desktop = 0;
   return 1;
}

static int
_e_module_tiling_desk_before_show(void *data, int type, void *event)
{
   E_Event_Desk_Before_Show *ev = event;
   _desk_before_show(ev->desk);
   currently_switching_desktop = 1;
   return 1;
}

static Evas_Bool
_clear_bd_from_info_hash(const Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   Tiling_Info *ti = data;
   E_Event_Border_Desk_Set *ev = fdata;
   if (!ev || !ti) return 1;
   if (ti->desk == ev->desk)
     {
	ti->need_rearrange = 1;
	DBG("set need_rearrange=1\n");
	return 1;
     }
   if (evas_list_find(ti->client_list, ev->border) == ev->border)
     {
	ti->client_list = evas_list_remove(ti->client_list, ev->border);
	if (ti->desk == get_current_desk())
	  {
	     E_Border *first;
	     if ((first = get_first_window(NULL, ti->desk)))
	       rearrange_windows(first, 0);
	  }
     }
   if (ti->mainbd == ev->border)
     ti->mainbd = get_first_window(NULL, ti->desk);
   if (evas_list_find(ti->floating_windows, ev->border) == ev->border)
     ti->floating_windows = evas_list_remove(ti->floating_windows, ev->border);
   return 1;
}

static int
_e_module_tiling_desk_set(void *data, int type, void *event)
{
   /* We use this event to ensure that border desk changes are done correctly because
    * a user can move the window to another desk (and events are fired) involving
    * zone changes or not (depends on the mouse position) */
   E_Event_Border_Desk_Set *ev = event;
   Tiling_Info *_tinfo = evas_hash_find(info_hash, desk_hash_key(ev->desk));
   if (!_tinfo)
     {
	DBG("create new info for %s\n", ev->desk->name);
	_tinfo = _initialize_tinfo(ev->desk);
     }

   evas_hash_foreach(info_hash, _clear_bd_from_info_hash, ev);
   DBG("desk set\n");

   return 1;
}

static int
_e_module_tiling_mouse_move(void *data, int type, void *event)
{
   Ecore_X_Event_Mouse_Move *ev = event;

   if (!current_zone || !E_INSIDE(ev->root.x, ev->root.y,
	current_zone->x, current_zone->y, current_zone->w, current_zone->h))
     {
	_desk_before_show(tinfo->desk);
	E_Desk *desk = get_current_desk();
	current_zone = desk->zone;
	_desk_show(desk);
     }

   return 1;
}


/***************************************************************************/
/* Exported functions */
/***************************************************************************/

EAPI void
e_mod_tiling_rearrange()
{
   Evas_List *l, *ll, *lll;
   E_Zone *zone;
   E_Desk *desk;
   E_Border *first;
   int i;

   for (l = e_manager_list(); l; l = l->next)
     for (ll = ((E_Manager*)(l->data))->containers; ll; ll = ll->next)
       for (lll = ((E_Container *)(ll->data))->zones; lll; lll = lll->next)
	 {
	    zone = lll->data;
	    for (i = 0; i < (zone->desk_x_count * zone->desk_y_count); i++)
	      {
		 desk = zone->desks[i];
		 if ((first = get_first_window(NULL, desk)))
		   rearrange_windows(first, 0);
	      }
	 }
}

/***************************************************************************/
/* Module setup */
/***************************************************************************/

static Evas_Bool
_clear_info_hash(const Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   Tiling_Info *ti = data;
   evas_list_free(ti->floating_windows);
   evas_list_free(ti->client_list);
   E_FREE(ti);
   return 1;
}

EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
     "Tiling"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];
   tiling_module = m;

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   /* Callback for new windows or changes */
   hook = e_border_hook_add(E_BORDER_HOOK_EVAL_POST_FETCH, _e_module_tiling_cb_hook, NULL);
   /* Callback for hiding windows */
   handler_hide = ecore_event_handler_add(E_EVENT_BORDER_HIDE, _e_module_tiling_hide_hook, NULL);
   /* Callback when virtual desktop changes */
   handler_desk_show = ecore_event_handler_add(E_EVENT_DESK_SHOW, _e_module_tiling_desk_show, NULL);
   /* Callback before virtual desktop changes */
   handler_desk_before_show = ecore_event_handler_add(E_EVENT_DESK_BEFORE_SHOW, _e_module_tiling_desk_before_show, NULL);
   /* Callback when the mouse moves */
   handler_mouse_move = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _e_module_tiling_mouse_move, NULL);
   /* Callback when a border is set to another desk */
   handler_desk_set = ecore_event_handler_add(E_EVENT_BORDER_DESK_SET, _e_module_tiling_desk_set, NULL);

   /* Module's actions */
   ACTION_ADD(act_toggletiling, _e_mod_action_toggle_tiling_cb, "Toggle tiling", "toggle_tiling");
   ACTION_ADD(act_togglefloat, _e_mod_action_toggle_floating_cb, "Toggle floating", "toggle_floating");
   ACTION_ADD(act_switchtiling, _e_mod_action_switch_tiling_cb, "Switch tiling mode", "switch_tiling");
   ACTION_ADD(act_moveleft, _e_mod_action_move_left, "Move window to the left", "tiling_move_left");
   ACTION_ADD(act_moveright, _e_mod_action_move_right, "Move window to the right", "tiling_move_right");
   ACTION_ADD(act_movebottom, _e_mod_action_move_top, "Move window to the bottom", "tiling_move_bottom");
   ACTION_ADD(act_movetop, _e_mod_action_move_bottom, "Move window to the top", "tiling_move_top");

   /* Configuration entries */
   snprintf(buf, sizeof(buf), "%s/e-module-tiling.edj", e_module_dir_get(m));
   e_configure_registry_category_add("windows", 50, D_("Windows"), NULL, "enlightenment/windows");
   e_configure_registry_item_add("windows/tiling", 150, D_("Tiling"), NULL, buf, e_int_config_tiling_module);

   /* Configuration itself */
   tiling_config_edd = E_CONFIG_DD_NEW("Tiling_Config", Config);
   vdesk_edd = E_CONFIG_DD_NEW("Tiling_Config_VDesk", struct _Config_vdesk);
   E_CONFIG_VAL(tiling_config_edd, Config, tiling_enabled, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, tiling_mode, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, dont_touch_borders, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, tile_dialogs, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, float_too_big_windows, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, grid_rows, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, grid_distribute_equally, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, big_perc, DOUBLE);
   E_CONFIG_VAL(tiling_config_edd, Config, floating_border, STR);
   E_CONFIG_VAL(tiling_config_edd, Config, tiling_border, STR);
   E_CONFIG_VAL(tiling_config_edd, Config, space_between, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, between_x, INT);
   E_CONFIG_VAL(tiling_config_edd, Config, between_y, INT);

   E_CONFIG_LIST(tiling_config_edd, Config, vdesks, vdesk_edd);
   E_CONFIG_VAL(vdesk_edd, struct _Config_vdesk, x, INT);
   E_CONFIG_VAL(vdesk_edd, struct _Config_vdesk, y, INT);
   E_CONFIG_VAL(vdesk_edd, struct _Config_vdesk, layout, INT);
   E_CONFIG_VAL(vdesk_edd, struct _Config_vdesk, zone_num, INT);

   tiling_config = e_config_domain_load("module.tiling", tiling_config_edd);
   if (!tiling_config)
     {
	tiling_config = E_NEW(Config, 1);
	tiling_config->tiling_mode = TILE_BIGMAIN;
	tiling_config->float_too_big_windows = 1;
	tiling_config->big_perc = 0.5;
	tiling_config->grid_rows = 2;
     }
   else
     {
	/* Because e doesn't strdup these when loading from configuration, we have to */
	if (tiling_config->floating_border)
	  tiling_config->floating_border = strdup(tiling_config->floating_border);
	if (tiling_config->tiling_border)
	  tiling_config->tiling_border = strdup(tiling_config->tiling_border);
     }
   if (!tiling_config->tiling_border)
     tiling_config->tiling_border = strdup("pixel");
   if (!tiling_config->floating_border)
     tiling_config->floating_border = strdup("default");
   E_CONFIG_LIMIT(tiling_config->tiling_enabled, 0, 1);
   E_CONFIG_LIMIT(tiling_config->dont_touch_borders, 0, 1);
   E_CONFIG_LIMIT(tiling_config->tiling_mode, TILE_GRID, TILE_INDIVIDUAL);
   E_CONFIG_LIMIT(tiling_config->tile_dialogs, 0, 1);
   E_CONFIG_LIMIT(tiling_config->float_too_big_windows, 0, 1);
   E_CONFIG_LIMIT(tiling_config->grid_rows, 1, 12);
   E_CONFIG_LIMIT(tiling_config->grid_distribute_equally, 0, 1);
   E_CONFIG_LIMIT(tiling_config->big_perc, 0.1, 1);
   E_CONFIG_LIMIT(tiling_config->space_between, 0, 1);

   E_Desk *desk = get_current_desk();
   current_zone = desk->zone;
   tinfo = _initialize_tinfo(desk);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   if (hook)
     {
	e_border_hook_del(hook);
	hook = NULL;
     }
   FREE_HANDLER(handler_hide);
   FREE_HANDLER(handler_desk_show);
   FREE_HANDLER(handler_desk_before_show);
   FREE_HANDLER(handler_mouse_move);
   FREE_HANDLER(handler_desk_set);

   ACTION_DEL(act_toggletiling, "Toggle tiling", "toggle_tiling");
   ACTION_DEL(act_togglefloat, "Toggle floating", "toggle_floating");
   ACTION_DEL(act_switchtiling, "Switch tiling mode", "switch_tiling");
   ACTION_DEL(act_moveleft, "Move window to the left", "tiling_move_left");
   ACTION_DEL(act_moveright, "Move window to the right", "tiling_move_right");
   ACTION_DEL(act_movebottom, "Move window to the bottom", "tiling_move_bottom");
   ACTION_DEL(act_movetop, "Move window to the top", "tiling_move_top");

   e_configure_registry_item_del("windows/tiling");
   e_configure_registry_category_del("windows");

   E_FREE(tiling_config);
   E_CONFIG_DD_FREE(tiling_config_edd);
   E_CONFIG_DD_FREE(vdesk_edd);

   tiling_module = NULL;
   evas_hash_foreach(info_hash, _clear_info_hash, NULL);
   evas_hash_free(info_hash);
   info_hash = NULL;
   tinfo = NULL;

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.tiling", tiling_config_edd, tiling_config);
   return 1;
}
