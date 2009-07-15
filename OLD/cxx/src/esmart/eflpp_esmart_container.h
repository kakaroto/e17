#ifndef EFLPP_ESMART_CONTAINER_H
#define EFLPP_ESMART_CONTAINER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <eflpp_esmart.h>

/* EFL */
#include <Esmart/Esmart_Container.h>

namespace efl {

class EvasEsmartContainer : public EvasEsmart
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
  EvasEsmartContainer( EvasCanvas* canvas, const char* name = 0 );
  EvasEsmartContainer( Direction dir, EvasCanvas* canvas, const char* name = 0 );
  EvasEsmartContainer( int x, int y, Direction dir, EvasCanvas* canvas, const char* name = 0 );
  EvasEsmartContainer( int x, int y, int width, int height, Direction dir, EvasCanvas* canvas, const char* name = 0 );
  virtual ~EvasEsmartContainer();

public:

  /* Container Properties */
  void setDirection( Direction dir );
  Direction direction() const;
  void setPadding( double l, double r, double t, double b );
  /* Padding padding() const; */
  void setFillPolicy( FillPolicy fill );
  FillPolicy fillPolicy() const;
  void setAlignment( Alignment align );
  Alignment alignment() const;
  void setSpacing( int spacing );
  int spacing() const;
  void setMovingButton( int move );
  int movingButton() const;
  void setLayoutPlugin( const char* name );

  /* Adding/Removing Elements */
  void append( EvasObject* object, EvasObject* after = 0 );
  void prepend( EvasObject* object, EvasObject* before = 0 );
  void remove( EvasObject* object );
  void destroy( EvasObject* object );
  void clear();
  //void sort();
  //EvasList* elements() const;

  /* Scrolling */
  void startScrolling( double velocity );
  void stopScrolling();
  void scroll( int val );
  void setScrollOffset( int val );
  int scrollOffset() const;
  void setScrollPercent( double percent );
  double scrollPercent() const;
  void scrollTo( EvasObject* object );

  // double esmart_container_elements_length_get(Evas_Object *container);
  // double esmart_container_elements_orig_length_get(Evas_Object *container);
  //int esmart_container_layout_plugin_set(Evas_Object *container, const char *name);

protected:

  virtual void addHandler() {};
  virtual void delHandler() {};
  virtual void moveHandler( Evas_Coord x, Evas_Coord y ) {};
  virtual void resizeHandler( Evas_Coord w, Evas_Coord h ) {};
  virtual void showHandler() {};
  virtual void hideHandler() {};
  virtual void colorSetHandler( int r, int g, int b, int a ) {};
  virtual void clipSetHandler( Evas_Object *clip ) {};
  virtual void clipUnsetHandler() {};
};

} // end namespace efl

#endif // EFLPP_ESMART_CONTAINER_H
