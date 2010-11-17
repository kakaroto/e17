#ifndef ELMXX_GEN_LIST_DATA_MODEL_H
#define ELMXX_GEN_LIST_DATA_MODEL_H

/* EFL */
#include <evasxx/Evasxx.h>
#include <Elementary.h>

/* STD */
#include <string>

namespace Elmxx {

/* forward declarations */
class GenList;
class Icon;
class Window;
class Object;
class GenListColumnConstructor;

class GenListDataModel
{
public:
  friend class GenList;
  
  GenListDataModel (const std::string &style);
  virtual ~GenListDataModel () {}

  virtual std::string getLabel (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) const = 0;

  virtual Elmxx::Object *getIcon (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) = 0;

  virtual bool getState (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) = 0;
  
private:
  static char *gl_label_get (void *data, Evas_Object *obj, const char *part);
  static Evas_Object *gl_icon_get (void *data, Evas_Object *obj, const char *part);
  static Eina_Bool gl_state_get (void *data, Evas_Object *obj, const char *part);
  static void gl_del (void *data, Evas_Object *obj);
  
  Elm_Genlist_Item_Class mGLIC;
  std::string mStyle;
};

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_DATA_MODEL_H
