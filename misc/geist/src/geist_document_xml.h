#ifndef GEIST_DOCUMENT_XML_H
#define GEIST_DOCUMENT_XML_H

#include "geist.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_rect.h"
#include "geist_text.h"
#include <gnome-xml/parser.h>

enum xml_load_err {ERR_NO_FILE, ERR_EMPTY_FILE, ERR_WRONG_TYPE};

int geist_document_save_xml (geist_document *doc, char *filename);
geist_document *geist_document_load_xml(char *xml, gint * err_return);

#endif
