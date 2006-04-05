#include "dEvian.h"

static Evas_List *_popups_warn;

static void _update(Popup_Warn *popw, const char *text);
static void _check_overlap(int *px, int *py, int *pw, int *ph, int tries, int org_x, int org_y);
static int _cb_timer(void *data);
static void _cb_edje_close(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_next(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_desactivate(void *data, Evas_Object *obj, const char *emission, const char *source);

/* PUBLIC FUNCTIONS */

/**
 * Add popup warning above all windows to alert user that something happened
 * If one already exists, update it
 * Multiple types of popup exists
 * 
 * @param popup_warn Adress where (will) live the popup. If NULL, do check if popup already exists and save it either
 * @param type Type of poput (.h)
 * @param text Text to display with popup
 * @param data Extra data
 * @return 0 if fails to create, 1 if first creation and 2 if update existing popup
 */
int DEVIANF(popup_warn_add) (Popup_Warn **popup_warn, int type, const char *text, void *data)
{
   Popup_Warn *popw;
   int ret;

   if (popup_warn)
      popw = *popup_warn;
   else
      popw = NULL;

   if (popw)
     {
        _update(popw, text);
        ret = 2;
     }
   else
     {
        E_Zone *zone;
        int px, py, pw, ph;
        int fw, fh;
        int show_desactivate;

        popw = E_NEW(Popup_Warn, 1);

        popw->type = type;
        popw->name = NULL;
        popw->pop = NULL;
        popw->log = NULL;
        popw->face = NULL;
        popw->timer = NULL;
        popw->timer_org = 0;
        popw->data = data;

        zone = e_util_zone_current_get(e_manager_current_get());

        /* Pop */
        popw->pop = e_popup_new(zone, 0, 0, 1, 1);
        if (!popw->pop)
          {
             DEVIANF(popup_warn_del) (popw);
             return 0;
          }
        e_popup_layer_set(popw->pop, 255);

        evas_event_freeze(popw->pop->evas);

        /* Face */
        popw->face = edje_object_add(popw->pop->evas);
        if (!DEVIANF(devian_edje_load) (popw->face, "devian/popup/warn", DEVIAN_THEME_TYPE_POPUP))
          {
             DEVIANF(popup_warn_del) (popw);
             return 0;
          }
        edje_object_signal_callback_add(popw->face, "close", "popup", _cb_edje_close, popw);
        edje_object_signal_callback_add(popw->face, "next", "popup", _cb_edje_next, popw);
        edje_object_signal_callback_add(popw->face, "desactivate", "popup", _cb_edje_desactivate, popw);
        edje_object_part_text_set(popw->face, "text", text);
        edje_object_part_geometry_get(popw->face, "text_border", NULL, NULL, &fw, NULL);
        edje_object_part_geometry_get(popw->face, "background", NULL, NULL, NULL, &fh);
        evas_object_resize(popw->face, fw, fh);
        evas_object_move(popw->face, 0, 0);
        show_desactivate = 0;

        /* OK :) Name & Pos + Extra */
        pw = fw;
        ph = fh;
        switch (type)
          {
             DEVIANN *devian;
             int *time;

          case POPUP_WARN_TYPE_DEVIAN:
             devian = (DEVIANN *)data;
             popw->name = evas_stringshare_add(DEVIANF(source_name_get) (devian, -1));
             edje_object_part_text_set(popw->face, "name", popw->name);
             px = DEVIANM->canvas_w - (fw + 20);
             py = DEVIANM->canvas_h - (fh + 20);
             /* Log */
             popw->log = evas_list_append(popw->log, evas_stringshare_add(text));
             /* Timer */
#ifdef HAVE_RSS
             if (DEVIANM->conf->sources_rss_popup_news_timer)
                popw->timer = ecore_timer_add(DEVIANM->conf->sources_rss_popup_news_timer, _cb_timer, popw);
#endif
             show_desactivate = 1;
             break;

          case POPUP_WARN_TYPE_INFO:
             popw->name = evas_stringshare_add(MODULE_NAME);
             edje_object_part_text_set(popw->face, "name", popw->name);
             px = (DEVIANM->canvas_w - fw) / 2;
             py = (DEVIANM->canvas_h - fh) / 2;
             break;

          case POPUP_WARN_TYPE_INFO_TIMER:
             time = data;
             popw->name = evas_stringshare_add(MODULE_NAME);
             edje_object_part_text_set(popw->face, "name", popw->name);
             px = (DEVIANM->canvas_w - fw) / 2;
             py = (DEVIANM->canvas_h - fh) / 2;
             /* Timer */
             popw->timer = ecore_timer_add(*time, _cb_timer, popw);
             popw->timer_org = *time;

          default:
             popw->name = evas_stringshare_add("");
          }
        edje_object_message_send(popw->face, EDJE_MESSAGE_INT, POPUP_WARN_EDJE_MESSAGE_SHOW_DESACTIVATE, &show_desactivate);
        evas_object_show(popw->face);
        e_popup_edje_bg_object_set(popw->pop, popw->face);

        /* Check for popup overlaps */
        _check_overlap(&px, &py, &pw, &ph, 0, px, py);
        DPOPW(("New: %dx%d : %dx%d", px, py, pw, ph));
        DPOPW(("New face: %dx%d", fw, fh));

        /* Go ! */
        popw->x = px;
        popw->y = py;
        popw->w = pw;
        popw->h = ph;
        e_popup_move_resize(popw->pop, px, py, pw, ph);

        evas_event_thaw(popw->pop->evas);
        e_popup_show(popw->pop);

        if (popup_warn)
           *popup_warn = popw;

        _popups_warn = evas_list_append(_popups_warn, popw);

        ret = 1;
     }

   edje_object_signal_emit(popw->face, "update", "devian");

   return ret;
}

/**
 * Delete the warning popup of a dEvian
 *
 * @param popw The popup
 */
void DEVIANF(popup_warn_del) (Popup_Warn *popw)
{
   if (!popw)
      return;

   /* Extra */
   switch (popw->type)
     {
        Evas_List *l;
        DEVIANN *devian;

     case POPUP_WARN_TYPE_DEVIAN:
        devian = (DEVIANN *)popw->data;
        devian->popup_warn = NULL;
        /* Timer */
        if (popw->timer)
           ecore_timer_del(popw->timer);
        /* Log */
        for (l = popw->log; l; l = evas_list_next(l))
           evas_stringshare_del(l->data);
        evas_list_free(popw->log);
        break;

     case POPUP_WARN_TYPE_INFO_TIMER:
        /* Timer */
        ecore_timer_del(popw->timer);
        break;
     }

   /* Pop */
   if (popw->pop)
      e_object_del(E_OBJECT(popw->pop));

   /* Name */
   if (popw->name)
      evas_stringshare_del(popw->name);

   /* Face */
   if (popw->face)
      evas_object_del(popw->face);

   _popups_warn = evas_list_remove(_popups_warn, popw);

   E_FREE(popw);
}

/**
 * Del all popups
 */
void DEVIANF(popup_warn_del_all) (void)
{
   Evas_List *l;
   Popup_Warn *p;

   for (l = _popups_warn; l; l = evas_list_next(l))
     {
        p = evas_list_data(l);
        DEVIANF(popup_warn_del) (p);
     }
   evas_list_free(_popups_warn);
}

/**
 * Desactivate popups for dEvian events
 * and delete them
 */
void DEVIANF(popup_warn_devian_desactivate) (void)
{
   Evas_List *l;
   DEVIANN *devian;

#ifdef HAVE_RSS
   DEVIANM->conf->sources_rss_popup_news = 0;
#endif
   /* Delete all devians' popups */
   for (l = DEVIANM->devians; l; l = evas_list_next(l))
     {
        devian = evas_list_data(l);
        if (devian->popup_warn)
           DEVIANF(popup_warn_del) (devian->popup_warn);
     }
}

/**
 * Change the theme of active popups (TODO ...)
 */
void DEVIANF(popup_warn_theme_change) (void)
{

}

/* PRIVATE FUNCTIONS */

static void
_update(Popup_Warn *popw, const char *text)
{
   char buf[3];

   /* Name */
   switch (popw->type)
     {
        DEVIANN *devian;
        char *tmp;
        int times;

     case POPUP_WARN_TYPE_DEVIAN:
        devian = (DEVIANN *)popw->data;
        tmp = DEVIANF(source_name_get) (devian, -1);
        if (strcmp(popw->name, tmp))
          {
             evas_stringshare_del(popw->name);
             popw->name = evas_stringshare_add(tmp);
             edje_object_part_text_set(popw->face, "name", popw->name);
          }
        /* Log */
        popw->log = evas_list_append(popw->log, evas_stringshare_add(text));
        times = evas_list_count(popw->log);
        snprintf(buf, sizeof(buf), "%d", times);
        edje_object_part_text_set(popw->face, "times", buf);
        /* Timer */
        if (popw->timer)
           ecore_timer_del(popw->timer);
#ifdef HAVE_RSS
        if (DEVIANM->conf->sources_rss_popup_news_timer)
           popw->timer = ecore_timer_add(DEVIANM->conf->sources_rss_popup_news_timer, _cb_timer, popw);
#endif
        break;

     case POPUP_WARN_TYPE_INFO_TIMER:
        /* Timer */
        ecore_timer_del(popw->timer);
        popw->timer = ecore_timer_add(popw->timer_org, _cb_timer, popw);
        break;
     }

   edje_object_part_text_set(popw->face, "text", text);
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
             (p->y >= *py) && (p->y <= pyh)) || ((*px >= p->x) && (*px <= p_xw) && (*py >= p->y) && (*py <= p_yh)))
          {
             /* Overlap ! Correct coords */
             /* Try upper, and then on the left */
             //...TODO: Try down and right, maybe placement policy ?
             DPOPW(("Overlap !"));
             *py = *py - (*ph + POPUP_WARN_OVERLAP_BORDER);
             if (*py < 0)
               {
                  *py = org_y;
                  *px = *px - (*px + POPUP_WARN_OVERLAP_BORDER);
                  if (*px < 0)
                    {
                       break;
                    }
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
   DEVIANF(popup_warn_del) (popw);

   return 0;
}

static void
_cb_edje_close(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Popup_Warn *popw;

   popw = data;
   DEVIANF(popup_warn_del) (popw);
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

   /* Name */
   switch (popw->type)
     {
     case POPUP_WARN_TYPE_DEVIAN:
        times = evas_list_count(popw->log);
        if (times == 1)
          {
             DEVIANF(popup_warn_del) (popw);
             return;
          }
        /* Log */
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
        /* Timer */
        if (popw->timer)
           ecore_timer_del(popw->timer);
#ifdef HAVE_RSS
        if (DEVIANM->conf->sources_rss_popup_news_timer)
           popw->timer = ecore_timer_add(DEVIANM->conf->sources_rss_popup_news_timer, _cb_timer, popw);
#endif
        break;
     }
}

static void
_cb_edje_desactivate(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Popup_Warn *popw;

   popw = data;

   /* Name */
   switch (popw->type)
     {
     case POPUP_WARN_TYPE_DEVIAN:
        DEVIANF(popup_warn_devian_desactivate) ();
        break;
     }
}
