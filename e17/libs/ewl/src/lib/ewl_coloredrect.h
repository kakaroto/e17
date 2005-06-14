#ifndef _EWL_COLOREDRECT_H
#define _EWL_COLOREDRECT_H

/**
 * @file ewl_coloredrect.h
 * @defgroup Ewl_ColoredRect ColoredRect The coloredrect widget
 *
 * @{
 */

/**
 * @themekey /coloredrect/file
 * @themekey /coloredrect/group
 */

typedef struct _ewl_coloredrect Ewl_ColoredRect;

#define EWL_COLOREDRECT(cr) ((Ewl_ColoredRect *)cr)

struct _ewl_coloredrect {
  Ewl_Image widget;
  int r, g, b, a; 
  int redraw;
};

Ewl_Widget     *ewl_coloredrect_new(int r, int g, int b, int a);
void            ewl_coloredrect_init(Ewl_ColoredRect * cp, int r, int g, int b, int a);

void            ewl_coloredrect_color_set(Ewl_ColoredRect *cr, int r, int g, int b, int a);

/*
 * Internally used callbacks, override at your own risk.
 */
void         ewl_coloredrect_configure_cb(Ewl_Widget * w, void *ev_data,
				       void *user_data);

/**
 * @}
 */
#endif

