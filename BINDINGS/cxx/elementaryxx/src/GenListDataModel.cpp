/* Project */
#include "../include/elementaryxx/GenListDataModel.h"

/* STD */
#include <iostream>

using namespace std;

namespace Elmxx {

GenListDataModel::GenListDataModel ()
{
  mGLIC.item_style     = "default";
  mGLIC.func.label_get = GenListDataModel::gl_label_get;
  mGLIC.func.icon_get  = GenListDataModel::gl_icon_get;
  mGLIC.func.state_get = GenListDataModel::gl_state_get;
  mGLIC.func.del       = GenListDataModel::gl_del; 
}

std::string GenListDataModel::getLabel (Evasxx::Object &obj, const std::string &part) const
{
  cout << "GenListDataModel::getLabel" << endl;
  
  return "Item";
}

/* wrappers */

char *GenListDataModel::gl_label_get (const void *data, Evas_Object *obj, const char *part)
{
  cout << "gl_label_get" << endl;
  
  GenListDataModel *model = (GenListDataModel*) data;
  Evasxx::Object *objWrap = Evasxx::Object::wrap (obj);
  
  std::string label = model->getLabel (*objWrap, part);
  
  // FIXME: from logic this should be deleted, but results in a segfault. no idea why...
  //delete objWrap;
  
  return strdup (label.c_str ());
}

Evas_Object *GenListDataModel::gl_icon_get (const void *data, Evas_Object *obj, const char *part)
{
  return NULL;
}

Eina_Bool GenListDataModel::gl_state_get (const void *data, Evas_Object *obj, const char *part)
{
  return true;
}

void GenListDataModel::gl_del(const void *data, Evas_Object *obj)
{
  printf ("del\n");
}
  
} // end namespace Elmxx
