#ifndef __EDVI_PAGE_H__
#define __EDVI_PAGE_H__


#include <Evas.h>

#include "edvi_forward.h"


/**
 * @file edvi_page.h
 *
 * @defgroup Edvi_Page Edvi Page
 *
 * @brief DVI related functions to manage the pages.
 *
 * Provides a set of functions that manages a DVI page.
 *
 * @ingroup Edvi
 *
 * @{
 */


/**
 * @brief Return a newly created Edvi_Page object
 *
 * @param document The document
 * @return An Edvi_Page
 *
 * Return a newly created Edvi_Page object of the document @p doc,
 *  corresponding to the page @p index. It must be freed with
 * edvi_page_delete().
 */
Edvi_Page *edvi_page_new (Edvi_Document *document);

/**
 * @brief Delete an Edvi_Page object
 *
 * @param page The page to delete
 *
 * Delete the Edvi_Page object @p page, created with
 * edvi_page_new().
 */
void edvi_page_delete (Edvi_Page *page);

/**
 * @brief Render a page on an Evas_Object
 *
 * @param page The page
 * @param device The device
 * @param o The Evas_Object on which the page is rendered.
 *
 * Render page @p page on the Evas_Object @p o. @p hscale and @p vscale
 * are respectively the horizontal and vertical scales, in pixel per
 * points to render the page. @p orientation set the orientation of the
 * page.
 */
void edvi_page_render (const Edvi_Page *page,
                       Edvi_Device     *device,
                       Evas_Object     *o);

/**
 * @brief Set the page number
 *
 * @param page The page
 * @param p The page number
 *
 * Set the page number of the page @p page to @p p
 */
void edvi_page_page_set (Edvi_Page *page, int p);

/**
 * @brief Return the current page number
 *
 * @param page The page
 * @return The current page number
 *
 * Return the current page number of @p page.
 */
int edvi_page_page_get (const Edvi_Page *page);

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
void edvi_page_size_get (const Edvi_Page *page, int *width, int *height);

/**
 * @brief Set the magnification
 *
 * @param page The page
 * @param mag The magnification value
 *
 * Set the magnification value of @p page
 */
void edvi_page_mag_set (Edvi_Page *page,
                        double     mag);

/**
 * @brief Get the magnification
 *
 * @param page The page
 * @return The magnification value
 *
 * Get the magnification value of @p page
 */
double edvi_page_mag_get (const  Edvi_Page *page);

/**
 * @brief Set the orientation of a page
 *
 * @param page The page
 * @param orientation The orientation
 *
 * Set the orientation of @p page
 */
void edvi_page_orientation_set (Edvi_Page            *page,
                                Edvi_Page_Orientation orientation);

/**
 * @brief Return the orientation of a page
 *
 * @param page The page
 * @return The orientation of the page
 *
 * Return the orientation of the page @p page
 */
Edvi_Page_Orientation edvi_page_orientation_get (const  Edvi_Page *page);

/**
 * @}
 */


#endif /* __EDVI_PAGE_H__ */
