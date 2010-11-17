/* Project */
#include "../include/elementaryxx/GenListDataModel.h"
#include "../include/elementaryxx/Icon.h"
#include "../include/elementaryxx/Window.h"
#include "../include/elementaryxx/GenListColumnConstructor.h"

/* STD */
#include <iostream>
#include <cassert>

using namespace std;

namespace Elmxx {

GenListDataModel::GenListDataModel (const std::string &style) :
  mStyle (style)
{
  cout << "creating GenListDataModel with style = " << mStyle << endl;
  mGLIC.item_style     = mStyle.c_str ();
  mGLIC.func.label_get = GenListDataModel::gl_label_get;
  mGLIC.func.icon_get  = GenListDataModel::gl_icon_get;
  mGLIC.func.state_get = GenListDataModel::gl_state_get;
  mGLIC.func.del       = GenListDataModel::gl_del; 
}

/* wrappers */

char *GenListDataModel::gl_label_get (void *data, Evas_Object *obj, const char *part)
{
  cout << "gl_label_get" << endl;

  GenListColumnConstructor *construction = static_cast <GenListColumnConstructor*> (
                                           const_cast <void*> (data));
  GenListDataModel *model = construction->mDataModel;
  Evasxx::Object *objWrap = Evasxx::Object::objectLink (obj);
    
  const std::string label = model->getLabel (construction, *objWrap, part);

  static int counter = 0;
  ++counter;
  cout << "label: " << label << " ," << counter << endl;
    
  return (!label.empty ()) ? strdup (label.c_str ()) : NULL;
}

Evas_Object *GenListDataModel::gl_icon_get (void *data, Evas_Object *obj, const char *part)
{
  GenListColumnConstructor *construction = static_cast <GenListColumnConstructor*> (
                                           const_cast <void*> (data));
  GenListDataModel *model = construction->mDataModel;
  Evasxx::Object *objWrap = Evasxx::Object::objectLink (obj);
  
  Object *objxx = model->getIcon (construction, *objWrap, part);
  
  return objxx ? objxx->obj () : NULL;
}

Eina_Bool GenListDataModel::gl_state_get (void *data, Evas_Object *obj, const char *part)
{
  GenListColumnConstructor *construction = static_cast <GenListColumnConstructor*> (
                                           const_cast <void*> (data));
  GenListDataModel *model = construction->mDataModel;
  Evasxx::Object *objWrap = Evasxx::Object::objectLink (obj);
      
  return model->getState (construction, *objWrap, part);
}

void GenListDataModel::gl_del(void *data, Evas_Object *obj)
{
  GenListColumnConstructor *construction = static_cast <GenListColumnConstructor*> (
                                           const_cast <void*> (data));
  GenListDataModel *model = construction->mDataModel;
  Evasxx::Object *objWrap = Evasxx::Object::objectLink (obj);
  assert (model);
  assert (objWrap);
  
  // TODO: delete all allocated menuitem/construction data
  // TODO: also call virtual function to allow user code to react on delete event
}
  
} // end namespace Elmxx
