#ifndef ESMART_BASE
#define ESMART_BASE

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>

/* EFL */
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Text_Entry.h>

/* STD */
#include <iostream>
using namespace std;

/**
 * C++ Wrapper for the Enlightenment Smart Object Library (ESMART)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

class EvasEsmartContainer : public EvasObject
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
    ~EvasEsmartContainer();

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
    void clean();
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


};

class EvasEsmartTextEntry : public EvasObject
{
  public:
    EvasEsmartTextEntry( EvasCanvas* canvas, const char* name = 0 );
    EvasEsmartTextEntry( int x, int y, EvasCanvas* canvas, const char* name = 0 );
    EvasEsmartTextEntry( int x, int y, int width, int height, EvasCanvas* canvas, const char* name = 0 );
    ~EvasEsmartTextEntry();
};

}

#endif
