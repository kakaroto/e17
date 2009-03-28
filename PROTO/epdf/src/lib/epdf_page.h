#ifndef __EPDF_PAGE_H__
#define __EPDF_PAGE_H__


#include "epdf_forward.h"


/**
 * @file epdf_page.h
 *
 * @defgroup Epdf_Page Epdf Page
 *
 * @brief PDF related functions to manage the pages.
 *
 * Provides a set of functions that manages a PDF page.
 *
 * @ingroup Epdf
 *
 * @{
 */


typedef struct
{
  double x1;
  double y1;
  double x2;
  double y2;
}Epdf_Rectangle;


/**
 * @brief Return a newly created Epdf_Page object
 *
 * @param doc The document
 * @return An Epdf_Page
 *
 * Return a newly created Epdf_Page object of the document @p doc,
 * corresponding to the page @p index. It must be freed with
 * epdf_page_delete()
 */
Epdf_Page *epdf_page_new (const Epdf_Document *doc);

/**
 * @brief Delete an Epdf_Page object
 *
 * @param page The page to delete
 *
 * Delete the Epdf_Page object @p page, created with
 * epdf_page_new()
 */
void epdf_page_delete (Epdf_Page *page);

/**
 * @brief Render a page on an Evas_Object
 *
 * @param page The page
 * @param o The Evas_Object on which the page is rendered
 *
 * Render page @p page on the Evas_Object @p o with the current
 * properties of @p page.
 */
void epdf_page_render (Epdf_Page   *page,
                       Evas_Object *o);

/**
 * @brief Render a page on an Evas_Object
 *
 * @param page The page
 * @param o The Evas_Object on which the page is rendered
 * @param x The x coordinate of the upper-left corner
 * @param y The y coordinate of the upper-left corner
 * @param w The width of rectangle to render
 * @param h The height of rectangle to render
 *
 * Render a rectangle of the page @p page on the Evas_Object @p o. The
 * upper-left corner of the rectangle has the coordinates (@p x, @ py),
 * its width is @p w and its height is @p h.
 *
 * If @p w is less than 0, the original width is choosen.
 * If @p h is less than 0, the original height is choosen.
 */
void epdf_page_render_slice (Epdf_Page   *page,
                             Evas_Object *o,
                             int          x,
                             int          y,
                             int          w,
                             int          h);

/**
 * @brief Set the page number
 *
 * @param page The page
 * @param p The page number
 *
 * Set the page number of the page @p page to @p p
 */
void epdf_page_page_set (Epdf_Page *page, int p);

/**
 * @brief Return the current page number
 *
 * @param page The page
 * @return The current page number
 *
 * Return the current number of the page @p page
 */
int epdf_page_page_get (const Epdf_Page *page);

/**
 * @brief Return the text of a page contained in a rectangle
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
 * @brief Return a list of rectangle in tha page, all of them containing a given text
 *
 * @param page The page
 * @param text The text
 * @param is_case_sensitive 1 if the text is search with sensitive
 * case, 0 otherwise
 * @return A list of rectanles which all contain the text @p text
 *
 * Return a list of rectangles in the page @p page containing the text
 * @p text. If @p is_case_sensitive is equal to 0, the case is not taken
 * into account. The list must be destroyed with a call to
 * ecore_list_destroy()
 */
Eina_List *epdf_page_text_find (const Epdf_Page *page,
                                const char      *text,
                                unsigned char    is_case_sensitive);

/**
 * Return the transition of a page
 *
 * @param page The page
 * @return The transition of the page, as an Epdf_Page_Transition
 *
 * Return the transition of the page @p page
 */
Epdf_Page_Transition *epdf_page_transition_get (const Epdf_Page *page);

/**
 * @brief Return the size of a page
 *
 * @param page The page
 * @param width The width of the page
 * @param height The height of the page
 *
 * Return the size of the page @p page in @p width and @p height.
 * On failure, @p width and @p height store both 0. @p width or
 * @p height can be @c NULL.
 */
void epdf_page_size_get (const Epdf_Page *page, int *width, int *height);

/**
 * @brief Set the scale of a page
 *
 * @param page The page
 * @param hscale The horizontal scale
 * @param vscale The vertical scale
 *
 * Set the horizontal scale @p hscale and vertical scale @p vscale of
 * the @p page, in pixel per points. A value of 1.0 and 1.0 (resp.)
 * means that the original size of the document is choosen.
 */
void epdf_page_scale_set (Epdf_Page *page,
                          double     hscale,
                          double     vscale);

/**
 * @brief Get the scale of a page
 *
 * @param page The page
 * @param hscale The horizontal scale
 * @param vscale The vertical scale
 *
 * Store the horizontal and vertical scale of the page @p page respectively
 * in the buffers @p hscale and vertical scale @p vscale, in pixel per points.
 */
void epdf_page_scale_get (const Epdf_Page *page,
                          double          *hscale,
                          double          *vscale);

/**
 * @brief Set the orientation of a page
 *
 * @param page The page
 * @param orientation The orientation
 *
 * Set the orientation of @p page to @p orientation.
 */
void epdf_page_orientation_set (Epdf_Page            *page,
                                Epdf_Page_Orientation orientation);

/**
 * @brief Return the orientation of a page
 *
 * @param page The page
 * @return The orientation of the page
 *
 * Return the orientation of the page @p page
 */
Epdf_Page_Orientation epdf_page_orientation_get (const Epdf_Page *page);

/**
 * @}
 */


#endif /* __EPDF_PAGE_H__ */
