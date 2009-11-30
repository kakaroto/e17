#include <eflxx/System.h>
#include <eflxx/Debug.h>
#include <evasxx/Evasxx.h>
#include <ecorexx/Ecorexx.h>

using namespace Eflxx;

#include <iostream>
#include <cstring>
using namespace std;

#define WIDTH 240
#define HEIGHT 320

#define DOTSIZE 2

class PaintArea : public Evasxx::Rectangle
{
public:
  PaintArea( Evasxx::Canvas &evas ) :
      Evasxx::Rectangle( evas ),
      mouseDown( false ),
      mEvas (evas)
  {
    setLayer( 10 );
    setColor( Color (255, 255, 255, 0) ); // fully transparent, to catch the mouse events
    setFocus( true );

    signalHandleKeyUp.connect (sigc::mem_fun (this, &PaintArea::handleKeyUp));
    signalHandleMouseUp.connect (sigc::mem_fun (this, &PaintArea::handleMouseUp));
    signalHandleMouseDown.connect (sigc::mem_fun (this, &PaintArea::handleMouseDown));
    signalHandleMouseMove.connect (sigc::mem_fun (this, &PaintArea::handleMouseMove));
  }
  virtual ~PaintArea() { };

  virtual void handleMouseUp( const Evasxx::MouseUpEvent& e )
  {
    cout << "PA::mouseUp" << endl;
    mouseDown = false;
  }
  virtual void handleMouseDown( const Evasxx::MouseDownEvent& e )
  {
    cout << "PA::mouseDown" << endl;
    mouseDown = true;
    Evasxx::Object* l = new Evasxx::Rectangle( mEvas, Rect (e.data->canvas.x, e.data->canvas.y, 2, 2) );
    int b=200+(int) (55.0*rand()/(RAND_MAX+1.0));
    l->setColor( Color (b, b, b, 255) );
    l->show();
  }
  virtual void handleMouseMove( const Evasxx::MouseMoveEvent& e )
  {
    cout << "PA::mouseMove" << endl;
    if ( mouseDown )
    {
      int width = 2 + abs( e.data->cur.canvas.x - e.data->prev.canvas.x );
      int height = 2 + abs( e.data->cur.canvas.y - e.data->prev.canvas.y );
      cout << "width = " << width << ", height = " << height << endl;
      Evasxx::Object* l = new Evasxx::Rectangle( mEvas, Rect (e.data->cur.canvas.x - width/2, e.data->cur.canvas.y - height/2, width, height) );
      int b=200+(int) (55.0*rand()/(RAND_MAX+1.0));
      l->setColor( Color (b, b, b, 255) );
      l->show();
    }
  }
  virtual void handleKeyUp( const Evasxx::KeyUpEvent& e )
  {
    cout << "PA:::keyUp - released '" << e.data->keyname << "'" << endl;
    if ( strcmp( e.data->keyname, "Escape" ) == 0 )
    {
      Ecorexx::Application::quit();
    }
    if ( strcmp( e.data->keyname, "0x5b" ) == 0 )
    {
      int rot = Ecorexx::Application::getInstance()->getMainWindow()->rotation();
      rot = rot ? 0: 270;
      Ecorexx::Application::getInstance()->getMainWindow()->setRotation( rot );
      //eApp->mainWindow()->resize(
    }
    if ( strcmp( e.data->keyname, "j" ) == 0 )
    {
      Rect vp = Ecorexx::Application::getInstance()->getMainWindow()->getCanvas().getViewport();
      Ecorexx::Application::getInstance()->getMainWindow()->getCanvas().setViewport( Rect (vp.x()+10, vp.y()+10, vp.width()-10, vp.height()-10) );
    }
    if ( strcmp( e.data->keyname, "k" ) == 0 )
    {
      Rect vp = Ecorexx::Application::getInstance()->getMainWindow()->getCanvas().getViewport();
      Ecorexx::Application::getInstance()->getMainWindow()->getCanvas().setViewport( Rect (vp.x()-10, vp.y()-10, vp.width()+10, vp.height()+10) );
    }

  }
private:
  bool mouseDown;
  Evasxx::Canvas &mEvas;
};

int main( int argc, const char **argv )
{
  /* Create the application object */
  Ecorexx::Application* app = new Ecorexx::Application( argc, argv, "Ecore Paint Example" );

  /* Create the main window, a window with an embedded canvas */
  Ecorexx::EvasWindowSoftwareX11* mw = new Ecorexx::EvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );
  mw->setAlpha (true);

  mw->show ();

  /* Create some objects on the canvas */
  Evasxx::Canvas &evas = mw->getCanvas();

  Size s = evas.getSize();
  cout << "PA: Size = " << s << endl;

  Evasxx::Rectangle* bg = new Evasxx::Rectangle( evas, s );
  bg->setColor( Color (50, 50, 50, 128) );
  bg->setLayer( 0 );
  bg->show();

  PaintArea* p = new PaintArea( evas );
  p->resize( s );
  p->setLayer( 1 );
  p->show();

  /* Enter the application main loop */
  app->exec();

  /* Delete the application */
  delete app;

  return 0;
}

