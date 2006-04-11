#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Container_Box Container_Box;
typedef struct _Container_Box_Anim Container_Box_Anim;

#else

#ifndef E_MOD_CONTAINER_BOX_H_INCLUDED
#define E_MOD_CONTAINER_BOX_H_INCLUDED

/* Default things */
#define CONTAINER_BOX_EDJE_PART_DEFAULT 0

#define CONTAINER_BOX_AUTO_RESIZE_DEFAULT 1
#define CONTAINER_BOX_SPEED_DEFAULT 1
#define CONTAINER_BOX_NICE_RESIZE_DEFAULT 1
#define CONTAINER_BOX_NICE_TRANS_DEFAULT 1

#define CONTAINER_BOX_SIZE_DEFAULT 150
#define CONTAINER_BOX_SIZE_MAX 700
#define CONTAINER_BOX_SIZE_MIN 50

#define CONTAINER_BOX_STACKING_DEFAULT 1
#define CONTAINER_BOX_STACKING_NORMAL 1
#define CONTAINER_BOX_STACKING_UNDER -1
#define CONTAINER_BOX_STACKING_UPPER 2

#define CONTAINER_BOX_ALWAYS_ID_DEFAULT 0
#define CONTAINER_BOX_ALWAYS_NAME_DEFAULT 1

#define CONTAINER_BOX_ANIM_DEFAULT 0
#define CONTAINER_BOX_ANIM_NO 0
#define CONTAINER_BOX_ANIM_LINE 1
#define CONTAINER_BOX_ANIM_GOULOUM 2
#define CONTAINER_BOX_ANIM_GHOST 3
#define CONTAINER_BOX_ANIM_BOINGBOING 4

#define CONTAINER_BOX_ANIM_GHOST_TIMER_DEFAULT 20

#define DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_INFOS_POS_SET 0
#define DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_PART_CHANGE_SET 1
#define DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_INFOS_CHANGE_SET 2
#define DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_LOADING_STATE 3
#define DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_SHOW_SET_BG 4
#define DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_SHOW_PREVIOUS 5
#define DEVIAN_CONTAINER_BOX_TO_EDJE_MSG_WARNING_INDICATOR 6
#define DEVIAN_CONTAINER_BOX_FROM_EDJE_MSG_INFOS_W 0
#define DEVIAN_CONTAINER_BOX_FROM_EDJE_MSG_INFOS_VISIBLE 1

#define CONTAINER_BOX_ALLOW_OVERLAP_DEFAULT 1

#define CONTAINER_BOX_INFOS_SHOW_DEFAULT 1
#define CONTAINER_BOX_INFOS_POS_DEFAULT 2

#define CONTAINER_BOX_THEME_BORDER_MAX 100

#define CONTAINER_BOX_ALPHA_DEFAULT 255

struct _Container_Box_Anim
{
   int data[20];
   Ecore_Timer *timer;
};

struct _Container_Box
{
   DEVIANN *devian;
   E_Gadman_Client *gmc;
   int gmc_init;

   Evas_Object *edje;
   const char *theme;

   Evas_Object *infos_scrollframe;
   int infos_scrollframe_w;
   Evas_Object *infos_tb;
   int edje_part;               /* part actualy show */
   Ecore_Animator *animator;
   Container_Box_Anim *anim;

   /* Box status */
   int in_transition;
   int in_resize;
   int infos_visible;

   /* Theme */
   int theme_border_w, theme_border_h;

   /* Box geometry */
   int max_size;
   int x, y;
   int w, h;
   int go_w, go_h;
   int layer;
   int alpha;
};

int DEVIANF(container_box_add) (DEVIANN *devian, int edje_part);
void DEVIANF(container_box_del) (Container_Box *container);
void DEVIANF(container_box_update_actions) (DEVIANN *devian);
void DEVIANF(container_box_resize_auto) (DEVIANN *devian);
void DEVIANF(container_box_alpha_set) (void *container, int alpha);
int DEVIANF(container_box_is_in_transition) (DEVIANN *devian);

int DEVIANF(container_box_edje_part_get) (Container_Box *container);
int DEVIANF(container_box_edje_part_change) (Container_Box *container);

int DEVIANF(container_box_infos_init) (Container_Box *box);
int DEVIANF(container_box_infos_shutdown) (Container_Box *box);
int DEVIANF(container_box_infos_text_change) (Container_Box *box, char *text);
void DEVIANF(container_box_infos_text_scroll) (Container_Box *box);
void DEVIANF(container_box_infos_text_change_set) (Container_Box *box, int action);
int DEVIANF(container_box_infos_edje_part_is_visible) (Container_Box *box);
void DEVIANF(container_box_infos_pos_set) (Container_Box *box);

void DEVIANF(container_box_loading_state_change) (Container_Box *box, int state);
void DEVIANF(container_box_warning_indicator_change) (Container_Box *box, int state);

int DEVIANF(container_box_devian_dying) (Container_Box *box);
int DEVIANF(container_box_update_id_devian) (Container_Box *box);
void DEVIANF(container_box_comments_display_set) (Container_Box *box);

int DEVIANF(container_box_animation_start) (Container_Box *box, int anim_num);
void DEVIANF(container_box_random_pos_get) (Container_Box *box, int *x, int *y, int max_size);

void DEVIANF(container_box_gadman_policy_update) (Container_Box *box);

#endif
#endif
