#ifndef __EPS_H__
#define __EPS_H__


/**
 * @mainpage Eps library
 *
 * @section intro_sec Introduction
 *
 * Eps is a library that provides a C interface to the Poppler library
 * to render PS documents in different ways, using an Evas frontend.
 * Straight Evas can be used, but several widgets implementations can
 * integrate a PS document in an Edje user interface using an Evas
 * smart object, or Ewl or Etk user interface using respective widgets.
 *
 * @section install_sec Installation
 *
 * The Eps library requires Evas, Ecore and Poppler to be installed.
 *
 * If the Ewl or Etk toolkits are available, then respective widgets
 * will be built. Also, an Ewl test will be installed.
 *
 * If Epsilon is available, then an Epsilon plugin to create PS
 * thumbnails will be installed.
 *
 * @subsection api_sec API Documentation
 *
 * For the Eps library, the complete api can be read in the
 * section @ref Eps.
 *
 * For the Esmart object library, the complete api can be read in the
 * section @ref Esmart_Ps.
 *
 * For the Ewl widget, the complete api can be read in the
 * section @ref Ewl_Ps.
 *
 * For the Etk widget, the complete api can be read in the
 * section @ref Etk_Ps.
 */


/**
 * @file Eps.h
 * @defgroup Eps  Eps
 * @brief A Pdf library that renders PDF documents
 *
 * The Eps library provides a set of functions to manage PDF documents.
 * It wraps the functions of the Poppler library in a C API and uses
 * Evas as front end.
 *
 * The simplest way to use Eps is to load a document using
 * eps_document_new(), to create a page using eps_page_new() and to
 * render the whole page in an Evas object using eps_page_render(). If
 * you want to render only a part of the page, use eps_page_render_slice().
 * Here is an example:
 *
 * @code
 * Eps_Document *document;
 * Eps_Page     *page;
 * char         *filename;
 *
 * document = eps_document_new (filename);
 * if (!document) {
 *   // manage error here
 * }
 *
 * page = eps_page_new (document, page_number);
 * if (!page) {
 *   // manage error here
 * }
 *
 * o = evas_object_image_add (evas);
 * evas_object_move (o, 0, 0);
 * eps_page_render (page, device, o);
 * evas_object_show (o);
 *
 * evas_object_del (o);
 * eps_page_delete (page);
 * eps_document_delete (document);
 * @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif


#include "eps_enum.h"
#include "eps_main.h"
#include "eps_document.h"
#include "eps_page.h"


#ifdef __cplusplus
}
#endif


#endif /* __EPS_H__ */
