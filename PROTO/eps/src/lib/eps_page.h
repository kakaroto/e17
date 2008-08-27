#ifndef __EPS_PAGE_H__
#define __EPS_PAGE_H__


#include <Evas.h>

#include "eps_forward.h"


/**
 * @file eps_page.h
 *
 * @defgroup Eps_Page Eps Page
 *
 * @brief Postscript related functions to manage the pages.
 *
 * Provides a set of functions that manages a Postscript page.
 *
 * @ingroup Eps
 *
 * @{
 */

/**
 * @brief Return a newly created Eps_Page object.
 *
 * @param document The document.
 * @return A pointer to a newly created Eps_Page.
 *
 * Return a newly created Eps_Page object of the document
 * @p document, corresponding to the page @p index. It must be freed with
 * eps_page_delete().
 */
Eps_Page *eps_page_new (const Eps_Document *document);

/**
 * @brief Delete an Eps_Page object.
 *
 * @param page The page to delete.
 *
 * Delete the Eps_Page object @p page, created with
 * eps_page_new().
 */
void eps_page_delete (Eps_Page *page);

/**
 * @brief Render a page on an Evas_Object.
 *
 * @param page The page.
 * @param o The Evas_Object on which the page is rendered.
 *
 * Render page @p page on the Evas_Object @p o. @p hscale and @p vscale
 * are respectively the horizontal and vertical scales, in pixel per
 * points to render the page. @p orientation set the orientation of the
 * page.
 */
void eps_page_render (const Eps_Page *page,
                      Evas_Object    *o);

/**
 * @brief Render a part of page on an Evas_Object.
 *
 * @param page The page.
 * @param o The Evas_Object on which the page is rendered.
 * @param x The x coordinate of the upper-left corner of rectangle to render.
 * @param y The y coordinate of the upper-left corner of rectangle to render.
 * @param width The width of rectangle to render.
 * @param height The height of rectangle to render.
 *
 * Render a rectangle of the page @p page on the Evas_Object @p o. The
 * upper-left corner of the rectangle has the coordinates (@p x, @p y),
 * its width is @p width and its height is @p height. @p hscale and @p vscale are
 * respectively the horizontal and vertical scales, in pixel per
 * points to render the rectangle. If the width or the height are less
 * or equal than 0, nothing is drawn. @p orientation set the orientation
 * of the rendered page.
 */
void eps_page_render_slice (const Eps_Page *page,
                            Evas_Object    *o,
                            int             x,
                            int             y,
                            int             width,
                            int             height);

/**
 * Set the page number
 *
 * @param page The page
 * @param p The page number
 *
 * Set the page number of the page @p page to @p p
 */
void eps_page_page_set (Eps_Page *page, int p);

/**
 * @brief Return the page number.
 *
 * @param page The page.
 * @return The page number.
 *
 * Return the page number of the page @p page.
 */
int eps_page_page_get (const Eps_Page *page);

/**
 * @brief Return the label.
 *
 * @param page The page.
 * @return The label number.
 *
 * Return the label of the page @p page.
 */
const char *eps_page_label_get (const Eps_Page *page);

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
void eps_page_size_get (const Eps_Page *page, int *width, int *height);

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
void eps_page_scale_set (const Eps_Page *page,
                         double          hscale,
                         double          vscale);

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
void eps_page_scale_get (const Eps_Page *page,
                         double         *hscale,
                         double         *vscale);

/**
 * @brief Set the orientation of a page
 *
 * @param page The page
 * @param orientation The orientation
 *
 * Set the orientation of @p page to @p orientation.
 */
void eps_page_orientation_set (const Eps_Page      *page,
                               Eps_Page_Orientation orientation);

/**
 * @brief Return the orientation of a page.
 *
 * @param page The page.
 * @return The orientation of the page.
 *
 * Return the orientation of the page @p page.
 */
Eps_Page_Orientation eps_page_orientation_get (const Eps_Page *page);

/**
 * @}
 */


#endif /* __EPS_PAGE_H__ */
