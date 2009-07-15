#ifndef EFLPP_ESMART_GROUP_H
#define EFLPP_ESMART_GROUP_H

#include <evasxx/EvasSmart.h>

#include <list>

namespace efl {

class EvasSmartGroup : public EvasSmart
{
public:
  EvasSmartGroup( EvasCanvas &canvas );    
  EvasSmartGroup( EvasCanvas &canvas, const Point &pos );
  EvasSmartGroup( EvasCanvas &canvas, const Rect &rect );
  virtual ~EvasSmartGroup();

  void append (EvasObject* object); // TODO: ,  EvasObject* after = 0
  void prepend (EvasObject* object); // TODO: , EvasObject* before = 0
  void remove (EvasObject* object);
  void clear();

private:
  virtual void addHandler();
  virtual void delHandler();
  virtual void moveHandler (const Point &pos);
  virtual void resizeHandler (const Size &size);
  virtual void showHandler();
  virtual void hideHandler();
  virtual void colorSetHandler (const Color &color);
  virtual void clipSetHandler( Evas_Object *clip );
  virtual void clipUnsetHandler();

  void connectSignals ();
  
  std::list<EvasObject*> mEvasObjectList;
};

} // end namespace efl

#endif // EFLPP_ESMART_GROUP_H
