#ifndef _EDJE_EDITOR_SPECTRA_WIDGET_H_
#define _EDJE_EDITOR_SPECTRA_WIDGET_H_

/* Spectra Widget */
Etk_Widget* spectra_widget_new             (Evas *evas);
void        spectra_widget_name_set        (Etk_Widget *spectra, const char *name);
const char* spectra_widget_name_get        (Etk_Widget *spectra);
void        spectra_widget_color_add       (Etk_Widget* spectra, int r, int g, int b, int a, int d);
void        spectra_widget_color_clear_all (Etk_Widget* spectra);
int         spectra_widget_colors_num_get  (Etk_Widget* spectra);
void        spectra_widget_color_get       (Etk_Widget *spectra, int color_num, int *r, int *g, int *b, int *a, int *d);


#endif
