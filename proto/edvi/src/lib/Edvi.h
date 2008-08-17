#ifndef __EDVI_H__
#define __EDVI_H__


/**
 * @mainpage Edvi library
 *
 * @section intro_sec Introduction
 *
 * Edvi is a library that provides a C interface to the dvilib library
 * to render DVI documents in different ways, using an Evas frontend.
 * Straight Evas can be used, but several widgets implementations can
 * integrate a DVI document in an Edje user interface using an Evas
 * smart object, or Ewl or Etk user interface using respective widgets.
 *
 * @section install_sec Installation
 *
 * The Edvi library requires Evas, Ecore and DVIlib to be installed.
 *
 * If the Ewl or Etk toolkits are available, then respective widgets
 * will be built. Also, an Ewl test will be installed.
 *
 * If Epsilon is available, then an Epsilon plugin to create DVI
 * thumbnails will be installed.
 *
 * @subsection api_sec API Documentation
 *
 * For the Edvi library, the complete api can be read in the
 * section @ref Edvi.
 *
 * For the Esmart object library, the complete api can be read in the
 * section @ref Esmart_Dvi.
 *
 * For the Ewl widget, the complete api can be read in the
 * section @ref Ewl_Dvi.
 *
 * For the Etk widget, the complete api can be read in the
 * section @ref Etk_Dvi.
 */


/**
 * @file Edvi.h
 * @defgroup Edvi  Edvi
 * @brief A Dvi library that renders DVI documents
 *
 * The Edvi library provides a set of functions to manage DVI documents.
 * It wraps the functions of the DVIlib library in a C API and uses
 * Evas as front end.
 *
 * The simplest way to use Edvi is to initialize Edvi with edvi_init(),
 * create a device with edvi_device_new() and a property with
 * edvi_property_new(), to load a document using edvi_document_new(),
 * to create a page using edvi_page_new() and to render the page in an
 * Evas object using edvi_page_render(). Here is an example:
 *
 * @code
 * Edvi_Device   *device;
 * Edvi_Property *property;
 * Edvi_Document *document;
 * Edvi_Page     *page;
 * char          *filename;
 *
 * if (!edvi_init (300, param_kpathsea_mode, 4,
 *                 1.0, 1.0,
 *                 0, 255, 255, 255, 0, 0, 0)) {
 *   // manage error here
 * }
 *
 * device = edvi_device_new (edvi_dpi_get (), edvi_dpi_get ());
 * if (!device) {
 *   // manage error here
 * }
 *
 * property = edvi_property_new ();
 * if (!property) {
 *   // manage error here
 * }
 * edvi_property_property_set (property, EDVI_PROPERTY_DELAYED_FONT_OPEN);
 *
 * document = edvi_document_new (filename);
 * if (!document) {
 *   // manage error here
 * }
 *
 * page = edvi_page_new (document);
 * if (!page) {
 *   // manage error here
 * }
 *
 * o = evas_object_image_add (evas);
 * evas_object_move (o, 0, 0);
 * edvi_page_render (page, device, o);
 * evas_object_show (o);
 *
 * evas_object_del (o);
 * edvi_page_delete (page);
 * edvi_document_delete (document);
 * edvi_property_delete (property);
 * edvi_device_delete (device);
 * edvi_shutdown ();
 * @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif


#include "edvi_enum.h"
#include "edvi_device.h"
#include "edvi_property.h"
#include "edvi_document.h"
#include "edvi_page.h"
#include "edvi_main.h"


#ifdef __cplusplus
}
#endif


#endif /* __EDVI_H__ */
