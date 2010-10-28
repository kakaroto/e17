#ifndef __EYESIGHT_IMG_H__
#define __EYESIGHT_IMG_H__


typedef enum
{
  EYESIGHT_IMG_ARCHIVE_NONE,
  EYESIGHT_IMG_ARCHIVE_CBA,
  EYESIGHT_IMG_ARCHIVE_CBR,
  EYESIGHT_IMG_ARCHIVE_CBT,
  EYESIGHT_IMG_ARCHIVE_CBZ,
  EYESIGHT_IMG_ARCHIVE_CB7
} Eyesight_Img_Archive;

typedef struct _Eyesight_Backend_Img Eyesight_Backend_Img;

struct _Eyesight_Backend_Img
{
  char *filename;
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
