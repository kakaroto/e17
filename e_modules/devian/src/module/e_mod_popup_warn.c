#include "dEvian.h"


static Evas_List *_popups_warn;

static void _update(Popup_Warn *popw, const char *text);
static void _try_close(Popup_Warn *popw);

static void _check_overlap(int *px, int *py, int *pw, int *ph, int tries, int org_x, int org_y);
static int _cb_timer(void *data);

static void _cb_edje_close(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_next(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_desactivate(void *data, Evas_Object *obj, const char *emission, const char *source);


/* PUBLIC FUNCTIONS */

/**
 * Initialise popup warn system
 * @return 1 on success
 */
int DEVIANF(popup_warn_init) (void)
{
   _popups_warn = NULL;
   return 1;
}

/**
 * Shutdown popup warn system
 */
void DEVIANF(popup_warn_shutdown) (void)
{
   Evas_List *l;
   Popup_Warn *p;

   for (l = _popups_warn; l; l = evas_list_next(l))
     {
        p = evas_list_data(l);
        DEVIANF(popup_warn_del) (p);
     }
   evas_list_free(_popups_warn);
   _popups_warn = NULL;
}

/**
 * Add popup warning above all windows to alert user that something happened
 * If one already exists, update it. Only text and popw_old are used in this case
 *
 * @param type Type of poput, INFO or ERROR
 * @param text Text to display with popup
 * @param popw_old Old popup to update, NULL to create a new popup
 * @param keep_old Keep popw_old or not, 0 if popw_old is NULL
 * @param timer Timer for the popup. 0 to set no timer
 * @param devian The dEvian wich is associated with the new popup, can be NULL
 * @param func_close Function to call when popup close
 * @param func_desactivate Function to call when popup desactivate, can be NULL
 * @return 0 if fails to create, 1 if first creation and 2 if update existing popup
 */
Popup_Warn *DEVIANF(popup_warn_add) (int type, const char *text, Popup_Warn *popw_old, int keep_old, int timer, DEVIANN *devian, int (*func_close) (Popup_Warn *popw, void *data), void (*func_desactivate) (Popup_Warn *popw, void *data))
{
   Popup_Warn *popw;

   if (popw_old && keep_old)
     {
        popw = popw_old;
        _update(popw, text);
     }
   else
     {
        E_Zone *zone;
        int fw, fh;

        popw = E_NEW(Popup_Warn, 1);

        popw->type = type;
        popw->name = NULL;
        popw->pop = NULL;
        popw->log = NULL;
        popw->face = NULL;
        popw->timer = NULL;
        popw->devian = NULL;
        popw->func_close = NULL;
        popw->func_desactivate = NULL;
        popw->timer_org = timer;

        zone = e_util_zone_current_get(e_manager_current_get());

        /* pop */
        popw->pop = e_popup_new(zone, 0, 0, 1, 1);
        if (!popw->pop)
          {
             DEVIANF(popup_warn_del) (popw);
             return 0;
          }
        evas_event_freeze(popw->pop->evas);
        e_popup_layer_set(popw->pop, 255);

        /* face (+text) */
        popw->face = edje_object_add(popw->pop->evas);
        if (!DEVIANF(devian_edje_load) (popw->face, "devian/popup/warn",
                                        DEVIAN_THEME_TYPE_POPUP))
          {
             DEVIANF(popup_warn_del) (popw);
             return 0;
          }
        edje_object_signal_callback_add(popw->face, "close", "popup",
                                        _cb_edje_close, popw);
        edje_object_signal_callback_add(popw->face, "next", "popup",
                                        _cb_edje_next, popw);
        edje_object_signal_callback_add(popw->face, "desactivate", "popup",
                                        _cb_edje_desactivate, popw);
        edje_object_part_text_set(popw->face, "text", text);
        edje_object_part_geometry_get(popw->face, "text_border",
                                      NULL, NULL, &fw, NULL);
        edje_object_part_geometry_get(popw->face, "background",
                                      NULL, NULL, NULL, &fh);
        evas_object_resize(popw->face, fw, fh);
        evas_object_move(popw->face, 0, 0);

        /* type */
        edje_object_message_send(popw->face, EDJE_MESSAGE_INT,
                                 POPUP_WARN_EDJE_MESSAGE_TYPE,
                                 &type);

        /* name */
        if (devian)
          popw->name = evas_stringshare_add(DEVIANF(source_name_get) (devian, -1));
        else
          popw->name = evas_stringshare_add(MODULE_NAME);
        edje_object_part_text_set(popw->face, "name", popw->name);

        /* pos */
        popw->x = DEVIANM->canvas_w - (fw + 20);;
        popw->y = DEVIANM->canvas_h - (fh + 20);;
        popw->w = fw;
        popw->h = fh;

        /* log */
        popw->log = evas_list_append(popw->log, evas_stringshare_add(text));

        /* timer */
        if (timer)
          popw->timer = ecore_timer_add(timer, _cb_timer, popw);

        /* attached to a devian or not */
        if (devian)
          {
             popw->devian = devian;
             DEVIANF(container_warning_indicator_change) (devian, 1);
          }

        /* close and desactivate functions */
        popw->func_close = func_close;
        if (func_desactivate)
          {
             int show_desactivate = 1;

             popw->func_desactivate = func_desactivate;
             edje_object_message_send(popw->face, EDJE_MESSAGE_INT,
                                      POPUP_WARN_EDJE_MESSAGE_SHOW_DESACTIVATE,
                                      &show_desactivate);
          }

        /* remove old popup */
        if (popw_old)
          DEVIANF(popup_warn_del) (popw_old);

        /* check for popup overlaps */
        _check_overlap(&popw->x, &popw->y, &popw->w, &popw->h, 0, popw->x, popw->y);
        e_popup_move_resize(popw->pop, popw->x, popw->y, popw->w, popw->h);
        DPOPW(("New: %dx%d : %dx%d", popw->x, popw->y, popw->w, popw->h));
        DPOPW(("New face: %dx%d", fw, fh));

        /* go ! */
        evas_object_show(popw->face);
        e_popup_edje_bg_object_set(popw->pop, popw->face);
        evas_event_thaw(popw->pop->evas);
        e_popup_show(popw->pop);

        _popups_warn = evas_list_append(_popups_warn, popw);
     }

   edje_object_signal_emit(popw->face, "update", "devian");

   return popw;
}

/**
 * Delete the warning popup of a dEvian
 *
 * @param popw The popup
 */
void DEVIANF(popup_warn_del) (Popup_Warn *popw)
{
   Evas_List *l;

   if (!popw)
     return;

   if (popw->devian)
     DEVIANF(container_warning_indicator_change) (popw->devian, 0);

   if (popw->timer)
     ecore_timer_del(popw->timer);

   for (l = popw->log; l; l = evas_list_next(l))
     evas_stringshare_del(l->data);
   evas_list_free(popw->log);

   if (popw->pop)
     e_object_del(E_OBJECT(popw->pop));

   if (popw->name)
     evas_stringshare_del(popw->name);

   if (popw->face)
     {
        edje_object_signal_callback_del(popw->face, "close", "popup",
                                        _cb_edje_close);
        edje_object_signal_callback_del(popw->face, "next", "popup",
                                        _cb_edje_next);
        edje_object_signal_callback_del(popw->face, "desactivate", "popup",
                                        _cb_edje_desactivate);
        evas_object_del(popw->face);
     }

   _popups_warn = evas_list_remove(_popups_warn, popw);

   free(popw);
}

/**
 * Change the theme of active popups (TODO...)
 */
void DEVIANF(popup_warn_theme_change) (void)
{

}


/* PRIVATE FUNCTIONS */

static void
_update(Popup_Warn *popw, const char *text)
{
   char buf[3];
   int times;

   /* name */
   if (popw->devian)
     {
        char *tmp;
        tmp = DEVIANF(source_name_get) (popw->devian, -1);
        if (strcmp(popw->name, tmp))
          {
             evas_stringshare_del(popw->name);
             popw->name = evas_stringshare_add(tmp);
             edje_object_part_text_set(popw->face, "name", popw->name);
          }
     }

   /* log */
   popw->log = evas_list_append(popw->log, evas_stringshare_add(text));
   times = evas_list_count(popw->log);
   snprintf(buf, sizeof(buf), "%d", times);
   edje_object_part_text_set(popw->face, "times", buf);

   /* timer */
   if (popw->timer)
     {
        ecore_timer_del(popw->timer);
        popw->timer = ecore_timer_add(popw->timer_org, _cb_timer, popw);
     }

   /* text */
   edje_object_part_text_set(popw->face, "text", text);
}

static void
_try_close(Popup_Warn *popw)
{
   if (popw->func_close)
     {
        if (popw->func_close(popw, popw->devian))
          DEVIANF(popup_warn_del) (popw);
     }
   else
     DEVIANF(popup_warn_del) (popw);
}

static void
_check_overlap(int *px, int *py, int *pw, int *ph, int tries, int org_x, int org_y)
{
   Evas_List *l;
   Popup_Warn *p;
   int pxw, pyh;
   int p_xw, p_yh;

   pxw = *px + *pw;
   pyh = *py + *ph;
   for (l = _popups_warn; l; l = evas_list_next(l))
     {
        p = evas_list_data(l);
        p_xw = p->x + p->w;
        p_yh = p->y + p->h;
        if (((p->x >= *px) && (p->x <= pxw) &&
             (p->y >= *py) && (p->y <= pyh))
            ||
            ((*px >= p->x) && (*px <= p_xw) &&
             (*py >= p->y) && (*py <= p_yh)))
          {
             /* overlap ! correct coords */
             /* try upper, and then on the left */
             /* TODO...: try down and right, maybe placement policy ? */
             DPOPW(("Overlap !"));
             *py = p->y - (*ph + POPUP_WARN_OVERLAP_BORDER);
             if (*py < 0)
               {
                  *py = org_y;
                  *px = *px - (*px + POPUP_WARN_OVERLAP_BORDER);
                  if (*px < 0) break;
               }
             tries++;
             if (tries > POPUP_WARN_OVERLAP_CHECK_MAX)
               return;
             else
               _check_overlap(px, py, pw, ph, tries, org_x, org_y);
             break;
          }
     }
}

static int
_cb_timer(void *data)
{
   Popup_Warn *popw;

   popw = data;
   _try_close(popw);

   return 0;
}

static void
_cb_edje_close(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Popup_Warn *popw;

   popw = data;
   _try_close(popw);
}

static void
_cb_edje_next(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Popup_Warn *popw;
   Evas_List *l;
   const char *tmp;
   char buf[3];
   int times;

   popw = data;

   /* log */
   times = evas_list_count(popw->log);
   if (times == 1)
     {
        _try_close(popw);
        return;
     }
   l = evas_list_last(popw->log);
   tmp = evas_list_data(l);
   edje_object_part_text_set(popw->face, "text", tmp);
   evas_stringshare_del(tmp);
   popw->log = evas_list_remove_list(popw->log, l);
   times--;
   if (times > 1)
     snprintf(buf, sizeof(buf), "%d", times);
   else
     strcpy(buf, "");
   edje_object_part_text_set(popw->face, "times", buf);

   /* timer */
   if (popw->timer)
     ecore_timer_del(popw->timer);
   popw->timer = ecore_timer_add(popw->timer_org, _cb_timer, popw);
}

static void
_cb_edje_desactivate(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Popup_Warn *popw;

   popw = data;

   if (popw->func_desactivate)
     popw->func_desactivate(popw, popw->devian);
}
