#ifndef __EYESIGHT_PS_H__
#define __EYESIGHT_PS_H__


typedef struct _Eyesight_Backend_Ps Eyesight_Backend_Ps;

struct _Eyesight_Backend_Ps
{
  char *filename;
  Eyesight_Document_Ps *doc;
  Evas_Object *obj;

  SpectreDocument      *document;
  SpectrePage          *page;
  SpectreRenderContext *rc;
  int                   page_count;
};


#endif /* __EYESIGHT_PS_H__ */
