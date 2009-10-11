#ifndef ELM_LAYOUT_H
#define ELM_LAYOUT_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

class ElmLayout : public ElmObject
{
public:
  static ElmLayout *factory (EvasObject &parent);
  
  bool setFile (const std::string &file);
  
  bool setFile (const std::string &file, const std::string &group);

  void setContent (const std::string &swallow, const EvasObject &content);
  
   //EAPI Evas_Object *elm_layout_edje_get(const Evas_Object *obj);



private:
  ElmLayout (); // forbid standard constructor
  ElmLayout (const ElmLayout&); // forbid copy constructor
  ElmLayout (EvasObject &parent); // private construction -> use factory ()
  ~ElmLayout (); // forbid direct delete -> use ElmObject::destroy()
};

} // end namespace efl

#endif // ELM_LAYOUT_H
