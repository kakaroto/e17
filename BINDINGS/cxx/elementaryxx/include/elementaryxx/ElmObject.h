#ifndef ELM_OBJECT_H
#define ELM_OBJECT_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include <evasxx/EvasSmart.h>

namespace efl {

class ElmObject : public EvasSmart
{
public:
  virtual void setScale (double scale);
  virtual double getScale ();
  virtual void setStyle (const std::string &style);
  virtual const std::string getStyle ();
  virtual void setDisabled (bool disabled);
  virtual bool getDisabled ();
  
  virtual void focus ();

  /*void         elm_object_scroll_hold_push(Evas_Object *obj);
  void         elm_object_scroll_hold_pop(Evas_Object *obj);
  void         elm_object_scroll_freeze_push(Evas_Object *obj);
  void         elm_object_scroll_freeze_pop(Evas_Object *obj);*/

protected:
  ElmObject (); // allow only construction for child classes
  virtual ~ElmObject (); // forbid direct delete -> use destroy()
  
  void elmInit ();

  void destroy ();
  
private:
  ElmObject (const ElmObject&); // forbid copy constructor

  
  void freeSignalHandler ();
};

} // end namespace efl

#endif // ELM_OBJECT_H

