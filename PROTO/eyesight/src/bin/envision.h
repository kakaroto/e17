#ifndef ENVISION_H
#define ENVISION_H


extern int _envision_log_domain;

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_envision_log_domain, __VA_ARGS__)
#define ERR(...)      EINA_LOG_DOM_ERR(_envision_log_domain, __VA_ARGS__)
#define WRN(...)      EINA_LOG_DOM_WARN(_envision_log_domain, __VA_ARGS__)
#define INF(...)      EINA_LOG_DOM_INFO(_envision_log_domain, __VA_ARGS__)
#define DBG(...)      EINA_LOG_DOM_DBG(_envision_log_domain, __VA_ARGS__)

typedef struct _Envision Envision;

struct _Envision
{
  struct {
    Evas_Object *window;
    Evas_Object *background;
    Evas_Object *box;
    Evas_Object *scroller;
    Evas_Object *genlist;
  } gui;
  Evas_Object *obj;
  char        *file;
  int          page_nbr;
  double       scale;
  unsigned int start_with_file : 1;
  unsigned int gui_built : 1;
};

#endif /* ENVISION_H */
