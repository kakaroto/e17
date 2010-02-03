#ifndef		_EVAS_BINDINGS_H__
# define	_EVAS_BINDINGS_H__

#include "Elixir.h"

extern elixir_parameter_t       evas_object_parameter;
extern elixir_parameter_t       evas_object_rectangle_parameter;
extern elixir_parameter_t       evas_object_line_parameter;
extern elixir_parameter_t       evas_object_gradient_parameter;
extern elixir_parameter_t       evas_object_polygon_parameter;
extern elixir_parameter_t       evas_object_image_parameter;
extern elixir_parameter_t       evas_object_smart_parameter;
extern elixir_parameter_t       evas_object_text_parameter;
extern elixir_parameter_t       evas_object_textblock_parameter;

Eina_Bool load_evas_params(JSContext *cx, JSObject *parent);
Eina_Bool load_void_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_void_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_int_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_int_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_const_binding(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_event_binding(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_string_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_const_binding(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_event_binding(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_string_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_params(JSContext *cx, JSObject *parent);
Eina_Bool load_2_evas_object_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_bindings(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_bindings(JSContext *cx, JSObject *parent);
Eina_Bool unload_2_evas_object_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_int_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_line_binding(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_text_binding(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_int_2_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_int_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_color_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_string_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_image_binding(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_int_2_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_line_binding(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_boolean_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_color_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_string_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_text_bindings(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_image_bindings(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_boolean_params(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_object_textblock_binding(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_object_textblock_binding(JSContext *cx, JSObject *parent);
Eina_Bool load_evas_smart_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_evas_smart_params(JSContext *cx, JSObject *parent);
Eina_Bool load_4_int_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_4_int_params(JSContext *cx, JSObject *parent);
Eina_Bool load_3_int_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_3_int_params(JSContext *cx, JSObject *parent);
Eina_Bool load_2_double_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_2_double_params(JSContext *cx, JSObject *parent);
Eina_Bool load_1_double_params(JSContext *cx, JSObject *parent);
Eina_Bool unload_1_double_params(JSContext *cx, JSObject *parent);

Eina_Bool elixir_evas_file_extract(JSContext *cx, JSObject *obj,
				   const char** file, const char **key);
Eina_Bool elixir_evas_border_extract(JSContext *cx, JSObject *obj,
				     int *l, int *r, int *t, int *b);
Eina_Bool elixir_evas_font_extract(JSContext *cx, JSObject *obj,
				   const char** font, int *size);
Eina_Bool elixir_evas_line_extract(JSContext* cx, JSObject* obj,
				   int* x1, int* y1, int* x2, int* y2);
Eina_Bool elixir_evas_position_extract(JSContext *cx, JSObject *obj,  double *x, double *y);
Eina_Bool elixir_evas_aspect_extract(JSContext *cx, JSObject *obj, int *aspect, int *w, int *h);

Eina_Bool elixir_new_evas_file(JSContext *cx,
			       const char* file, const char* key,
			       jsval *rval);
Eina_Bool elixir_new_evas_pos(JSContext *cx,
			      int icx, int icy, int icw, int ich,
			      jsval *rval);
Eina_Bool elixir_new_evas_position(JSContext *cx,
				   double x, double y,
				   jsval *rval);
Eina_Bool elixir_new_evas_aspect(JSContext *cx,
				 int aspect, int w, int h,
				 jsval *rval);
Eina_Bool elixir_new_evas_border(JSContext *cx,
				 int l, int r, int t, int b,
				 jsval *rval);
Eina_Bool elixir_new_evas_font(JSContext *cx,
			       const char* font, int size,
			       jsval *rval);

const char *evas_object_to_elixir_object(Evas_Object *obj);
Eina_Bool evas_object_to_jsval(JSContext *cx, Evas_Object *obj, jsval *rval);
Eina_Bool evas_to_jsval(JSContext *cx, Evas *e, jsval *rval);

#endif		/* _EVAS_BINDINGS_H__ */
