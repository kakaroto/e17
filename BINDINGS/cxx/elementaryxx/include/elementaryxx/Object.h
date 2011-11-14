#ifndef ELMXX_OBJECT_H
#define ELMXX_OBJECT_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include <evasxx/Smart.h>

namespace Elmxx {

class Object : public Evasxx::Smart
{
public:
  virtual void setScale (double scale);
  virtual double getScale ();
  virtual void setStyle (const std::string &style);
  virtual const std::string getStyle ();
  virtual void setDisabled (bool disabled);
  virtual bool getDisabled ();
  
  virtual void focus ();

  /*!
   * Check if the given Evas Object is an Elementary widget.
   */
  bool checkWidget ();
 
  /*!
   * Get the first parent of the given object that is an Elementary widget.
   */
 	Eflxx::CountedPtr <Evasxx::Object> getParentWidget ();
    
  /*!
   * Get the top level parent of an Elementary widget.
   */
 	Eflxx::CountedPtr <Evasxx::Object> getTopWidget ();

  /*!
   * Get the string that represents this Elementary widget. 
   */
  const std::string getWidgetType ();	

  /*void         elm_object_scroll_hold_push(Evas_Object *obj);
  void         elm_object_scroll_hold_pop(Evas_Object *obj);
  void         elm_object_scroll_freeze_push(Evas_Object *obj);
  void         elm_object_scroll_freeze_pop(Evas_Object *obj);*/

  void destroy ();
  
protected:
  Object (); // allow only construction for child classes
  virtual ~Object (); // forbid direct delete -> use destroy()
  
  void elmInit ();
  
private:
  Object (const Object&); // forbid copy constructor

  void freeSignalHandler ();
};

} // end namespace Elmxx

#endif // ELMXX_OBJECT_H

