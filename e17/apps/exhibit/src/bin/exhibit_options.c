#include "exhibit.h"

Ex_Options *
_ex_options_new()
{
   Ex_Options *o;
   
   o = calloc(1, sizeof(Ex_Options));
   o->app1 =     NULL;
   o->app2 =     NULL;
   o->app3 =     NULL;
   o->app4 =     NULL;
   o->fav_path = NULL;   
   o->blur_thresh      = EX_DEFAULT_BLUR_THRESH;
   o->sharpen_thresh   = EX_DEFAULT_SHARPEN_THRESH;
   o->brighten_thresh  = EX_DEFAULT_BRIGHTEN_THRESH;
   o->slide_interval   = EX_DEFAULT_SLIDE_INTERVAL;
   o->comments_visible = EX_DEFAULT_COMMENTS_VISIBLE;
   
   return o;
}

void
_ex_options_free(Exhibit *e)
{
   E_FREE(e->options->app1);
   E_FREE(e->options->app2);
   E_FREE(e->options->app3);
   E_FREE(e->options->app4);
   E_FREE(e->options->fav_path);
   E_FREE(e->options);
}

void
_ex_options_save(Exhibit *e)
{
   
}

void
_ex_options_load(Exhibit *e)
{
   
}

void
_ex_options_window_show(Exhibit *e)
{
   
}

Etk_Bool 
_ex_options_window_delete_cb(void *data)
{
   return TRUE;
}
