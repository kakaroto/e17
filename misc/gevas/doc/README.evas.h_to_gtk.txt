evas_gradient_add_color

This is a list of what is done, and what is not done.


*DONE* void evas_set_color(Evas e, Evas_Object o, int r, int g, int b, int a);
*DONE* void evas_set_angle(Evas e, Evas_Object o, double angle);
*DONE* void evas_set_zoom_scale(Evas e, Evas_Object o, int scale);
*DONE* void evas_set_layer(Evas e, Evas_Object o, int l);
*DONE* void evas_set_layer_store(Evas e, int l, int store);
*DONE* void evas_raise(Evas e, Evas_Object o);
*DONE* void evas_lower(Evas e, Evas_Object o);
*DONE* void evas_stack_above(Evas e, Evas_Object o, Evas_Object above);
*DONE* void evas_stack_below(Evas e, Evas_Object o, Evas_Object below);
*DONE* void evas_move(Evas e, Evas_Object o, double x, double y);
*DONE* void evas_resize(Evas e, Evas_Object o, double w, double h);
*DONE* void evas_get_geometry(Evas e, Evas_Object o, double *x, double *y, double *w, double *h);
*DONE* void evas_del_object(Evas e, Evas_Object o);
*DONE* Evas_Object evas_add_image_from_file(Evas e, char *file);
*DONE* void evas_set_image_file(Evas e, Evas_Object o, char *file);
*DONE* void evas_set_image_fill(Evas e, Evas_Object o, double x, double y, double w, double h);
*DONE* void evas_set_image_border(Evas e, Evas_Object o, int l, int r, int t, int b);
*DONE* void evas_show(Evas e, Evas_Object o);
*DONE* void evas_hide(Evas e, Evas_Object o);
*DONE* void evas_get_color(Evas e, Evas_Object o, int *r, int *g, int *b, int *a);
*DONE* void evas_object_set_name(Evas e, Evas_Object o, char *name);   
*DONE* char *evas_object_get_name(Evas e, Evas_Object o);
*DONE* void evas_get_image_size(Evas e, Evas_Object o, int *w, int *h);
*DONE* void evas_get_image_border(Evas e, Evas_Object o, int *l, int *r, int *t, int *b);

///////////////////////////////////////////////////////
// Untested
/////////
*DONE* Evas_Object evas_get_object_under_mouse(Evas e);
*DONE* Evas_Object evas_object_in_rect(Evas e, double x, double y, double w, double h);
*DONE* Evas_Object evas_object_at_position(Evas e, double x, double y);
*DONE* Evas_Object evas_object_get_named(Evas e, char *name);

/* text */
*DONE* char  *evas_get_text_string(Evas e, Evas_Object o);
*DONE* char  *evas_get_text_font(Evas e, Evas_Object o);
*DONE* int    evas_get_text_size(Evas e, Evas_Object o);
*DONE* int    evas_text_at_position(Evas e, Evas_Object o, double x, double y, int *char_x, int *char_y, int *char_w, int *char_h);
*DONE* void   evas_text_at(Evas e, Evas_Object o, int index, int *char_x, int *char_y, int *char_w, int *char_h);
*DONE* void   evas_text_get_ascent_descent(Evas e, Evas_Object o, double *ascent, double *descent);
*DONE* void   evas_text_get_max_ascent_descent(Evas e, Evas_Object o, double *ascent, double *descent);
*DONE* void   evas_text_get_advance(Evas e, Evas_Object o, double *h_advance, double *v_advance);
*DONE* double evas_text_get_inset(Evas e, Evas_Object o);
*DONE* Evas_Object evas_add_text(Evas e, char *font, int size, char *text);
*DONE* void evas_set_text(Evas e, Evas_Object o, char *text);
*DONE* void evas_set_font(Evas e, Evas_Object o, char *font, int size);

/* gradient */
*DONE* Evas_Object evas_add_gradient_box(Evas e);
*DONE* Evas_Gradient evas_gradient_new(void);
*DONE* void evas_set_gradient(Evas e, Evas_Object o, Evas_Gradient grad);
*DONE* void evas_gradient_free(Evas_Gradient grad);
*DONE* void evas_gradient_add_color(Evas_Gradient grad, int r, int g, int b, int a, int dist);


// I don't plan to use these right away, so I have left them out, feel free to add them //
*LEFT OUT* void evas_set_image_data(Evas e, Evas_Object o, void *data, Evas_Image_Format format, int w, int h);
*LEFT OUT* void evas_set_pass_events(Evas e, Evas_Object o, int pass_events);
*LEFT OUT* Evas_Object evas_add_image_from_data(Evas e, void *data, Evas_Image_Format format, int w, int h);
*LEFT OUT* Evas_Object evas_add_rectangle(Evas e);
*LEFT OUT* Evas_Object evas_add_line(Evas e);
*LEFT OUT* void evas_set_line_xy(Evas e, Evas_Object o, double x1, double y1, double x2, double y2);
*LEFT OUT* Evas_Object evas_add_poly(Evas e);
*LEFT OUT* void evas_add_point(Evas e, Evas_Object o, double x, double y);
	 


///////////////////////////////////////////////////////
// These are all not done.
///////////////


////////////////// These need to go into gevas ////////////////////

/* object query */
Evas_List evas_objects_in_rect(Evas e, double x, double y, double w, double h);
Evas_List evas_objects_at_position(Evas e, double x, double y);
Evas_List evas_get_points(Evas e, Evas_Object o);

