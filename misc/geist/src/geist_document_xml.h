#include "geist.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_rect.h"
#include "geist_text.h"
#include <gnome-xml/parser.h>

enum xml_load_err {ERR_NO_FILE, ERR_EMPTY_FILE, ERR_WRONG_TYPE};

geist_document *
geist_project_load_xml (char *xml, int from_mem, int * err_return);
int
geist_project_save_xml (geist_document *doc, char *filename);
