#include "../include/ewlxx/EvasEwl.h"
#include "../include/ewlxx/EwlEmbed.h"

namespace efl {

EvasEwl::EvasEwl( EvasCanvas &canvas, EwlEmbed* ewlobj, const char* name )
{
#warning not sure whether this belongs here
#if 0 // [audifahrer]
  ewl_object_fill_policy_set( EWL_OBJECT(ewlobj->obj() ), EWL_FLAG_FILL_ALL );
  o = ewl_embed_evas_set( EWL_EMBED( ewlobj->obj() ), canvas->obj(), EWL_EMBED_EVAS_WINDOW(ecore_evas_software_x11_window_get(canvas->obj())));
#endif
}

EvasEwl::~EvasEwl()
{
}

} // end namespace efl
