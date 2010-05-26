/* Project */
#include "../include/elementaryxx/GenListDataModel.h"
#include "../include/elementaryxx/Icon.h"
#include "../include/elementaryxx/Window.h"
#include "../include/elementaryxx/GenListColumnConstructor.h"

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

std::string GenListDataModel::getLabel (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) const
{
  cout << "GenListDataModel::getLabel" << endl;
  
  return "Item";
}

Elmxx::Object *GenListDataModel::getIcon (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
{  
  Window *win = static_cast <Window*> (&obj);
  Icon *ic = Icon::factory (*win);
  ic->setFile ("/home/andreas/Bilder/e_penguin/penguin.png");
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Eflxx::Size (1, 1));
  
  //part: elm.swallow.icon
  //part: elm.swallow.end

  return ic;
}

bool GenListDataModel::getState (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
{
  return true;
}

void GenListDataModel::del (GenListColumnConstructor *construction, Evasxx::Object &obj)
{
  
}

/* wrappers */

char *GenListDataModel::gl_label_get (const void *data, Evas_Object *obj, const char *part)
{
  cout << "gl_label_get" << endl;

  GenListColumnConstructor *construction = (GenListColumnConstructor*) data;
  GenListDataModel *model = construction->mDataModel;
  Evasxx::Object *objWrap = Evasxx::Object::objectLink (obj);
    
  const std::string &label = model->getLabel (construction, *objWrap, part);
    
  return (!label.empty ()) ? strdup (label.c_str ()) : NULL;
}

Evas_Object *GenListDataModel::gl_icon_get (const void *data, Evas_Object *obj, const char *part)
{
  GenListColumnConstructor *construction = (GenListColumnConstructor*) data;
  GenListDataModel *model = construction->mDataModel;
  Evasxx::Object *objWrap = Evasxx::Object::objectLink (obj);
  
  Object *objxx = model->getIcon (construction, *objWrap, part);
  
  return objxx ? objxx->obj () : NULL;
}

Eina_Bool GenListDataModel::gl_state_get (const void *data, Evas_Object *obj, const char *part)
{
  GenListColumnConstructor *construction = (GenListColumnConstructor*) data;
  GenListDataModel *model = construction->mDataModel;
  Evasxx::Object *objWrap = Evasxx::Object::objectLink (obj);
      
  return model->getState (construction, *objWrap, part);
}

void GenListDataModel::gl_del(const void *data, Evas_Object *obj)
{
  GenListColumnConstructor *construction = (GenListColumnConstructor*) data;
  GenListDataModel *model = construction->mDataModel;
  Evasxx::Object *objWrap = Evasxx::Object::objectLink (obj);
      
  return model->del (construction, *objWrap);
}
  
} // end namespace Elmxx
