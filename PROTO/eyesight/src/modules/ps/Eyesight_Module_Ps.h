#ifndef __EYESIGHT_MODULE_PS_H__
#define __EYESIGHT_MODULE_PS_H__


typedef struct
{
  const char *filename;
  const char *title;
  const char *author;
  const char *for_;
  const char *format;
  const char *date_creation;
  int         language_level;
  Eina_Bool   is_eps : 1;
} Eyesight_Document_Ps;


#endif /* __EYESIGHT_MODULE_PS_H__ */
