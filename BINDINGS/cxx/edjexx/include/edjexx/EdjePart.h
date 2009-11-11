#ifndef EFLPP_EDJEPART_H
#define EFLPP_EDJEPART_H

/* STL */
#include <string>

/* EFL++ */
#include <eflxx/eflpp_common.h>
#include <eflxx/CountedPtr.h>

#include <evasxx/EvasObject.h>

using std::string;

namespace efl {

class EdjeObject;

class EdjePart
{
  friend class EdjeObject;

private:
  EdjePart( EdjeObject* parent, const std::string &partname );

public:
  ~EdjePart();

  Rect getGeometry() const;

  void setText( const std::string &text );
  const std::string getText() const;

  void swallow( EvasObject* );
  void unswallow( EvasObject* );

  CountedPtr <EvasObject> swallow();

  //const EvasObject* getObject ( const char* name );

private:
  EdjeObject* _parent;
  const std::string &_partname;

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

} // end namespace efl

#endif // EFLPP_EDJEPART_H
