#ifndef EDJE_BASE
#define EDJE_BASE

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_evas.h>
#include <eflpp_evasutils.h>
#include <eflpp_common.h>

/* EFL */
#include <Edje.h>

/* STD */
#include <map>

/**
 * C++ Wrapper for the Enlightenment Edje Library (EDJE)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

class EdjePart;
class EvasEdje;

typedef std::map<const char*, EdjePart*> EdjePartMap;

typedef sigc::signal<void, const char*, const char*> EdjeSignalSignal;
typedef sigc::slot2<void, const char*, const char*> EdjeSignalSlot;

class PartNotExistingException : public std::exception
{
public:
  PartNotExistingException (const char *partname) : txt (partname) {}
  const char* what () const throw ();
    
private:
  const char *txt;
};
  
//===============================================================================================
// Edje
//===============================================================================================
class Edje
{
  public:
    static int init();
    static int shutdown();

    static double frametime();
    static void setFrametime( double );

    static void freeze();
    static void thaw();

    static const char* fontset();
    static void setFontSet( const char* fonts );

    static EvasList<const char>* collection( const char* filename );
    static void freeCollection( EvasList<const char>* );

    static const char* data( const char* filename, const char* key );

    static void setFileCache( int size );
    static int fileCache();
    static void flushFileCache();

    static void setCollectionCache( int size );
    static int collectionCache();
    static void flushCollectionCache();

    static void setColorClass( const char* classname, const Color& object, const Color& outline, const Color& shadow );
    static void setTextClass( const char* classname, const char* fontname, int size );

    /*
    EAPI void         edje_extern_object_min_size_set (Evas_Object *obj, Evas_Coord minw, Evas_Coord minh);
    EAPI void         edje_extern_object_max_size_set (Evas_Object *obj, Evas_Coord maxw, Evas_Coord maxh);
    */
  private:
    Edje();
    Edje( const Edje& );
    ~Edje();
};

//===============================================================================================
// EdjePart
//===============================================================================================
class EdjePart
{
  friend class EvasEdje;

  private:
    EdjePart( EvasEdje* parent, const char* partname );
    ~EdjePart();

  public:
    Rect geometry() const;

    void setText( const char* text );
    const char* text() const;

    void swallow( EvasObject* );
    void unswallow( EvasObject* );
    EvasObject* swallow();

  private:
    EvasEdje* _parent;
    const char* _partname;

    /* State?
    EAPI const char  *edje_object_part_state_get      (Evas_Object *obj, const char *part, double *val_ret);
    */

    /* Directions?
    EAPI int          edje_object_part_drag_dir_get   (Evas_Object *obj, const char *part);
    */

    /* Drag?
    EAPI void         edje_object_part_drag_value_set (Evas_Object *obj, const char *part, double dx, double dy);
    EAPI void         edje_object_part_drag_value_get (Evas_Object *obj, const char *part, double *dx, double *dy);
    EAPI void         edje_object_part_drag_size_set  (Evas_Object *obj, const char *part, double dw, double dh);
    EAPI void         edje_object_part_drag_size_get  (Evas_Object *obj, const char *part, double *dw, double *dh);
    EAPI void         edje_object_part_drag_step_set  (Evas_Object *obj, const char *part, double dx, double dy);
    EAPI void         edje_object_part_drag_step_get  (Evas_Object *obj, const char *part, double *dx, double *dy);
    EAPI void         edje_object_part_drag_page_set  (Evas_Object *obj, const char *part, double dx, double dy);
    EAPI void         edje_object_part_drag_page_get  (Evas_Object *obj, const char *part, double *dx, double *dy);
    EAPI void         edje_object_part_drag_step      (Evas_Object *obj, const char *part, double dx, double dy);
    EAPI void         edje_object_part_drag_page      (Evas_Object *obj, const char *part, double dx, double dy);
    */
  private:
    EdjePart();
    EdjePart( const EdjePart& );
    bool operator=( const EdjePart& );
    bool operator==( const EdjePart& );
};

//===============================================================================================
// EvasEdje
//===============================================================================================
class EvasEdje : public EvasObject
{
  public:
    EvasEdje( EvasCanvas* canvas, const char* name = 0 );
    EvasEdje( const char* filename, const char* groupname, EvasCanvas* canvas, const char* name = 0 );
    EvasEdje( int x, int y, const char* filename, const char* groupname, EvasCanvas* canvas, const char* name = 0 );
    //EvasEdje( Evas_Object* object, EvasCanvas* canvas, const char* name = 0 );
    ~EvasEdje();

    /*
    EAPI void         edje_object_signal_callback_add (Evas_Object *obj, const char *emission, const char *source, void (*func) (void *data, Evas_Object *obj, const char *emission, const char *source), void *data);

    EAPI void        *edje_object_signal_callback_del (Evas_Object *obj, const char *emission, const char *source, void (*func) (void *data, Evas_Object *obj, const char *emission, const char *source));

    EAPI void         edje_object_signal_emit         (Evas_Object *obj, const char *emission, const char *source);

    EAPI void         edje_object_text_change_cb_set  (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, const char *part), void *data);

    EAPI void         edje_object_message_send           (Evas_Object *obj, Edje_Message_Type type, int id, void *msg);

    EAPI void         edje_object_message_handler_set    (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg), void *data);

    EAPI void         edje_object_message_signal_process (Evas_Object *obj);

    EAPI void         edje_message_signal_process        (void);
    */

    bool setFile( const char* filename, const char* partname );

    void setPlaying( bool b );
    bool isPlaying() const;
    void setAnimated( bool b );
    bool isAnimated() const;

    int freeze();
    int thaw();

    void setColorClass( const char* colorclass, const Color& object, const Color& outline, const Color& shadow );
    void setTextClass( const char* textclass, const char* fontname, int size );

    void setMinimumSize( const Size& size );
    void setMaximumSize( const Size& size );
    Size minimalSize() const;
    Size maximumSize() const;

    void recalculateLayout();

    bool hasPart( const char* partname ) const;
    EdjePart* operator[]( const char* partname );
    
    /*!
     * @param partname Loads a EdjePart into the EvasEdje.
     * @throw PartNotExistingException
     */
    EdjePart* part( const char* partname );

    /* signals and slots */
    void connect( const char* emission, const char* source, const EdjeSignalSlot& slot );
    void emit( const char* emission, const char* source );

  protected:
    mutable EdjePartMap _parts;

  private:
    static void _edje_message_handler_callback( void* data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg );
    static void _edje_signal_handler_callback( void *data, Evas_Object *obj, const char *emission, const char *source );

    EvasEdje();
    EvasEdje( const EvasEdje& );
    bool operator=( const EvasEdje& );
    bool operator==( const EvasEdje& );
};

}

#endif
