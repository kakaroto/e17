#ifndef E_MOD_TYPES_H
#define E_MOD_TYPES_H

typedef struct _Instance Instance;
typedef struct _Mixer Mixer;
typedef struct _Mixer_Win_Simple Mixer_Win_Simple;
typedef struct _Mixer_System Mixer_System;
typedef struct _Mixer_Card Mixer_Card;
typedef struct _Mixer_Channel Mixer_Channel;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Mixer           *mixer;
};

struct _Mixer
{
   Instance     *inst;
   Evas         *evas;
   Mixer_System *mix_sys;

   Mixer_Win_Simple *simple_win;
   Evas_Object *base;
};

struct _Mixer_Win_Simple
{
   Mixer       *mixer;
   E_Win       *window;
   
   Evas_Object *event_obj;
   Evas_Object *bg_obj;
   Evas_Object *slider;
   
   int          x, y, w, h;
   int          to_top;
   int          popped_up;
   double       start_time;

   Ecore_Animator *slide_animator;
};

struct _Mixer_System 
{
   Evas_List *(*get_cards)    (void);
   void      *(*get_card)     (int id);
   Evas_List *(*get_channels) (void *data);
   void      *(*get_channel)  (void *data, int card_id);

   int        (*set_volume)   (int card_id, int channel_id, int vol);
   int        (*get_volume)   (int card_id, int channel_id);
   
   void       (*free_cards)   (void *data);
   
   Evas_List *cards;
};

struct _Mixer_Card 
{
   int id, active;

   const char *name;
   const char *real;
   
   Evas_List  *channels;
};

struct _Mixer_Channel 
{
   int id, card_id;
   
   const char *name;
};

#endif
