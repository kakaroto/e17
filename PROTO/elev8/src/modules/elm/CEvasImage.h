#ifndef C_EVAS_IMAGE_H
#define C_EVAS_IMAGE_H

#include <v8.h>

#include "elev8_elm.h"
#include "CEvasObject.h"

using namespace v8;

class CEvasImage : public CEvasObject {
   FACTORY(CEvasImage)

protected:
   CPropHandler<CEvasImage> prop_handler;

public:
   CEvasImage(CEvasObject *parent, Local<Object> obj);

   virtual void file_set(Handle<Value> val);

   virtual Handle<Value> file_get(void) const;

   virtual void width_set(Handle<Value> val);

   virtual Handle<Value> width_get(void) const;

   virtual void height_set(Handle<Value> val);

   virtual Handle<Value> height_get(void) const;

   virtual void fill_set(Handle<Value> val);

   virtual Handle<Value> fill_get(void) const;

   virtual void border_set(Handle<Value> val);

   virtual Handle<Value> border_get(void) const;

   virtual void border_center_fill_set(Handle<Value> val);

   virtual Handle<Value> border_center_fill_get(void) const;

   virtual void filled_set(Handle<Value> val);

   virtual Handle<Value> filled_get(void) const;

   virtual void border_scale_set(Handle<Value> val);

   virtual Handle<Value> border_scale_get(void) const;

   virtual void fill_spread_set(Handle<Value> val);

   virtual Handle<Value> fill_spread_get(void) const;

   virtual void alpha_set(Handle<Value> val);

   virtual Handle<Value> alpha_get(void) const;

   virtual void smooth_scale_set(Handle<Value> val);

   virtual Handle<Value> smooth_scale_get(void) const;

   virtual void pixels_dirty_set(Handle<Value> val);

   virtual Handle<Value> pixels_dirty_get(void) const;

   virtual void load_dpi_set(Handle<Value> val);

   virtual Handle<Value> load_dpi_get(void) const;

   virtual void load_size_set(Handle<Value> val);

   virtual Handle<Value> load_size_get(void) const;

   virtual void load_scale_down_set(Handle<Value> val);

   virtual Handle<Value> load_scale_down_get(void) const;

   virtual void load_region_set(Handle<Value> val);

   virtual Handle<Value> load_region_get(void) const;

   virtual void load_orientation_set(Handle<Value> val);

   virtual Handle<Value> load_orientation_get(void) const;

   virtual void colorspace_set(Handle<Value> val);

   virtual Handle<Value> colorspace_get(void) const;

   virtual void on_click(void *event_info);

   static void eo_on_click(void *data, Evas *, Evas_Object *, void *event_info);

   virtual void on_clicked_set(Handle<Value> val);

   virtual Handle<Value> on_clicked_get(void) const;
};

#endif
