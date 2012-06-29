#ifndef EVKBD_PRIVATE
#define EVKBD_PRIVATE
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Ecore.h>

typedef struct _Smart_Data Smart_Data;

typedef struct _Ekbd_Int_Tie Ekbd_Int_Tie;
typedef struct _Ekbd_Int_Key Ekbd_Int_Key;
typedef struct _Ekbd_Int_Key_State Ekbd_Int_Key_State;


struct _Smart_Data
{
   Eina_List *layouts;
   Evas_Coord x, y, w, h;
   const char *theme;
   unsigned char aspect_fixed : 1;
   struct {
        char *directory;
        const char *file;
        int direction;
        int w, h;
        int fuzz;
        Eina_List *keys;
        int state;
        Ekbd_Int_Key *pressed;
   } layout;
   struct {
        Evas_Coord x, y, cx, cy;
        unsigned char down : 1;
        unsigned char hold : 1;
        unsigned char stroke : 1;
        Ekbd_Int_Tie *tie;
        Ecore_Timer *hold_timeout;
        Ecore_Timer *repeat;
        double trepeat;
   } down;
   Evas_Object *layout_obj, *event_obj, *cover_obj, *s_obj;
};

struct _Ekbd_Layout
{
   const char *path;
   const char *dir;
   const char *icon;
   const char *name;
   int type;
};

struct _Ekbd_Int_Tie
{
   Evas_Coord w, h;
   const char *file;
   int fuzz;
   Eina_List *keys;
   Ekbd_Int_Key *key;
   Evas_Object *base_obj;
};

struct _Ekbd_Int_Key
{
   int x, y, w, h, orig_x, orig_y, orig_w, orig_h;
   Eina_List *states;
   Evas_Object *obj, *icon_obj;

   Eina_Bool pressed : 1;
   Eina_Bool selected : 1;
   Eina_Bool is_shift : 1;
   Eina_Bool is_multi_shift : 1;
   Eina_Bool is_ctrl : 1;
   Eina_Bool is_alt : 1;
   Eina_Bool is_altgr : 1;
   Eina_Bool is_capslock : 1;
};

struct _Ekbd_Int_Key_State
{
   int state;
   const char *label, *icon;
   const char *out;
   const char *lp_out;
   Ekbd_Int_Tie *tie;
   Ekbd_Int_Tie *lp_tie;
   struct _Ekbd_Layout *layout;
};


#endif /* EVKD_PRIVATE */
