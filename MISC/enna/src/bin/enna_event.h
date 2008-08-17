#ifndef _ENNA_EVENT_H
#define _ENNA_EVENT_H

#include "enna.h"

typedef enum _enna_event enna_event;

enum _enna_event
{
   enna_event_down,
   enna_event_right,
   enna_event_left,
   enna_event_up,
   enna_event_escape,
   enna_event_exit,
   enna_event_enter,
   enna_event_addtopl,
   enna_event_home,
   enna_event_end,
   enna_event_stop,
   enna_event_play,
   enna_event_next,
   enna_event_prev,
   enna_event_pause,
   enna_event_fastforward,
   enna_event_rewind,
   enna_event_aspect,
   enna_event_language,
   enna_event_mute,
   enna_event_fullscreen,
   /*enna_event_a,
    * enna_event_b,
    * enna_event_c,
    * enna_event_d,
    * enna_event_e,
    * enna_event_f,
    * enna_event_g,
    * enna_event_h,
    * enna_event_i,
    * enna_event_j,
    * enna_event_k,
    * enna_event_l,
    * enna_event_m,
    * enna_event_n,
    * enna_event_o,
    * enna_event_p,
    * enna_event_q,
    * enna_event_r,
    * enna_event_s,
    * enna_event_t,
    * enna_event_u,
    * enna_event_v,
    * enna_event_w,
    * enna_event_x,
    * enna_event_y,
    * enna_event_z, */
   enna_event_0,
   enna_event_1,
   enna_event_2,
   enna_event_3,
   enna_event_4,
   enna_event_5,
   enna_event_6,
   enna_event_7,
   enna_event_8,
   enna_event_9,
   enna_event_none
};

EAPI void           enna_event_lirc_code(void *data, char *code);
EAPI void           enna_event_bg_key_down_cb(void *data, Evas * e,
					      Evas_Object * obj,
					      void *event_info);
EAPI void           enna_event_process_event(Enna * enna, enna_event event);
EAPI void           enna_togle_fullscreen(Enna * enna);
EAPI void           enna_event_init(Enna * enna);
#endif
