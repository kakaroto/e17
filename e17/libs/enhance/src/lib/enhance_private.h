#ifndef EN_PRIV_H
#define EN_PRIV_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <EXML.h>
#include <Ecore_Data.h>
#include <Evas.h>
#include <Etk.h>

#include "config.h"

#include "Enhance.h"
#include "enhance_widget.h"

typedef void (*etk_callback_type)(void);

void      _e_property_handle(Enhance *en, EXML_Node *node);
void      _e_signal_handle(Enhance *en, EXML_Node *node);
void      _e_traverse_packing_xml(Enhance *en, E_Widget *widget);
void      _e_traverse_property_xml(Enhance *en);
void      _e_traverse_signal_xml(Enhance *en);
void      _e_traverse_child_xml(Enhance *en);
E_Widget *_e_traverse_widget_xml(Enhance *en);
void      _e_traverse_xml(Enhance *en);
void      _en_stock_items_hash_init(void);

#endif
