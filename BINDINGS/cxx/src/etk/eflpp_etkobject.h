#ifndef EFLPP_ETKOBJECT_H
#define EFLPP_ETKOBJECT_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>

/* EFL */
#include <etk/etk_object.h>

using std::string;

namespace efl {

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
    void init( );
  
    Etk_Object* _o;
    const char* _type;
    bool _managed;
};


} // end namespace efl

#endif // EFLPP_ETKOBJECT_H
