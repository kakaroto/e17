#ifndef EWL_BASE
#define EWL_BASE

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>

/* EFL */
#include <Ewl.h>

#define ewlApp EwlApplication::application()

namespace efl {

class EwlObject;

enum Orientation
{
    Horizontal = EWL_ORIENTATION_HORIZONTAL,
    Vertical   = EWL_ORIENTATION_VERTICAL,
};

enum FillPolicy
{
    None = EWL_FLAG_FILL_NONE,
    ShrinkHorizontal = EWL_FLAG_FILL_HSHRINK,
    ShrinkVertical = EWL_FLAG_FILL_VSHRINK,
    Shrink = EWL_FLAG_FILL_SHRINK,
    FillHorizontal = EWL_FLAG_FILL_HFILL,
    FillVertical = EWL_FLAG_FILL_VFILL,
    Fill = EWL_FLAG_FILL_FILL,
    All = EWL_FLAG_FILL_ALL,
};

enum Alignment
{
    Center = EWL_FLAG_ALIGN_CENTER,
    Left = EWL_FLAG_ALIGN_LEFT,
    Right = EWL_FLAG_ALIGN_RIGHT,
    Top = EWL_FLAG_ALIGN_TOP,
    Bottom = EWL_FLAG_ALIGN_BOTTOM,
};

class EwlObject
{
    public:
    EwlObject( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlObject();

    void setFillPolicy( FillPolicy );
    void setAlignment( Alignment );

    void resize( const Size& size );

    public:
    Ewl_Object* obj() const { return _o; };

    protected:
    Ewl_Object* _o;
};

class EwlWidget : public EwlObject
{
    public:
    EwlWidget( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlWidget();

    void show();
};

class EwlContainer : public EwlWidget
{
    public:
    EwlContainer( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlContainer();

    void appendChild( EwlWidget* child );
};

class EwlOverlay : public EwlContainer
{
    public:
    EwlOverlay( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlOverlay();
};

class EwlBox : public EwlContainer
{
    public:
    EwlBox( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlBox();
};

class EwlHBox : public EwlBox
{
    public:
    EwlHBox( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlHBox();
};

class EwlVBox : public EwlBox
{
    public:
    EwlVBox( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlVBox();
};

class EwlButton : public EwlBox
{
    public:
    EwlButton( EwlObject* parent = 0, const char* name = 0 );
    EwlButton( const char* text, EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlButton();

    void setText( const char* text );
};

class EwlEmbed : public EwlOverlay
{
    public:
    EwlEmbed( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlEmbed();

    void setFocus( bool b );
};

class EvasEwl : public EvasObject
{
    public:
    EvasEwl( EwlEmbed* ewlobj, EvasCanvas* canvas, const char* name = 0 );
    ~EvasEwl();
};

class EwlWindow : public EwlEmbed
{
    public:
    EwlWindow( EwlObject* parent = 0, const char* name = 0 );
    virtual ~EwlWindow();

    void setTitle( const char* title );
};

class EwlApplication
{
    public:
    EwlApplication( int argc, const char** argv, const char* name );
    ~EwlApplication();

    static EwlApplication* application();
    const char* name() const { return _name; };

    /* Main Window */
    void setMainWindow( EwlWindow* );
    EwlWindow* mainWindow() const;

    /* Main Loop */
    void exec();
    void exit();

    private:
    const char* _name;
    static EwlApplication* _instance;
    EwlWindow* _mainWindow;

};

}
#endif
