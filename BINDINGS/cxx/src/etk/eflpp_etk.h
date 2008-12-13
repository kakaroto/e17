#ifndef ETK_BASE
#define ETK_BASE

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>

/* EFL */
#include <etk/Etk.h>

#define etkApp EtkApplication::application()

namespace efl {

class EtkObject;
class EtkWidget;

class EtkObject
{
    public:
    EtkObject( EtkObject* parent = 0, const char* type = "<unknown>", const char* name = 0 );
    virtual ~EtkObject();

    void setFillPolicy(  );
    void setAlignment(  );

    void resize( const Size& size );

    public:
    Etk_Object* obj() const { return _o; };

    protected:
    Etk_Object* _o;
    const char* _type;
};

class EtkWidget : public EtkObject
{
    public:
    EtkWidget( EtkObject* parent = 0, const char* type = "Widget", const char* name = 0 );
    virtual ~EtkWidget();

    void show();
    void showAll();
    void hide();
    void hideAll();

    bool isVisible() const;
    void setVisibilityLock( bool );
    bool visibilityLock() const;
};

class EtkContainer : public EtkWidget
{
    public:
    EtkContainer( EtkObject* parent = 0, const char* type = "Container", const char* name = 0 );
    virtual ~EtkContainer();

    void appendChild( EtkWidget* child );
    void setBorderWidth( int );
};

class EtkTopLevelWidget : public EtkContainer
{
    public:
    EtkTopLevelWidget( EtkObject* parent = 0, const char* type = "TopLevelWidget", const char* name = 0 );
    virtual ~EtkTopLevelWidget();
};

class EtkBox : public EtkContainer
{
    public:
    EtkBox( EtkObject* parent = 0, const char* type = "Box", const char* name = 0 );
    virtual ~EtkBox();
};

class EtkHBox : public EtkBox
{
    public:
    EtkHBox( EtkObject* parent = 0, const char* type = "HBox", const char* name = 0 );
    virtual ~EtkHBox();
};

class EtkVBox : public EtkBox
{
    public:
    EtkVBox( EtkObject* parent = 0, const char* type = "VBox", const char* name = 0 );
    virtual ~EtkVBox();
};

class EtkButton : public EtkBox
{
    public:
    EtkButton( EtkObject* parent = 0, const char* type = "Button", const char* name = 0 );
    EtkButton( const char* text, EtkObject* parent = 0, const char* type = "Button", const char* name = 0 );
    virtual ~EtkButton();

    void setText( const char* text );
};

class EtkEmbed : public EtkTopLevelWidget
{
    public:
    EtkEmbed( EvasCanvas*, EtkObject* parent = 0, const char* type = "Embed", const char* name = 0 );
    virtual ~EtkEmbed();

    void setFocus( bool b );
};

class EvasEtk : public EvasObject
{
    public:
    EvasEtk( EtkEmbed* ewlobj, const char* name = 0 );
    ~EvasEtk();
};

class EtkWindow : public EtkTopLevelWidget
{
    public:
    EtkWindow( EtkObject* parent = 0, const char* type = "Window", const char* name = 0 );
    virtual ~EtkWindow();

    void setTitle( const char* title );
};

class EtkApplication
{
    public:
    EtkApplication( int argc, const char** argv, const char* name );
    ~EtkApplication();

    static EtkApplication* application();
    const char* name() const { return _name; };

    /* Main Window */
    void setMainWindow( EtkWindow* );
    EtkWindow* mainWindow() const;

    /* Main Loop */
    void exec();
    void exit();

    private:
    const char* _name;
    static EtkApplication* _instance;
    EtkWindow* _mainWindow;

};

}
#endif
