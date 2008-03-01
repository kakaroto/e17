#ifndef __EDVI_PAGE_H__
#define __EDVI_PAGE_H__


#include <Evas.h>

#include "edvi_forward.h"


/**
 * @file edvi_page.h
 * @defgroup Edvi_Page Edvi Page
 * @brief Functions that manage DVI pages
 * @ingroup Edvi
 *
 * Functions that manage DVI pages
 */


/**
 * Return a newly created Edvi_Page object
 *
 * @param document The document
 * @param index The page number
 * @return An Edvi_Page
 *
 * Return a newly created Edvi_Page object of the document @p
 * doc, corresponding to the page @p index. It must be freed with
 * edvi_page_delete
 *
 * @ingroup Edvi_Page
 */
Edvi_Page *edvi_page_new (Edvi_Document *document,
                          int            index);

/**
 * Delete an Edvi_Page object
 *
 * @param page The page to delete
 *
 * Delete the Edvi_Page object @p page, created with
 * edvi_page_new
 *
 * @ingroup Edvi_Page
 */
void edvi_page_delete (Edvi_Page *page);

/**
 * Render a page on an Evas_Object
 *
 * @param page The page
 * @param device The device
 * @param o The Evas_Object on which the page is rendered.
 *
 * Render page @p page on the Evas_Object @p o. @p hscale and @p vscale
 * are respectively the horizontal and vertical scales, in pixel per
 * points to render the page. @p orientation set the orientation of the
 * page.
 *
 * @ingroup Edvi_Page
 */
void edvi_page_render (Edvi_Page   *page,
                       Edvi_Device *device,
                       Evas_Object *o);

/**
 * @brief Get the current page number
 *
 * @param page The page
 * @return The current page number
 *
 * Get the current page number of @p page.
 *
 * @ingroup Edvi_Page
 */
int edvi_page_number_get (Edvi_Page *page);

/**
 * @brief Set the magnification
 *
 * @param page The page
 * @param mag The magnification value
 *
 * Set the magnification value of @p page
 *
 * @ingroup Edvi_Page
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
 *
 * @ingroup Edvi_Page
 */
double edvi_page_mag_get (Edvi_Page *page);

/**
 * @brief Set the page size
 *
 * @param page The page
 * @param size The page size
 *
 * Set the page size of @p page
 *
 * @ingroup Edvi_Page
 */
void edvi_page_size_set (Edvi_Page     *page,
                         Edvi_Page_Size size);

/**
 * @brief Get the page size
 *
 * @param page The page
 * @return The page size
 *
 * Get the page size of @p page
 *
 * @ingroup Edvi_Page
 */
Edvi_Page_Size edvi_page_size_get (Edvi_Page *page);

/**
 * @brief Get the width size
 *
 * @param page The page
 * @return The width
 *
 * Get the width of @p page
 *
 * @ingroup Edvi_Page
 */
int edvi_page_width_get (Edvi_Page *page);

/**
 * @brief Get the height size
 *
 * @param page The page
 * @return The height
 *
 * Get the height of @p page
 *
 * @ingroup Edvi_Page
 */
int edvi_page_height_get (Edvi_Page *page);

/**
 * @brief Set the orientation of a page
 *
 * @param page The page
 * @param orientation The orientation
 *
 * Set the orientation of @p page
 *
 * @ingroup Edvi_Page
 */
void edvi_page_orientation_set (Edvi_Page            *page,
                                Edvi_Page_Orientation orientation);

/**
 * Return the orientation of a page
 *
 * @param page The page
 * @return The orientation of the page
 *
 * Return the orientation of the page @p page
 *
 * @ingroup Edvi_Page
 */
Edvi_Page_Orientation edvi_page_orientation_get (Edvi_Page *page);


#endif /* __EDVI_PAGE_H__ */
