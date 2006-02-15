#ifndef __POPPLER_PAGE_H__
#define __POPPLER_PAGE_H__


#include <Evas.h>
#include <Ecore_Data.h>

#include "poppler_forward.h"


typedef struct
{
  double x1;
  double y1;
  double x2;
  double y2;
}Rectangle;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Return a newly created Evas_Poppler_Page object
 *
 * @param doc The document
 * @param index The page number
 * @return An Evas_Poppler_Page
 *
 * Return a newly created Evas_Poppler_Page object of the document @p
 * doc, corresponding to the page @p index. It must be freed with
 * evas_poppler_page_delete
 */
Evas_Poppler_Page *evas_poppler_page_new (const Evas_Poppler_Document *doc,
                                          int                          index);

/**
 * Delete an Evas_Poppler_Page object
 *
 * @param page The page
 *
 * Delete the Evas_Poppler_Page object @p page, created with
 * evas_poppler_page_new
 */
void evas_poppler_page_delete (Evas_Poppler_Page *page);

/**
 * Render a page on an Evas_Object
 *
 * @param page The page
 * @param o The Evas_Object on which the page is rendered
 * @param x The x coordinate
 * @param y The y coordinate
 * @param w The width
 * @param h The height
 * @param xres The x resolution
 * @param yres The y resolution
 *
 * Render a rectangle of the page @p page on the Evas_Object @p o. The
 * upper-left corner of the rectangle has the coordinates (@px, @py),
 * its width is @pw and its height is @p h. @p xres and @p yres are
 * respectively the x and y resolution used to render the rectangle
 */
void evas_poppler_page_render (Evas_Poppler_Page *page,
                               Evas_Object       *o,
                               int                x,
                               int                y,
                               int                w,
                               int                h,
                               double             xres,
                               double             yres);

/**
 * Return the page number
 *
 * @param page The page
 * @return The page number
 *
 * Return page number of the page @p page
 */
int evas_poppler_page_number_get (Evas_Poppler_Page *page);

/**
 * Return the text of a page contained in a rectangle
 *
 * @param page The page
 * @param r The rectangle
 * @return The text contained in @p r
 *
 * Return the text of the page @p contained in the rectangle @p r. The
 * result must be freed
 */
char *evas_poppler_page_text_get (Evas_Poppler_Page *page, Rectangle r);

/**
 * Return a list of rectangle in tha page, all of them containing a given
 * text
 *
 * @param page The page
 * @param text The text
 * @param is_case_sensitive 1 if the text is search with sensitive
 * case, 0 otherwise
 * @return A list of rectanles which all contain the text @p text
 *
 * Return a list of rectangles in the page @p page containing the text
 * @text. If @p is_case_sensitive is equal to 0, the case is not taken
 * into account. The list must be destroyed with a call to
 * ecore_list_destroy
 */
Ecore_List *evas_poppler_page_text_find (Evas_Poppler_Page *page,
                                         const char        *text,
                                         unsigned char      is_case_sensitive);

/**
 * Return the transition of a page
 *
 * @param page The page
 * @return The transition of the page, as an Evas_Poppler_Page_Transition
 *
 * Return the transition of the page @p page
 */
Evas_Poppler_Page_Transition *evas_poppler_page_transition_get (Evas_Poppler_Page *page);

/**
 * Return the width of a page
 *
 * @param page The page
 * @return The width of the page
 *
 * Return the width of the page @p page
 */
int evas_poppler_page_width_get (Evas_Poppler_Page *page);

/**
 * Return the height of a page
 *
 * @param page The page
 * @return The height of the page
 *
 * Return the height of the page @p page
 */
int evas_poppler_page_height_get (Evas_Poppler_Page *page);

/**
 * Return the orientation of a page
 *
 * @param page The page
 * @return The orientation of the page
 *
 * Return the orientation of the page @p page
 */
Evas_Poppler_Page_Orientation evas_poppler_page_orientation_get (Evas_Poppler_Page *page);

/**
 * Return the orientation of a page as a string
 *
 * @param page The page
 * @return The orientation of the page as a string
 *
 * Return the orientation of the page @p page as a string. The
 * returned string must not be freed
 */
const char *evas_poppler_page_orientation_name_get (Evas_Poppler_Page *page);


#ifdef __cplusplus
}
#endif


#endif /* __POPPLER_PAGE_H__ */
