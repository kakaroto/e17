
/**
 * Something that lives in the engage bar.  There are two parts to this
 * smart object that you should be concerned with
 * behavior: an object to handle mouse interaction separate from representation
 * icon: the image file or edje to represent this element visually
 *
 * Both objects are clipped to a single clip.
 *
 * Image icon files should retain aspect, other than that maintain properly
 * layering etc for this object
 *
 */

#ifndef ENGAGE_ELEMENT_H
#define ENGAGE_ELEMENT_H

typedef struct _Engage_Element Engage_Element;

struct _Engage_Element {
  Evas_Object    *clip;
  Evas_Object    *icon;
  Evas_Object    *behavior;
  Evas_Coord      x, y, w, h;
  int             iw, ih;
};

Evas_Object    *engage_element_new(Evas * evas, const char *bfile,
                                   const char *bgroup, const char *ifile,
                                   const char *igroup);

#endif
