#ifndef _EFLAME_H__
#define _EFLAME_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  
  /* Ecore header */
#include <Ecore.h>
#include <Ecore_Evas.h>
  
#define IMAX 300
  
  /* the global data structure */
  typedef struct
  {
    /* the window */
    Ecore_Evas  *ee;
    Evas        *evas;
    Evas_Object *o_flame;
    int w, h;

    /* the palette */
    unsigned int pal[IMAX];
    unsigned int *im;
    int ims;

    /* the flame arrays */
    int ws;
    unsigned int *flame, *flame2;

  }global;
  
  int  ef_init       (int argc, const char **argv);
  int  ef_draw_flame (void *data);
  void ef_flame      (int w, int h);
  void ef_finish     (void);
  
  /*
   * functions that acts on the flame arrays
   */
  
  void ef_set_palette           (unsigned int *pal);
  void ef_set_flame_zero        (void);
  void ef_set_random_flame_base (void);
  void ef_modify_flame_base     (void);
  void ef_process_flame         (void);
  
  int powerof (unsigned int);
  
  /* 
   * Internally used callbacks. Override at your own risks 
   */
  
  void delete_request_cb (Ecore_Evas *ee);
  int  signal_exit_cb    (void *data, int ev_type, void *ev);
  
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _EFLAME_H__ */
