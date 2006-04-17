#ifndef __EPDF_PAGE_H__
#define __EPDF_PAGE_H__


#include <Evas.h>
#include <Ecore_Data.h>

#include "poppler_forward.h"


typedef struct
{
  double x1;
  double y1;
  double x2;
  double y2;
}Epdf_Rectangle;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Return a newly created Epdf_Page object
 *
 * @param doc The document
 * @param index The page number
 * @return An Epdf_Page
 *
 * Return a newly created Epdf_Page object of the document @p
 * doc, corresponding to the page @p index. It must be freed with
 * epdf_page_delete
 */
Epdf_Page *epdf_page_new (const Epdf_Document *doc,
                          int                  index);

/**
 * Delete an Epdf_Page object
 *
 * @param page The page
 *
 * Delete the Epdf_Page object @p page, created with
 * epdf_page_new
 */
void epdf_page_delete (Epdf_Page *page);

/**
 * Render a page on an Evas_Object
 *
 * @param page The page
 * @param o The Evas_Object on which the page is rendered
 * @param x The x coordinate of the upper-left corner
 * @param y The y coordinate of the upper-left corner
 * @param w The width of rectangle to render
 * @param h The height of rectangle to render
 * @param hscale The horizontal scale specified as pixels per point
 * @param vscale The vertical scale specified as pixels per point
 *
 * Render a rectangle of the page @p page on the Evas_Object @p o. The
 * upper-left corner of the rectangle has the coordinates (@px, @py),
 * its width is @pw and its height is @p h. @p hscale and @p vscale are
 * respectively the horizontal and vertical scales, in pixel per
 * points to render the rectangle. A value of 1.0 and 1.0 (resp.)
 * means that the original size of the document is choosen.
 */
void epdf_page_render (Epdf_Page            *page,
                       Evas_Object          *o,
                       Epdf_Page_Orientation orientation,
                       int                   x,
                       int                   y,
                       int                   w,
                       int                   h,
                       double                hscale,
                       double                vscale);

/**
 * Return the page number
 *
 * @param page The page
 * @return The page number
 *
 * Return page number of the page @p page
 */
int epdf_page_number_get (Epdf_Page *page);

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
char *epdf_page_text_get (Epdf_Page     *page,
                          Epdf_Rectangle r);

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
Ecore_List *epdf_page_text_find (Epdf_Page    *page,
                                 const char   *text,
                                 unsigned char is_case_sensitive);

/**
 * Return the transition of a page
 *
 * @param page The page
 * @return The transition of the page, as an Epdf_Page_Transition
 *
 * Return the transition of the page @p page
 */
Epdf_Page_Transition *epdf_page_transition_get (Epdf_Page *page);

/**
 * Return the width of a page
 *
 * @param page The page
 * @return The width of the page
 *
 * Return the width of the page @p page
 */
int epdf_page_width_get (Epdf_Page *page);

/**
 * Return the height of a page
 *
 * @param page The page
 * @return The height of the page
 *
 * Return the height of the page @p page
 */
int epdf_page_height_get (Epdf_Page *page);

/**
 * Return the orientation of a page
 *
 * @param page The page
 * @return The orientation of the page
 *
 * Return the orientation of the page @p page
 */
Epdf_Page_Orientation epdf_page_orientation_get (Epdf_Page *page);

/**
 * Return the orientation of a page as a string
 *
 * @param page The page
 * @return The orientation of the page as a string
 *
 * Return the orientation of the page @p page as a string. The
 * returned string must not be freed
 */
const char *epdf_page_orientation_name_get (Epdf_Page *page);


#ifdef __cplusplus
}
#endif


#endif /* __EPDF_PAGE_H__ */
