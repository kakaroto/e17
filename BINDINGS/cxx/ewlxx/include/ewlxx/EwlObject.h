#ifndef EFLXX_EWL_OBJECT_H
#define EFLXX_EWL_OBJECT_H

/* EFL */
#include <Ewl.h>

namespace efl {

/* forward declarations */
class Size;
  
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
  Ewl_Object* obj() const 
  {
    return _o;
  };

protected:
  Ewl_Object* _o;
};

} // end namespace efl

#endif // EFLXX_EWL_OBJECT_H
