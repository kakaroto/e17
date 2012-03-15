#include <Eina.h>
#include <Evas.h>

#include "elev8_elm.h"
#include "CEvasImage.h"

using namespace v8;

CEvasImage::CEvasImage(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   Evas *evas = evas_object_evas_get(parent->get());
   eo = evas_object_image_filled_add(evas);
   construct(eo, obj);
}

void CEvasImage::file_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        if (0 > access(*str, R_OK))
          ELM_WRN( "warning: can't read image file %s", *str);
        evas_object_image_file_set(eo, *str, NULL);
     }
   evas_object_raise(eo);
}

Handle<Value> CEvasImage::file_get(void) const
{
   const char *f = NULL, *key = NULL;
   evas_object_image_file_get(eo, &f, &key);
   if (f)
     return String::New(f);
   else
     return Null();
}

void CEvasImage::width_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        Evas_Coord x, y, w, h;
        evas_object_geometry_get(eo, &x, &y, &w, &h);
        w = val->ToInt32()->Value();
        evas_object_resize(eo, w, h);
     }
}

Handle<Value> CEvasImage::width_get(void) const
{
   Evas_Coord x, y, w, h;
   evas_object_geometry_get(eo, &x, &y, &w, &h);
   return Number::New(w);
}

void CEvasImage::height_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        Evas_Coord x, y, w, h;
        evas_object_geometry_get(eo, &x, &y, &w, &h);
        h = val->ToInt32()->Value();
        evas_object_resize(eo, w, h);
     }
}

Handle<Value> CEvasImage::height_get(void) const
{
   Evas_Coord x, y, w, h;
   evas_object_geometry_get(eo, &x, &y, &w, &h);
   return Number::New(h);
}

void CEvasImage::fill_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Coord xx, yy, ww, hh;
   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::New("x"));
   Local<Value> y = obj->Get(String::New("y"));
   Local<Value> w = obj->Get(String::New("w"));
   Local<Value> h = obj->Get(String::New("h"));
   ww = w->Int32Value();
   hh = h->Int32Value();
   xx = x->Int32Value();
   yy = y->Int32Value();
   evas_object_image_fill_set (eo, xx, yy, ww, hh);
}

Handle<Value> CEvasImage::fill_get(void) const
{
   Evas_Coord x, y, w, h;
   evas_object_image_fill_get (eo,  &x, &y, &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));
   obj->Set(String::New("x"), Number::New(w));
   obj->Set(String::New("y"), Number::New(h));
   return obj;
}

void CEvasImage::border_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Coord ll, rr, tt, bb;
   Local<Object> obj = val->ToObject();
   Local<Value> l = obj->Get(String::New("l"));
   Local<Value> r = obj->Get(String::New("r"));
   Local<Value> t = obj->Get(String::New("t"));
   Local<Value> b = obj->Get(String::New("b"));
   ll = l->Int32Value();
   rr = r->Int32Value();
   tt = t->Int32Value();
   bb = b->Int32Value();
   evas_object_image_fill_set(eo, ll, rr, tt, bb);
}

Handle<Value> CEvasImage::border_get(void) const
{
   Evas_Coord l, r, t, b;
   evas_object_image_fill_get (eo,  &l, &r, &t, &b);
   Local<Object> obj = Object::New();
   obj->Set(String::New("l"), Number::New(l));
   obj->Set(String::New("r"), Number::New(r));
   obj->Set(String::New("t"), Number::New(t));
   obj->Set(String::New("b"), Number::New(b));
   return obj;
}

void CEvasImage::border_center_fill_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        int bcf = val->ToInt32()->Value();
        evas_object_image_border_center_fill_set(eo, (Evas_Border_Fill_Mode)bcf);
     }
}

Handle<Value> CEvasImage::border_center_fill_get(void) const
{
   Evas_Border_Fill_Mode bcf = evas_object_image_border_center_fill_get(eo);
   return Number::New(bcf);
}

void CEvasImage::filled_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        evas_object_image_filled_set(eo, val->BooleanValue());
     }
}

Handle<Value> CEvasImage::filled_get(void) const
{
   Eina_Bool fill = evas_object_image_filled_get(eo);
   return Boolean::New(fill);
}

void CEvasImage::border_scale_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        evas_object_image_border_scale_set(eo, val->ToInt32()->Value());
     }
}

Handle<Value> CEvasImage::border_scale_get(void) const
{
   Eina_Bool fill = evas_object_image_border_scale_get(eo);
   return Number::New(fill);
}

void CEvasImage::fill_spread_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        int spread = val->ToInt32()->Value();
        evas_object_image_fill_spread_set(eo, (Evas_Fill_Spread)spread);
     }
}

Handle<Value> CEvasImage::fill_spread_get(void) const
{
   Evas_Fill_Spread fs = evas_object_image_fill_spread_get(eo);
   return Number::New(fs);
}

void CEvasImage::alpha_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        evas_object_image_alpha_set(eo, val->BooleanValue());
     }
}

Handle<Value> CEvasImage::alpha_get(void) const
{
   Eina_Bool alpha = evas_object_image_alpha_get(eo);
   return Boolean::New(alpha);
}

void CEvasImage::smooth_scale_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        evas_object_image_smooth_scale_set(eo, val->BooleanValue());
     }
}

Handle<Value> CEvasImage::smooth_scale_get(void) const
{
   Eina_Bool ss = evas_object_image_smooth_scale_get(eo);
   return Boolean::New(ss);
}

void CEvasImage::pixels_dirty_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        evas_object_image_pixels_dirty_set(eo, val->BooleanValue());
     }
}

Handle<Value> CEvasImage::pixels_dirty_get(void) const
{
   Eina_Bool pd = evas_object_image_pixels_dirty_get(eo);
   return Boolean::New(pd);
}

void CEvasImage::load_dpi_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        evas_object_image_load_dpi_set(eo, val->ToInt32()->Value());
     }
}

Handle<Value> CEvasImage::load_dpi_get(void) const
{
   double dpi = evas_object_image_load_dpi_get(eo);
   return Number::New(dpi);
}

void CEvasImage::load_size_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Coord ww, hh;
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("w"));
   Local<Value> h = obj->Get(String::New("h"));
   ww = w->Int32Value();
   hh = h->Int32Value();
   evas_object_image_load_size_set(eo, ww, hh);
}

Handle<Value> CEvasImage::load_size_get(void) const
{
   Evas_Coord w, h;
   evas_object_image_load_size_get(eo, &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));
   return obj;
}

void CEvasImage::load_scale_down_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        evas_object_image_load_scale_down_set(eo, val->ToInt32()->Value());
     }
}

Handle<Value> CEvasImage::load_scale_down_get(void) const
{
   double dpi = evas_object_image_load_scale_down_get(eo);
   return Number::New(dpi);
}

void CEvasImage::load_region_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Coord xx, yy, ww, hh;
   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::New("x"));
   Local<Value> y = obj->Get(String::New("y"));
   Local<Value> w = obj->Get(String::New("w"));
   Local<Value> h = obj->Get(String::New("h"));
   ww = w->Int32Value();
   hh = h->Int32Value();
   xx = x->Int32Value();
   yy = y->Int32Value();
   evas_object_image_load_region_set (eo, xx, yy, ww, hh);
}

Handle<Value> CEvasImage::CEvasImage::load_region_get(void) const
{
   Evas_Coord x, y, w, h;
   evas_object_image_load_region_get (eo,  &x, &y, &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));
   obj->Set(String::New("x"), Number::New(w));
   obj->Set(String::New("y"), Number::New(h));
   return obj;
}

void CEvasImage::load_orientation_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        evas_object_image_load_orientation_set(eo, val->BooleanValue());
     }
}

Handle<Value> CEvasImage::load_orientation_get(void) const
{
   Eina_Bool pd = evas_object_image_load_orientation_get(eo);
   return Boolean::New(pd);
}

void CEvasImage::colorspace_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        evas_object_image_colorspace_set(eo, (Evas_Colorspace)val->Int32Value());
     }
}

Handle<Value> CEvasImage::colorspace_get(void) const
{
   Evas_Colorspace pd = evas_object_image_colorspace_get(eo);
   return Number::New(pd);
}

void CEvasImage::on_click(void *event_info)
{
   Handle<Object> obj = get_object();
   HandleScope handle_scope;
   Handle<Value> val = on_clicked_val;

   // also provide x and y positions where it was clicked
   //
   if (event_info!=NULL)
     {
        Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;
        Handle<Number> ox = Number::New(ev->canvas.x);
        Handle<Number> oy = Number::New(ev->canvas.y);
        // FIXME: pass event_info to the callback
        // FIXME: turn the pieces below into a do_callback method
        Handle<Value> args[3] = { obj, ox, oy };
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        printf("Hello2\n");
        fn->Call(obj, 3, args);
     }
   else
     {
        // FIXME: pass event_info to the callback
        // FIXME: turn the pieces below into a do_callback method
        Handle<Value> args[1] = { obj };
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        printf("Hello1\n");
        fn->Call(obj, 1, args);
     }

}

void CEvasImage::eo_on_click(void *data, Evas *, Evas_Object *, void *event_info)
{
   CEvasObject *clicked = static_cast<CEvasObject*>(data);

   clicked->on_click(event_info);
}

void CEvasImage::on_clicked_set(Handle<Value> val)
{
   on_clicked_val.Dispose();
   on_clicked_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_event_callback_add(eo, EVAS_CALLBACK_MOUSE_UP, &eo_on_click, this);
   else
     evas_object_event_callback_del(eo, EVAS_CALLBACK_MOUSE_UP, &eo_on_click);
}

Handle<Value> CEvasImage::on_clicked_get(void) const
{
   return on_clicked_val;
}

PROPERTIES_OF(CEvasImage) = {
     PROP_HANDLER(CEvasImage, file),
     PROP_HANDLER(CEvasImage, width),
     PROP_HANDLER(CEvasImage, height),
     PROP_HANDLER(CEvasImage, border),
     PROP_HANDLER(CEvasImage, border_center_fill),
     PROP_HANDLER(CEvasImage, filled),
     PROP_HANDLER(CEvasImage, border_scale),
     PROP_HANDLER(CEvasImage, fill),
     PROP_HANDLER(CEvasImage, fill_spread),
     PROP_HANDLER(CEvasImage, alpha),
     PROP_HANDLER(CEvasImage, smooth_scale),
     PROP_HANDLER(CEvasImage, pixels_dirty),
     PROP_HANDLER(CEvasImage, load_dpi),
     PROP_HANDLER(CEvasImage, load_size),
     PROP_HANDLER(CEvasImage, load_scale_down),
     PROP_HANDLER(CEvasImage, load_region),
     PROP_HANDLER(CEvasImage, load_orientation),
     PROP_HANDLER(CEvasImage, colorspace),
     { NULL }
};
