#ifndef EFLPP_EDJEPART_H
#define EFLPP_EDJEPART_H

/* STL */
#include <string>

/* EFL++ */
#include <eflxx/Common.h>
#include <eflxx/CountedPtr.h>
#include <evasxx/Object.h>

/* EFL */
#include <Edje.h>

using std::string;

namespace Edjexx {

// forward declarations
class ExternalParam;
class Object;

class Part
{
  friend class Object;

private:
  Part( Object* parent, const std::string &partname );

public:
  ~Part();

  Eflxx::Rect getGeometry() const;

  void setText( const std::string &text );
  const std::string getText() const;

  void swallow( Evasxx::Object* );
  void unswallow( Evasxx::Object* );

  /*!
   * @throw SwallowNotExistingException
   */
  Evasxx::Object &getSwallow ();

  /*!
   * @throw ExternalNotExistingException
   */
  Evasxx::Object &getExternalObject ();

  Evasxx::Object &getExternalContent (const std::string &content);

  // TODO: wrap Param?
  bool setParam (Edjexx::ExternalParam *param);

  //const Evasxx::Object* getObject ( const char* name );

private:
  Object* mParent;
  const std::string &mPartname;
  Evasxx::Object *mExternalObject;
  Evasxx::Object *mExternalContent;
  Evasxx::Object *mSwallowObject;

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
  Part();
  Part( const Part& );
  bool operator=( const Part& );
  bool operator==( const Part& );
};

} // end namespace Edjexx

#endif // EFLPP_EDJEPART_H
