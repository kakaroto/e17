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
Elm_Callback *_elm_callback_new(void);
void _elm_callback_call(Elm_Obj *obj, Elm_Callback_Type, void *info);

extern Elm_Engine _elm_engine;

#endif
