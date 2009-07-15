#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <eflpp_evas.h>
#include <eflpp_ecore.h>

using namespace efl;

enum
{
    TopLeft = 0,
    TopRight = 1,
    BottomRight = 2,
    BottomLeft = 3,
    Center = 4,
    LastPosition = Center
};

class CalibrationData
{
  public:
    Point devicePoints[5]; // unrotated, unscaled
    Point screenPoints[5]; // unrotated, scaled
    Point canvasPoints[5]; // rotated, scaled
};

class CalibrationRectangle : public EvasRectangle, public Trackable
{
public:
    CalibrationRectangle( int x, int y, int width, int height, EvasCanvas* evas );
    virtual ~CalibrationRectangle();

    Signal done;

    bool calibrate();
    void nextPoint( int x, int y );
    void timerEvent();

protected:
    virtual bool handleMouseUp( const EvasMouseUpEvent& );
    virtual bool handleShow();
    void moveCrossHair();

private:
    CalibrationData cd;
    unsigned int position;
    EvasGradient* background;
    EvasImage* crosshair;
    EvasImage* crosshairShadow;
    EvasLine* crosshairhorz;
    EvasLine* crosshairvert;
    EvasText* text;

    int angle;
};

class CalibrationAnimator : public EcoreAnimator
{
  public:
    enum CAtype { position, size, angle, alpha };
    CalibrationAnimator( CAtype, EvasObject* o, int x, int y );
    ~CalibrationAnimator();
    virtual bool tick();

  private:
    CAtype _t;
    EvasObject* _o;
    int _x;
    int _y;
};

#endif
