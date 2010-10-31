#ifndef __EYESIGHT_IMG_H__
#define __EYESIGHT_IMG_H__


typedef struct _Eyesight_Backend_Img Eyesight_Backend_Img;

struct _Eyesight_Backend_Img
{
  char *filename;
  Eyesight_Document_Img *document;
  Evas_Object *obj;
  Eina_Bool is_archive;
  Eyesight_Img_Archive archive;
  char *archive_path;

  /* Document */
  struct {
    Eina_List       *toc;
  } doc;

  /* Current page */
  struct {
    int                  page;
    Eyesight_Orientation orientation;
    double               hscale;
    double               vscale;
  } page;
};


#endif /* __EYESIGHT_IMG_H__ */
