#ifndef EFLPP_ESMART_TEXTENTRY_H
#define EFLPP_ESMART_TEXTENTRY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <eflpp_esmart.h>
#include <eflpp_edje.h>

/* EFL */
#include <Esmart/Esmart_Text_Entry.h>

namespace efl {

class EvasEsmartTextEntry : public EvasEsmart
{
public:
  EvasEsmartTextEntry( EvasCanvas* canvas, const char* name = 0 );
  EvasEsmartTextEntry( int x, int y, EvasCanvas* canvas, const char* name = 0 );
  EvasEsmartTextEntry( int x, int y, int width, int height, EvasCanvas* canvas, const char* name = 0 );
  virtual ~EvasEsmartTextEntry();

  void setText (const char *str);
  
  void isPassword (bool val);

  void setMaxChars (int max);

  void setEdjePart (EvasEdje *edje, const char *part);

  Evas_Object *getEdjeObject ();

  const char *getEdjePart ();

  /* you've gotta free this resut */
  const char *getText ();

  /*EAPI void esmart_text_entry_return_key_callback_set (Evas_Object * o,
                                                     void (*func) (void *data,
                                                                   const char *str),
                                                     void *data);*/


protected:
  // smart object handlers
  virtual void addHandler() {};
  virtual void delHandler() {};
  virtual void moveHandler( Evas_Coord x, Evas_Coord y ) {};
  virtual void resizeHandler( Evas_Coord w, Evas_Coord h ) {};
  virtual void showHandler() {};
  virtual void hideHandler() {};
  virtual void colorSetHandler( int r, int g, int b, int a ) {};
  virtual void clipSetHandler( Evas_Object *clip ) {};
  virtual void clipUnsetHandler() {};
};

} // end namespace efl

#endif // EFLPP_ESMART_TEXTENTRY_H
