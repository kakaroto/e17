#ifndef ESMARTXX_CONTAINER_H
#define ESMARTXX_CONTAINER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <evasxx/Smart.h>

/* EFL */
#include <Esmart/Esmart_Container.h>

namespace Esmartxx {

class Container : public Evasxx::Smart
{
public:

  enum Direction
  {
    Horizontal = CONTAINER_DIRECTION_HORIZONTAL,
    Vertical = CONTAINER_DIRECTION_VERTICAL,
  };

  enum Alignment
  {
    Center = CONTAINER_ALIGN_CENTER,
    Left = CONTAINER_ALIGN_LEFT,
    Right = CONTAINER_ALIGN_RIGHT,
    Bottom = CONTAINER_ALIGN_BOTTOM,
    Top = CONTAINER_ALIGN_TOP,
  };

  enum FillPolicy
  {
    None = CONTAINER_FILL_POLICY_NONE,
    KeepAspect = CONTAINER_FILL_POLICY_KEEP_ASPECT,
    FillX = CONTAINER_FILL_POLICY_FILL_X,
    FillY = CONTAINER_FILL_POLICY_FILL_Y,
    Fill = CONTAINER_FILL_POLICY_FILL,
    Homogenous = CONTAINER_FILL_POLICY_HOMOGENOUS,
  };

public:
  Container( Evasxx::Canvas &canvas);
  Container( Evasxx::Canvas &canvas, Direction dir);
  Container( Evasxx::Canvas &canvas, const Eflxx::Point &pos, Direction dir);
  Container( Evasxx::Canvas &canvas, const Eflxx::Rect &rect, Direction dir);
  virtual ~Container();

public:

  /* Container Properties */
  void setDirection( Direction dir );
  Direction direction() const;
  void setPadding( double l, double r, double t, double b );
  
  /* Padding padding() const; */
  void setFillPolicy( FillPolicy fill );
  FillPolicy getFillPolicy() const;
  void setAlignment( Alignment align );
  Alignment getAlignment() const;
  void setSpacing( int spacing );
  int getSpacing() const;
  void setMovingButton( int move );
  int getMovingButton() const;
  void setLayoutPlugin( const std::string &name );

  /* Adding/Removing Elements */
  void append( Evasxx::Object* object, Evasxx::Object* after = 0 );
  void prepend( Evasxx::Object* object, Evasxx::Object* before = 0 );
  void remove( Evasxx::Object* object );
  void destroy( Evasxx::Object* object );
  void clear();
  //void sort();
  //EvasList* elements() const;

  /* Scrolling */
  void startScrolling( double velocity );
  void stopScrolling();
  void scroll( int val );
  void setScrollOffset( int val );
  int getScrollOffset() const;
  void setScrollPercent( double percent );
  double getScrollPercent() const;
  void scrollTo( Evasxx::Object* object );

  // double esmart_container_elements_length_get(Evas_Object *container);
  // double esmart_container_elements_orig_length_get(Evas_Object *container);
  //int esmart_container_layout_plugin_set(Evas_Object *container, const char *name);

};

} // end namespace Esmartxx

#endif // ESMARTXX_CONTAINER_H
