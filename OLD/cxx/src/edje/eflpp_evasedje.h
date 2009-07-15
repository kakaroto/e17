#ifndef EFLPP_EVASEDJE_H
#define EFLPP_EVASEDJE_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_countedptr.h>

#include "eflpp_evasobject.h"
#include "eflpp_edjepart.h"
#include "eflpp_evascanvas.h"

/* EFL */
#include <Edje.h>

using std::string;

namespace efl {
  
typedef sigc::signal<void, const char*, const char*> EdjeSignalSignal;
typedef sigc::slot2<void, const char*, const char*> EdjeSignalSlot;

class EvasEdje : public EvasObject
{
  public:
    EvasEdje( EvasCanvas* canvas, const char* name = 0 );
    EvasEdje( const char* filename, const char* groupname, EvasCanvas* canvas, const char* name = 0 );
    EvasEdje( int x, int y, const char* filename, const char* groupname, EvasCanvas* canvas, const char* name = 0 );
    
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
    CountedPtr <EdjePart> operator[]( const char* partname );
    
    /*!
     * @param partname Access a EdjePart in the EvasEdje.
     * @throw PartNotExistingException
     */
    CountedPtr <EdjePart> part( const char* partname );

    /* signals and slots */
    void connect( const char* emission, const char* source, const EdjeSignalSlot& slot );
    void emit( const char* emission, const char* source );
    
    static EvasEdje *wrap( Evas_Object* o );

  private:
    // TODO: wrap Edje_Message_Type to avoid include of C header
    static void _edje_message_handler_callback( void* data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg );
    static void _edje_signal_handler_callback( void *data, Evas_Object *obj, const char *emission, const char *source );

    EvasEdje();
    EvasEdje( const EvasEdje& );

    EvasEdje( Evas_Object* object );
    
    bool operator=( const EvasEdje& );
    bool operator==( const EvasEdje& );
};


} // end namespace efl

#endif // EFLPP_EVASEDJE_H
