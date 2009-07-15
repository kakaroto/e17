#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <eflpp_evas.h>
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

class CalibrationRectangle : public EvasRectangle
{
public:
    CalibrationRectangle( int x, int y, int width, int height, EvasCanvas* evas );
    virtual ~CalibrationRectangle();

    Signal done;

    bool calibrate();
    void nextPoint( int x, int y );

protected:
    virtual bool handleShow();
    void moveCrossHair();

private:
    CalibrationData cd;
    unsigned int position;
    EvasGradient* background;
    EvasImage* crosshair;
    EvasLine* crosshairhorz;
    EvasLine* crosshairvert;
    EvasText* text;
};

#endif
