#ifndef ESMARTXX_GROUP_H
#define ESMARTXX_GROUP_H

#include <evasxx/Smart.h>

#include <list>

namespace Esmartxx {

class Group : public Evasxx::Smart
{
public:
  Group( Evasxx::Canvas &canvas );    
  Group( Evasxx::Canvas &canvas, const Eflxx::Point &pos );
  Group( Evasxx::Canvas &canvas, const Eflxx::Rect &rect );
  virtual ~Group();

  void append (Evasxx::Object* object); // TODO: ,  Evasxx::Object* after = 0
  void prepend (Evasxx::Object* object); // TODO: , Evasxx::Object* before = 0
  void remove (Evasxx::Object* object);
  void clear();

private:
  virtual void addHandler();
  virtual void delHandler();
  virtual void moveHandler (const Eflxx::Point &pos);
  virtual void resizeHandler (const Eflxx::Size &size);
  virtual void showHandler();
  virtual void hideHandler();
  virtual void colorSetHandler (const Eflxx::Color &color);
  virtual void clipSetHandler( Evas_Object *clip );
  virtual void clipUnsetHandler();

  void connectSignals ();
  
  std::list<Evasxx::Object*> mEvasObjectList;
};

} // end namespace Esmartxx

#endif // ESMARTXX_GROUP_H
