#include "dEvian.h"

static int _gadman_init(Container_Box *box);
static void _gadman_shutdown(Container_Box *box);
static void _gadman_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);

static void _animation_stop(Container_Box *box);

static void _cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void _cb_edje_part_change(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_part_change_start(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_part_change_stop(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_part_change_wanted(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_part_previous_wanted(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_set_bg(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_edje_messages(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg);

static int _cb_ecore_animator(void *data);
static int _cb_timer_anim_ghost(void *data);


/* PUBLIC FUNCTIONS */

/**
 * Put the devian in a box
 *
 * @param devian The dEvian
 * @param edje_part The edje part where to start, -1 for default
 * @return 0 on success, 1 on fail
 */
int DEVIANF(container_box_add) (DEVIANN *devian, int edje_part)
{
   Container_Box *box;

   box = E_NEW(Container_Box, 1);

   box->devian = devian;
   box->gmc = NULL;

   DMAIN(("init container box START"));

   box->layer = DEVIANM->conf->boxs_stacking;

   /* edje object */
   box->edje = edje_object_add(DEVIANM->container->bg_evas);
   if (!(box->theme = DEVIANF(container_edje_load) (box->edje, "devian/box", devian, CONTAINER_BOX, box)))
     {
        DEVIANF(container_box_del) (box);
        return 1;
     }
   evas_object_layer_set(box->edje, box->layer);
   /* default edje part : set the other one, because we are going to do
    * an edje_part_change at the end of the container's creation */
   if (edje_part == -1)
     box->edje_part = !CONTAINER_BOX_EDJE_PART_DEFAULT;
   else
     box->edje_part = !edje_part;

   box->in_transition = 0;
   box->infos_visible = 0;

   /* messages from edje */
   edje_object_message_handler_set(box->edje, _cb_edje_messages, box);

   /* init show name & id */
   DEVIANF(container_box_comments_display_set) (box);
   DEVIANF(container_box_update_id_devian) (box);

   /* infos scrollframe */
   if (!DEVIANF(container_box_infos_init) (box))
     {
        DEVIANF(container_box_del) (box);
        return 1;
     }

   /* callbacks for mouse events on box */
   evas_object_event_callback_add(box->edje, EVAS_CALLBACK_MOUSE_DOWN, _cb_mouse_down, box);
   evas_object_event_callback_add(box->edje, EVAS_CALLBACK_MOUSE_IN, _cb_mouse_in, box);
   evas_object_event_callback_add(box->edje, EVAS_CALLBACK_MOUSE_OUT, _cb_mouse_out, box);

   /* callbacks for controls on edje */
   edje_object_signal_callback_add(box->edje, "pause", "button_playpause", _cb_edje_part_change_stop, box);
   edje_object_signal_callback_add(box->edje, "play", "button_playpause", _cb_edje_part_change_start, box);
   edje_object_signal_callback_add(box->edje, "set_bg", "button_set_bg", _cb_edje_set_bg, box);
   edje_object_signal_callback_add(box->edje, "refresh", "button_refresh", _cb_edje_part_change_wanted, box);
   edje_object_signal_callback_add(box->edje, "previous", "button_previous", _cb_edje_part_previous_wanted, box);

   /*position, size and alpha */
   box->x = devian->conf->box_x - box->theme_border_w / 2;
   box->y = devian->conf->box_y - box->theme_border_h / 2;
   box->w = 0;
   box->h = 0;
   box->go_w = devian->conf->box_w + box->theme_border_w;
   box->go_h = devian->conf->box_h + box->theme_border_h;
   box->max_size = devian->conf->box_max_size_source + box->theme_border_h;
   evas_object_move(box->edje, box->x, box->y);
   DEVIANF(container_box_alpha_set) (box, devian->conf->box_alpha);

   /* show edje */
   evas_object_show(box->edje);

   /* disable warning indicator */
   DEVIANF(container_box_warning_indicator_change) (box, 0);

   /* show we are waiting for a source */
   DEVIANF(container_box_loading_state_change) (box, 1);

   /* animation */
   box->anim = E_NEW(Container_Box_Anim, 1);
   DEVIANF(container_box_animation_start) (box, box->devian->conf->box_anim);

   /* gadman */
   _gadman_init(box);

   /* attach the object to the devian */
   devian->conf->container_type = CONTAINER_BOX;
   devian->container = box;

   /* set gui actions */
   DEVIANF(container_box_update_actions) (devian);

   /* transition and resize */
   if (devian->source)
     {
        DEVIANF(container_box_edje_part_change) (box);
        DEVIANF(container_box_resize_auto) (devian);
     }

   /* actions */
   devian->container_func.resize_auto = DEVIANF(container_box_resize_auto);
   devian->container_func.update_actions = DEVIANF(container_box_update_actions);
   devian->container_func.is_in_transition = DEVIANF(container_box_is_in_transition);
   devian->container_func.alpha_set = DEVIANF(container_box_alpha_set);

   DCONTAINER(("init container box OK"));

   return 0;
}

void DEVIANF(container_box_del) (Container_Box *container)
{
   DEVIANN *devian;
   Evas_Object *source0, *source1;

   devian = container->devian;

   devian->conf->container_type = CONTAINER_NO;
   devian->container = NULL;

   /* gadman */
   _gadman_shutdown(container);

   /* callbacks for mouse events */
   evas_object_event_callback_del(container->edje, EVAS_CALLBACK_MOUSE_DOWN, _cb_mouse_down);
   evas_object_event_callback_del(container->edje, EVAS_CALLBACK_MOUSE_IN, _cb_mouse_in);
   evas_object_event_callback_del(container->edje, EVAS_CALLBACK_MOUSE_OUT, _cb_mouse_out);

   /* callbacks for controls */
   edje_object_signal_callback_del(container->edje, "pause", "button_playpause", _cb_edje_part_change_stop);
   edje_object_signal_callback_del(container->edje, "play", "button_playpause", _cb_edje_part_change_start);
   edje_object_signal_callback_del(container->edje, "set", "button_background", _cb_edje_set_bg);
   edje_object_signal_callback_del(container->edje, "refresh", "button_refresh", _cb_edje_part_change_wanted);
   edje_object_signal_callback_del(container->edje, "previous", "button_previous", _cb_edje_part_previous_wanted);

   /* animation */
   if (container->animator)
     {
        if (container == ecore_animator_del(container->animator))
          {
             DCONTAINER(("Ecore animator deletion ok"));
             container->animator = NULL;
          }
     }
   DEVIANF(container_box_animation_start) (container, CONTAINER_BOX_ANIM_NO);
   E_FREE(container->anim);

   /* source */
   if (DEVIANF(source_evas_object_get) (devian, &source0, &source1))
     {
        evas_object_hide(source0);
        evas_object_hide(source1);
     }

   /* edje object */
   if (container->theme)
     evas_stringshare_del(container->theme);
   if (container->edje)
     {
        evas_object_hide(container->edje);
        evas_object_del(container->edje);
     }

   /* infos object */
   DEVIANF(container_box_infos_shutdown) (container);

   DEVIANM->container_box_count--;

   E_FREE(container);
}

void DEVIANF(container_box_update_actions) (DEVIANN *devian)
{
   Container_Box *box;
   int action;

   box = devian->container;

   if (devian->source)
     {
        if (devian->source_info.paused)
          action = 0;
        else
          action = 1;
        edje_object_message_send(box->edje, EDJE_MESSAGE_INT, DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_PART_CHANGE_SET, &action);

        if (devian->source_info.provide_set_bg)
          action = 1;
        else
          action = 0;
        edje_object_message_send(box->edje, EDJE_MESSAGE_INT, DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_SHOW_SET_BG, &action);

        if (devian->source_info.provide_previous)
          action = 1;
        else
          action = 0;
        edje_object_message_send(box->edje, EDJE_MESSAGE_INT, DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_SHOW_PREVIOUS, &action);

        DEVIANF(container_box_infos_text_change_set) (box, box->devian->conf->box_infos_show);
     }
}

void DEVIANF(container_box_alpha_set) (void *container, int alpha)
{
   Container_Box *box;

   box = container;
   evas_object_color_set(box->edje, 255, 255, 255, alpha);
   box->alpha = alpha;
}

void DEVIANF(container_box_resize_auto) (DEVIANN *devian)
{
   Container_Box *box;
   int w, h, w_org, h_org;
   double w2, h2;
   int max_w, max_h;
   double thm_w, thm_h;

   box = devian->container;

   /* calc the size */

   if (!box->devian->dying)
     {
        max_w = box->devian->conf->box_max_size_source;
        max_h = box->devian->conf->box_max_size_source;
        thm_w = box->theme_border_w;
        thm_h = box->theme_border_h;

        /* get the new size in (w h) */
        if (box->devian->conf->box_auto_resize)
          {
             double ratio;

             if (!DEVIANF(source_original_size_get) (box->devian, box->edje_part, &w_org, &h_org))
               {
                  DCONTAINER(("Cant get source's size !!!"));
                  w_org = CONTAINER_BOX_SIZE_MIN;
                  h_org = CONTAINER_BOX_SIZE_MIN;
               }
             if (box->devian->size_policy == SIZE_POLICY_AUTO)
               {
                  DCONTAINER((" - Auto resize - (im %dx%d) (max %dx%d) (r %f %f)", w, h, max_w, max_h, thm_w, thm_h));

                  /* box size limited to max box size, keeping ratio
                   * size of edje <-> size of the source
                   * what size the box should have to perfectly fit the source */

                  w2 = (double)w_org;
                  h2 = (double)h_org;

                  {
                     ratio = h2 / w2;
                     w2 = (double)max_w;
                     h2 = w2 * ratio;
                  }
                  if ((int)h2 > max_h)
                    {
                       ratio = w2 / h2;
                       h2 = (double)max_h;
                       w2 = h2 * ratio;
                    }

                  w = (int)w2 + thm_w;
                  h = (int)h2 + thm_h;
               }
             else
               {
                  if (box->devian->size_policy == SIZE_POLICY_USER)
                    {
                       //w = w_org; // ...CHANGE to this
                       //when original_size_get of rss will be ok
                       w = box->go_w;   // ...REMOVE
                       h = max_h + thm_h;
                    }
               }
          }
        else
          {
             if (box->devian->size_policy == SIZE_POLICY_AUTO)
               {
                  DCONTAINER((" - NO Auto resize -"));
                  w = max_w + thm_w;
                  h = max_h + thm_h;
               }
             else
               {
                  if (box->devian->size_policy == SIZE_POLICY_USER)
                    {
                       w = box->go_w;
                       h = max_h + thm_h;
                    }
               }
          }
        DCONTAINER(("Box new size : %dx%d", w, h));

        /* check the size is correct */

        if (w < CONTAINER_BOX_SIZE_MIN + thm_w)
          w = CONTAINER_BOX_SIZE_MIN + thm_w;
        if (h < CONTAINER_BOX_SIZE_MIN + thm_h)
          h = CONTAINER_BOX_SIZE_MIN + thm_h;
        if (w > CONTAINER_BOX_SIZE_MAX + thm_w)
          w = CONTAINER_BOX_SIZE_MAX + thm_w;
        if (h > CONTAINER_BOX_SIZE_MAX + thm_h)
          h = CONTAINER_BOX_SIZE_MAX + thm_h;

        if ((box->w == w) && (box->h == h))
          return;

        /* we are going to this new (w h) size */

        box->go_w = w;
        box->go_h = h;
     }

   /* set the size */

   if (DEVIANM->conf->boxs_nice_resize)
     {
        /* nice resize */
        box->in_resize = 1;
        if (!box->animator)
          {
             box->animator = ecore_animator_add(_cb_ecore_animator, box);
          }
     }
   else
     {
        /* quick resize */
        if (((box->go_w == 0) || (box->go_h == 0)) && box->devian->dying)
          {
             DEVIANF(devian_del) (box->devian, 1);
             return;
          }
        box->w = box->go_w;
        box->h = box->go_h;
        if (box->gmc)
          {
             box->gmc->w = box->w;
             box->gmc->h = box->h;
          }
        evas_object_resize(box->edje, box->w, box->h);
     }

   return;
}

int DEVIANF(container_box_is_in_transition) (DEVIANN *devian)
{
   Container_Box *box;

   box = devian->container;

   return box->in_transition;
}

int DEVIANF(container_box_edje_part_get) (Container_Box *container)
{
   return container->edje_part;
}

int DEVIANF(container_box_edje_part_change) (Container_Box *container)
{
   Evas_Object *source0, *source1;
   char *source_name;
   int w, h;

   if (!DEVIANF(source_evas_object_get) (container->devian, &source0, &source1))
     return 0;

   if (container->in_transition)
     DCONTAINER(("Already in transition ! bur starting a new one =)"));

   /* change the name displayed */
   source_name = NULL;
   source_name = DEVIANF(source_name_get) (container->devian, !container->edje_part);
   if (source_name)
     {
        edje_object_part_text_set(container->edje, "name", source_name);
     }
   else
     {
        edje_object_part_text_set(container->edje, "name", "???");
     }

   /* support double buf ? */
   if (!container->devian->source_info.provide_double_buf)
     {
        if (!container->edje_part)
          {
             /* already on good part, apply and exit */
             edje_object_part_swallow(container->edje, "source0", source0);
             evas_object_show(source0);
             DEVIANF(container_box_loading_state_change) (container, 0);
             return 1;
          }
        /* change to the part0 */
     }

   /* start transition */
   if (!container->edje_part)
     {
        container->edje_part = 1;
        if (container->gmc)
          {
             if (container->devian->size_policy == SIZE_POLICY_AUTO)
               {
                  if (DEVIANF(source_original_size_get) (container->devian, 1, &w, &h))
                    e_gadman_client_aspect_set(container->gmc, ((float)w / (float)h), ((float)w / (float)h));
               }
             else
               {
                  e_gadman_client_aspect_set(container->gmc, 0.0, 9999999.0);
               }
          }

        edje_object_part_swallow(container->edje, "source1", source1);
        evas_object_show(source1);
        container->in_transition = 1;
        if (DEVIANM->conf->boxs_nice_trans)
          {
             edje_object_signal_emit(container->edje, "source_transition_0-1_go", "");
             edje_object_signal_callback_add(container->edje, "source_transition_0-1_end", "th", _cb_edje_part_change, container);
          }
        else
          {
             edje_object_signal_emit(container->edje, "source_transition_q_0-1_go", "");
             edje_object_signal_callback_add(container->edje, "source_transition_q_0-1_end", "th", _cb_edje_part_change, container);
          }
        DCONTAINER(("Begining of transition 0-1"));
     }
   else
     {
        container->edje_part = 0;
        if (container->gmc)
          {
             if (container->devian->size_policy == SIZE_POLICY_AUTO)
               {
                  if (DEVIANF(source_original_size_get) (container->devian, 0, &w, &h))
                    e_gadman_client_aspect_set(container->gmc, ((float)w / (float)h), ((float)w / (float)h));
               }
             else
               {
                  e_gadman_client_aspect_set(container->gmc, 0.0, 9999999.0);
               }
          }
        edje_object_part_swallow(container->edje, "source0", source0);
        evas_object_show(source0);
        container->in_transition = 1;
        if (DEVIANM->conf->boxs_nice_trans)
          {
             edje_object_signal_emit(container->edje, "source_transition_1-0_go", "");
             edje_object_signal_callback_add(container->edje, "source_transition_1-0_end", "th", _cb_edje_part_change, container);
          }
        else
          {
             edje_object_signal_emit(container->edje, "source_transition_q_1-0_go", "");
             edje_object_signal_callback_add(container->edje, "source_transition_q_1-0_end", "th", _cb_edje_part_change, container);
          }
        DCONTAINER(("Begining of transition 1-0"));
     }

   return 1;
}

int DEVIANF(container_box_infos_init) (Container_Box *box)
{
   Evas_Object *scrollframe;
   Evas_Object *tb;
   Evas_Textblock_Style *tb_style;
   const char *tmp;

   /* get the initial infos size */
   tmp = edje_object_data_get(box->edje, "infos_initial_size");
   if (!tmp)
     {
        printf("Theme is not valid, NULL data get !!!\n");
        return 0;
     }
   if (!(box->infos_scrollframe_w = atoi(tmp)))
     {
        printf("Theme is not valid, Data get == 0 !!!\n");
        return 0;
     }

   /* init the textblock */
   tb = evas_object_textblock_add(DEVIANM->container->bg_evas);
   tb_style = evas_textblock_style_new();
   evas_textblock_style_set(tb_style, "DEFAULT='font=Vera font_size=10 align=left color=#000000ff wrap=word'" "br='\n'");
   evas_object_textblock_style_set(tb, tb_style);
   evas_textblock_style_free(tb_style);

   scrollframe = e_scrollframe_add(DEVIANM->container->bg_evas);
   e_scrollframe_child_set(scrollframe, tb);

   edje_object_part_swallow(box->edje, "infos", scrollframe);
   evas_object_show(tb);
   evas_object_show(scrollframe);

   box->infos_tb = tb;
   box->infos_scrollframe = scrollframe;

   DEVIANF(container_box_infos_text_change_set) (box, box->devian->conf->box_infos_show);
   DEVIANF(container_box_infos_pos_set) (box);
   DEVIANF(container_box_infos_text_change) (box, "<br>Ooo Oo Oo Oo Oo oOo o<br> Waiting for source ...");

   return 1;
}

int DEVIANF(container_box_infos_shutdown) (Container_Box *box)
{
   edje_object_part_unswallow(box->edje, box->infos_scrollframe);

   evas_object_del(box->infos_tb);
   evas_object_del(box->infos_scrollframe);

   return 1;
}

int DEVIANF(container_box_infos_text_change) (Container_Box *box, char *text)
{
   int w, h;

   if (!box->infos_visible)
     return 0;

   if (!text)
     text = DEVIANF(source_infos_get) (box->devian, -1);
   if (!text)
     text = strdup("No informations about source");

   evas_object_textblock_clear(box->infos_tb);
   evas_object_resize(box->infos_tb, box->infos_scrollframe_w - 10, 20);

   evas_object_textblock_text_markup_set(box->infos_tb, text);

   evas_object_textblock_size_formatted_get(box->infos_tb, &w, &h);
   evas_object_resize(box->infos_tb, box->infos_scrollframe_w - 10, h);

   DCONTAINER(("Tb Infos: w:%d h:%d", w, h));
   DCONTAINER(("Infos edje part changed to:\n%s", text));

   E_FREE(text);

   /* go to first page */
   e_scrollframe_child_pos_set(box->infos_scrollframe, 0, 0);

   /* debug */
   /*
    * {
    * int x, y;
    * int x2, y2, x3, y3;
    * int w, h, w2, h2;
    * e_scrollframe_child_pos_get(box->infos_scrollframe, &x, &y);
    * e_scrollframe_page_size_get(box->infos_scrollframe, &x3, &y3);
    * e_scrollframe_child_viewport_size_get(box->infos_scrollframe, &w2, &h2);
    * evas_object_textblock_size_native_get(box->infos_tb, &w, &h);
    * DCONTAINER(("Scroll infos get: %d x:%d y:%d step_w:%d step_h:%d page_w:%d page_h:%d viewport_w:%d viewport_h:%d tb_w:%d tb_h:%d", box->infos_scrollframe_w, x, y, x2, y2, x3, y3, w2, h2, w, h));
    * }
    */

   return 1;
}

void DEVIANF(container_box_infos_text_scroll) (Container_Box *box)
{
   int whole_w, whole_h;
   int tb_w, tb_h;
   int pos_x, pos_y;

   if (!box->infos_visible)
     return;

   evas_object_geometry_get(box->infos_scrollframe, NULL, NULL, &whole_w, &whole_h);
   e_scrollframe_child_pos_get(box->infos_scrollframe, &pos_x, &pos_y);
   evas_object_geometry_get(box->infos_tb, NULL, NULL, &tb_w, &tb_h);

   if ((tb_h - pos_y) > (whole_h - 15))
     e_scrollframe_child_pos_set(box->infos_scrollframe, pos_x, pos_y + (whole_h - 50));       /* -> Next page */
   else if (pos_y)
     e_scrollframe_child_pos_set(box->infos_scrollframe, pos_x, 0);    /* -> First page */
}

/**
 * Tell to the container wether or not it can display the information panel
 *
 * @param box The box concerned
 * @param action The action to perform
 */
void DEVIANF(container_box_infos_text_change_set) (Container_Box *box, int action)
{
   if (box->devian->source_info.allow_info_panel)
     {
        box->devian->conf->box_infos_show = action;
     }
   else
     action = 0;

   DCONTAINER(("Message infos change set send (%d)", action));
   edje_object_message_send(box->edje, EDJE_MESSAGE_INT, DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_INFOS_CHANGE_SET, &action);
}

void DEVIANF(container_box_infos_pos_set) (Container_Box *box)
{
   edje_object_message_send(box->edje, EDJE_MESSAGE_INT,
                            DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_INFOS_POS_SET, &box->devian->conf->box_infos_pos);
}

void DEVIANF(container_box_loading_state_change) (Container_Box *box, int state)
{
   edje_object_message_send(box->edje, EDJE_MESSAGE_INT, DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_LOADING_STATE, &state);
}

void DEVIANF(container_box_warning_indicator_change) (Container_Box *box, int state)
{
   edje_object_message_send(box->edje, EDJE_MESSAGE_INT, DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_WARNING_INDICATOR, &state);
}

int DEVIANF(container_box_devian_dying) (Container_Box *box)
{
   box->go_w = 0;
   box->go_h = 0;
   DEVIANF(container_box_resize_auto) (box->devian);
   return 1;
}

int DEVIANF(container_box_update_id_devian) (Container_Box *box)
{
   if (!box->devian->id)
     return 0;

   edje_object_part_text_set(box->edje, "id", box->devian->id);

   return 1;
}

void DEVIANF(container_box_comments_display_set) (Container_Box *box)
{
   if (DEVIANM->conf->boxs_always_id)
     edje_object_signal_emit(box->edje, "id_always_activate", "");
   else
     edje_object_signal_emit(box->edje, "id_always_deactivate", "");
   if (DEVIANM->conf->boxs_always_name)
     edje_object_signal_emit(box->edje, "name_always_activate", "");
   else
     edje_object_signal_emit(box->edje, "name_always_deactivate", "");
}

/*
  Start a new animation of on a box
  Stop all other anim / restart current anim
  Args:
  - box
  - number of the anim
  Returns:
  - 1 if the animation has been set
  - 0 otherwise
*/
int DEVIANF(container_box_animation_start) (Container_Box *box, int anim_num)
{
   _animation_stop(box);

   switch (anim_num)
     {

     case CONTAINER_BOX_ANIM_NO:
        {
           evas_object_color_set(box->edje, 255, 255, 255, box->alpha);
           break;
        }

     case CONTAINER_BOX_ANIM_LINE:
        {
           evas_object_color_set(box->edje, 255, 255, 255, box->alpha);
           break;
        }

     case CONTAINER_BOX_ANIM_GOULOUM:
        {
           evas_object_color_set(box->edje, 255, 255, 255, box->alpha);
           box->anim->data[0] = 0;
           box->anim->data[1] = 1;
           box->anim->data[2] = 1;
           break;
        }

     case CONTAINER_BOX_ANIM_GHOST:
        {
           /* in fade */
           box->anim->data[1] = 0;
           /* 0: fade->trans, 1: fade->visible */
           box->anim->data[2] = 1;
           /* alpha */
           evas_object_color_set(box->edje, 255, 255, 255, box->alpha);
           box->anim->data[3] = box->alpha;
           /* timer */
           if (box->anim->timer)
             ecore_timer_del(box->anim->timer);
           box->anim->timer = ecore_timer_add(DEVIANM->conf->boxs_anim_ghost_timer, _cb_timer_anim_ghost, box);
           break;
        }

     default:
        {
           return 0;
        }

     }

   DCONTAINER(("Start animation %d", anim_num));

   box->devian->conf->box_anim = anim_num;

   if (anim_num)
     {
        if (!box->animator && (anim_num != CONTAINER_BOX_ANIM_GHOST))
          box->animator = ecore_animator_add(_cb_ecore_animator, box);
        if (anim_num != CONTAINER_BOX_ANIM_GHOST)
          {
             if (box->anim->timer)
               {
                  ecore_timer_del(box->anim->timer);
                  box->anim->timer = NULL;
               }
          }
     }
   else
     {
        if (box->anim)
          if (box->anim->timer)
            {
               ecore_timer_del(box->anim->timer);
               box->anim->timer = NULL;
            }
     }

   return 1;
}

void DEVIANF(container_box_random_pos_get) (Container_Box *box, int *x, int *y, int max_size)
{
   if (!box)
     {
        *x = rand() % (DEVIANM->canvas_w - (max_size + CONTAINER_BOX_THEME_BORDER_MAX));
        *y = rand() % (DEVIANM->canvas_h - (max_size + CONTAINER_BOX_THEME_BORDER_MAX));
     }
   else
     {
        *x = rand() % (DEVIANM->canvas_w - (max_size + box->theme_border_w));
        *y = rand() % (DEVIANM->canvas_h - (max_size + box->theme_border_h));
     }
}

void DEVIANF(container_box_gadman_policy_update) (Container_Box *box)
{
   E_Gadman_Policy policy;

   if (!box->gmc)
     return;

   policy = E_GADMAN_POLICY_ANYWHERE |
      E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE | E_GADMAN_POLICY_HSIZE | E_GADMAN_POLICY_VSIZE;

   if (box->devian->conf->box_allow_overlap)
     policy |= E_GADMAN_POLICY_ALLOW_OVERLAP;
   else
     policy &= ~E_GADMAN_POLICY_ALLOW_OVERLAP;

   e_gadman_client_policy_set(box->gmc, policy);

}


/* PRIVATE FUNCTIONS */

static int
_gadman_init(Container_Box *box)
{
   box->gmc = e_gadman_client_new(DEVIANM->container->gadman);

   box->gmc_init = 1;

   e_gadman_client_domain_set(box->gmc, "module." MODULE_NAME_NOCASE, DEVIANM->container_box_count++);
   e_gadman_client_load(box->gmc);      /* Only to avoid segfault when del the gmc */

   e_gadman_client_zone_set(box->gmc, e_zone_current_get(DEVIANM->container));

   DEVIANF(container_box_gadman_policy_update) (box);

   e_gadman_client_min_size_set(box->gmc, CONTAINER_BOX_SIZE_MIN, CONTAINER_BOX_SIZE_MIN);
   e_gadman_client_max_size_set(box->gmc, CONTAINER_BOX_SIZE_MAX, CONTAINER_BOX_SIZE_MAX);
   e_gadman_client_auto_size_set(box->gmc, (box->w), (box->h));

   e_gadman_client_padding_set(box->gmc, 10, 10, 10, 10);
   e_gadman_client_resize(box->gmc, box->w, box->h);
   e_gadman_client_change_func_set(box->gmc, _gadman_change, box);
   box->gmc->x = box->x;
   box->gmc->y = box->y;

   return 1;
}

static void
_gadman_shutdown(Container_Box *box)
{
   if (!box->gmc)
     return;

   while (e_object_unref(E_OBJECT(box->gmc)) > 0);
}

static void
_gadman_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change)
{
   Container_Box *box;
   Evas_Coord x, y, w, h;

   box = data;

   switch (change)
     {

     case E_GADMAN_CHANGE_MOVE_RESIZE:
        if (box->gmc_init)
          {
             box->gmc_init = 0;
             break;
          }

        e_gadman_client_geometry_get(box->gmc, &x, &y, &w, &h);
        if (!box->devian->dying)
          {
             box->x = x;
             box->y = y;
             box->w = w;
             box->h = h;

             box->go_w = box->w;
             box->go_h = box->h;
             if (box->devian->size_policy == SIZE_POLICY_AUTO)
               {
                  if (w > h)
                    {
                       box->max_size = box->w;
                       box->devian->conf->box_max_size_source = box->w - box->theme_border_w;
                    }
                  else
                    {
                       box->max_size = box->h;
                       box->devian->conf->box_max_size_source = box->h - box->theme_border_h;
                    }
               }
             else
               {
                  if (box->devian->size_policy == SIZE_POLICY_USER)
                    {
                       box->max_size = box->h;
                       box->devian->conf->box_max_size_source = box->h - box->theme_border_h;
                    }
               }
             box->in_resize = 0;
          }

        DEVIANF(config_save) ();

        evas_object_move(box->edje, x, y);
        evas_object_resize(box->edje, w, h);
        break;

     case E_GADMAN_CHANGE_RAISE:
        evas_object_raise(box->edje);
        break;

     default:
        break;

     }

}

static void
_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Container_Box *box;

   box = data;
   ev = event_info;

   if (ev->button == 2)
     {
        box->layer = CONTAINER_BOX_STACKING_UNDER;
        evas_object_layer_set(box->edje, CONTAINER_BOX_STACKING_UNDER);
     }
   else
     {
        if (ev->button == 3)
          {
             e_menu_activate_mouse(box->devian->menu,
                                   e_zone_current_get(DEVIANM->container),
                                   ev->output.x, ev->output.y, 1, 1, E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
             e_util_container_fake_mouse_up_all_later(DEVIANM->container);
          }
     }
}

static void
_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Container_Box *box;

   box = data;

   if (box->layer < CONTAINER_BOX_STACKING_UPPER)
     {
        box->layer = CONTAINER_BOX_STACKING_UPPER;
        DCONTAINER(("mouse in"));
        evas_object_layer_set(box->edje, CONTAINER_BOX_STACKING_UPPER);
     }
}

static void
_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Container_Box *box;

   box = data;

   if ((box->layer != CONTAINER_BOX_STACKING_NORMAL))
     {
        box->layer = CONTAINER_BOX_STACKING_NORMAL;
        DCONTAINER(("mouse out"));
        evas_object_layer_set(box->edje, CONTAINER_BOX_STACKING_NORMAL);
     }
}

static void
_cb_edje_part_change(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Container_Box *container;
   DEVIANN *devian;
   Evas_Object *source0, *source1;

   DCONTAINER(("End of transition has begin"));

   container = data;
   devian = container->devian;

   if (!container->edje || !obj)
     {
        DCONTAINER(("NO EDJE !!!"));
        container->in_transition = 0;
        return;
     }

   if (!DEVIANF(source_evas_object_get) (devian, &source0, &source1))
     return;

   if (!strcmp(emission, "source_transition_0-1_end"))
     {
        if (devian->source_info.provide_double_buf)
          {
             evas_object_hide(source0);
             edje_object_part_unswallow(container->edje, source0);
          }
        edje_object_signal_callback_del(obj, "source_transition_0-1_end", "th", _cb_edje_part_change);
        container->in_transition = 0;
        DEVIANF(source_detach) (devian, 0);
     }
   else
     {
        if (!strcmp(emission, "source_transition_q_0-1_end"))
          {
             if (devian->source_info.provide_double_buf)
               {
                  evas_object_hide(source0);
                  edje_object_part_unswallow(container->edje, source0);
               }
             edje_object_signal_callback_del(obj, "source_transition_q_0-1_end", "th", _cb_edje_part_change);
             container->in_transition = 0;
             DEVIANF(source_detach) (devian, 0);
          }
        else
          {
             if (!strcmp(emission, "source_transition_1-0_end"))
               {
                  if (devian->source_info.provide_double_buf)
                    {
                       evas_object_hide(source1);
                       edje_object_part_unswallow(container->edje, source1);
                    }
                  edje_object_signal_callback_del(obj, "source_transition_1-0_end", "th", _cb_edje_part_change);
                  container->in_transition = 0;
                  DEVIANF(source_detach) (devian, 1);
               }
             else
               {
                  if (!strcmp(emission, "source_transition_q_1-0_end"))
                    {
                       if (devian->source_info.provide_double_buf)
                         {
                            evas_object_hide(source1);
                            edje_object_part_unswallow(container->edje, source1);
                         }
                       edje_object_signal_callback_del(obj, "source_transition_q_1-0_end", "th", _cb_edje_part_change);
                       container->in_transition = 0;
                       DEVIANF(source_detach) (devian, 1);
                    }
                  else
                    printf(MODULE_NAME ": Get unknow edje transition signal !\n");
               }
          }
     }

   DEVIANF(container_box_loading_state_change) (container, 0);

   DCONTAINER(("End of transition"));
}

static void
_cb_edje_part_change_start(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Container_Box *box;

   box = (Container_Box *)data;

   DCONTAINER(("Edje cb part_change_start"));
   box->devian->source_func.timer_change(box->devian, 1, 0);
}

static void
_cb_edje_part_change_stop(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Container_Box *box;

   box = (Container_Box *)data;

   DCONTAINER(("Edje cb part_change_stop"));
   box->devian->source_func.timer_change(box->devian, 0, 0);
}

static void
_cb_edje_part_change_wanted(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   int i;
   Container_Box *box;

   box = (Container_Box *)data;

   DCONTAINER(("Edje cb part_change_wanted"));
   i = box->devian->source_func.refresh(box->devian, 0);
   DCONTAINER(("Edje cb: transition returns %d", i));
}

static void
_cb_edje_part_previous_wanted(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   int i;
   Container_Box *box;

   box = (Container_Box *)data;

   DCONTAINER(("Edje cb part_previous_wanted"));
   i = box->devian->source_func.refresh(box->devian, 1);
   DCONTAINER(("Edje cb: transition returns %d", i));
}

static void
_cb_edje_set_bg(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Container_Box *box;

   box = (Container_Box *)data;

   DCONTAINER(("Edje cb set_bg"));
   box->devian->source_func.set_bg(box->devian);
}

static void
_cb_edje_messages(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg)
{
   Container_Box *box;
   int *w;

   box = data;

   switch (id)
     {

     case DEVIAN_CONTAINER_BOX_FROM_EDJE_MSG_INFOS_W:
        if (type == EDJE_MESSAGE_INT)
          {
             w = msg;
             box->infos_scrollframe_w = *w;
             DCONTAINER(("Received message for scrollframe_w: %d", *w));
          }
        break;

     case DEVIAN_CONTAINER_BOX_FROM_EDJE_MSG_INFOS_VISIBLE:
        if (type == EDJE_MESSAGE_INT)
          {
             w = msg;
             box->infos_visible = *w;
             if (*w)
               DEVIANF(container_box_infos_text_change) (box, NULL);
             DCONTAINER(("Received message for infos visible: %d", *w));
          }
        break;
     }
}

/**
 * The animator for one Container_Box
 */
static int
_cb_ecore_animator(void *data)
{
   Container_Box *box;
   int spd;

   box = data;
   spd = box->devian->conf->box_speed;

   if (e_gadman_mode_get(box->gmc->gadman) == E_GADMAN_MODE_NORMAL)
     {
        /* resize */
        if (box->in_resize)
          {
             int diff_w, diff_h;
             int too_w, too_h;

             diff_w = box->w - box->go_w;
             diff_h = box->h - box->go_h;

             /* check: end of resize ? */
             if ((diff_w == 0) && (diff_h == 0))
               {
                  DCONTAINER(("Resize finished"));
                  box->in_resize = 0;
                  if (box->devian->dying)
                    {
                       DEVIANF(devian_del) (box->devian, 1);
                       return 0;
                    }
               }
             else
               {
                  /* resize */
                  if (diff_w > 3)
                    box->w = box->w - 3;
                  else
                    {
                       if (diff_w < -3)
                         box->w = box->w + 3;
                       else
                         box->w = box->go_w;
                    }
                  if (diff_h > 3)
                    box->h = box->h - 3;
                  else
                    {
                       if (diff_h < -3)
                         box->h = box->h + 3;
                       else
                         box->h = box->go_h;
                    }

                  /* out of the screen ? */
                  too_w = DEVIANM->canvas_w - (box->x + box->w);
                  if (too_w < 0)
                    {
                       box->x += too_w;
                       evas_object_move(box->edje, box->x, box->y);
                    }
                  too_h = DEVIANM->canvas_h - (box->y + box->h);
                  if (too_h < 0)
                    {
                       box->y += too_h;
                       evas_object_move(box->edje, box->x, box->y);
                    }

                  /* apply resize */
                  box->gmc->w = box->w;
                  box->gmc->h = box->h;
                  evas_object_resize(box->edje, box->w, box->h);

               }
          }

        /* animation */
        switch (box->devian->conf->box_anim)
          {
          case CONTAINER_BOX_ANIM_NO:
             break;
          case CONTAINER_BOX_ANIM_LINE:
             {
                box->x = box->x + spd;
                if ((box->x + box->w) > DEVIANM->canvas_w)
                  box->x = 0;

                box->gmc->x = box->x;
                box->gmc->y = box->y;
                evas_object_move(box->edje, box->x, box->y);

                break;
             }
          case CONTAINER_BOX_ANIM_GOULOUM:
             {
                box->x = box->x + box->anim->data[1] * spd;
                box->y = box->y + box->anim->data[2] * spd;
                if ((box->x + box->w) > DEVIANM->canvas_w)
                  box->anim->data[1] = -1;
                if ((box->x) < 0)
                  box->anim->data[1] = 1;
                if ((box->y + box->h) > DEVIANM->canvas_h)
                  box->anim->data[2] = -1;
                if ((box->y) < 0)
                  box->anim->data[2] = 1;

                box->gmc->x = box->x;
                box->gmc->y = box->y;
                evas_object_move(box->edje, box->x, box->y);

                break;
             }
          case CONTAINER_BOX_ANIM_GHOST:
             {
                if (box->anim->data[1])
                  {
                     if (!box->anim->data[2])
                       {
                          box->anim->data[3] = box->anim->data[3] - 10;
                          if (box->anim->data[3] < 0)
                            box->anim->data[3] = 0;
                          evas_object_color_set(box->edje, 255, 255, 255, box->anim->data[3]);
                          if (box->anim->data[3] == 0)
                            {
                               box->anim->data[2] = 1;
                               DEVIANF(container_box_random_pos_get) (box, &box->x, &box->y,
                                                                      box->devian->conf->box_max_size_source);
                               box->gmc->x = box->x;
                               box->gmc->y = box->y;
                               evas_object_move(box->edje, box->x, box->y);
                            }
                       }
                     else
                       {
                          box->anim->data[3] = box->anim->data[3] + 10;
                          if (box->anim->data[3] > box->alpha)
                            box->anim->data[3] = box->alpha;
                          evas_object_color_set(box->edje, 255, 255, 255, box->anim->data[3]);
                          if (box->anim->data[3] == box->alpha)
                            {
                               box->anim->data[1] = 0;
                            }
                       }
                  }
                break;
             }
          default:
             box->devian->conf->box_anim = CONTAINER_BOX_ANIM_NO;
          }

     }

   /* if there are no more animations for box , destroy ecore animator */
   if (!box->in_resize &&
       (!box->devian->conf->box_anim || (box->devian->conf->box_anim == CONTAINER_BOX_ANIM_GHOST && !box->anim->data[1])))
     {
        DCONTAINER(("> Stop anim callback <"));
        box->animator = NULL;
        return 0;
     }
   else
     return 1;
}

static int
_cb_timer_anim_ghost(void *data)
{
   Container_Box *box;

   box = (Container_Box *)data;

   if (!box->animator)
     box->animator = ecore_animator_add(_cb_ecore_animator, box);

   box->anim->data[1] = 1;
   box->anim->data[2] = !box->anim->data[2];

   return 1;
}

static void
_animation_stop(Container_Box *box)
{
   if (!box->anim)
     return;
   box->devian->conf->box_anim = CONTAINER_BOX_ANIM_NO;
}
