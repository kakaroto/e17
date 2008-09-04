#ifndef ELM_PRIV_H
#define ELM_PRIV_H

typedef enum _Elm_Engine
{
   ELM_SOFTWARE_X11,
     ELM_SOFTWARE_FB,
     ELM_SOFTWARE_16_X11,
     ELM_XRENDER_X11,
     ELM_OPENGL_X11
} Elm_Engine;

#define ELM_NEW(t) calloc(1, sizeof(t))

void _elm_obj_init(Elm_Obj *obj);
void _elm_obj_nest_push(void);
void _elm_obj_nest_pop(void);
int _elm_obj_del_defer(Elm_Obj *obj);
Elm_Cb *_elm_cb_new(void);
void _elm_cb_call(Elm_Obj *obj, Elm_Cb_Type, void *info);

extern Elm_Engine _elm_engine;
extern char *_elm_appname;

extern Elm_Obj_Class _elm_obj_class;
extern Elm_Win_Class _elm_win_class;
  
#endif
