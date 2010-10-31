#ifndef __EYESIGHT_MODULE_IMG_H__
#define __EYESIGHT_MODULE_IMG_H__


typedef enum
{
  EYESIGHT_IMG_ARCHIVE_NONE,
  EYESIGHT_IMG_ARCHIVE_CBA,
  EYESIGHT_IMG_ARCHIVE_CBR,
  EYESIGHT_IMG_ARCHIVE_CBT,
  EYESIGHT_IMG_ARCHIVE_CBZ,
  EYESIGHT_IMG_ARCHIVE_CB7
} Eyesight_Img_Archive;


typedef struct
{
  const char *filename;
  Eyesight_Img_Archive archive;
} Eyesight_Document_Img;


#endif /* __EYESIGHT_MODULE_IMG_H__ */
