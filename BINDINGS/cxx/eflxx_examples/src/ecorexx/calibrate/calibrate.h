#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>

using namespace Eflxx;

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

class CalibrationRectangle : public Evasxx::Rectangle, public Trackable
{
public:
  CalibrationRectangle( Evasxx::Canvas &evas, const Rect &rect );
  virtual ~CalibrationRectangle();

  Signal done;

  bool calibrate();
  void nextPoint( int x, int y );
  void timerEvent();

protected:
  virtual bool handleMouseUp( const Evasxx::MouseUpEvent& );
  virtual bool handleShow();
  void moveCrossHair();

private:
  CalibrationData cd;
  unsigned int position;
  Evasxx::Rectangle* background;
  Evasxx::Image* crosshair;
  Evasxx::Image* crosshairShadow;
  Evasxx::Line* crosshairhorz;
  Evasxx::Line* crosshairvert;
  Evasxx::Text* text;

  int angle;
};

class CalibrationAnimator : public Ecorexx::Animator
{
public:
  enum CAtype { position, size, angle, alpha };
  CalibrationAnimator( CAtype, Evasxx::Object* o, int x, int y );
  ~CalibrationAnimator();
  virtual bool tick();

private:
  CAtype _t;
  Evasxx::Object* _o;
  int _x;
  int _y;
};

#endif
