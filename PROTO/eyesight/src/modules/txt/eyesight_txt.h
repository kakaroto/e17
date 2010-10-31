#ifndef __EYESIGHT_TXT_H__
#define __EYESIGHT_TXT_H__


typedef struct _Eyesight_Backend_Txt Eyesight_Backend_Txt;

struct _Eyesight_Backend_Txt
{
  char *filename;
  Eyesight_Document_Txt *document;
  Evas_Object *obj;
  char *text;

  /* Current page */
  struct {
    Eyesight_Orientation orientation;
    double               hscale;
    double               vscale;
  } page;
};


#endif /* __EYESIGHT_TXT_H__ */
