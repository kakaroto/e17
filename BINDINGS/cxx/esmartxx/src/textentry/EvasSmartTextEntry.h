#ifndef EFLPP_ESMART_TEXTENTRY_H
#define EFLPP_ESMART_TEXTENTRY_H

#include <evasxx/EvasSmart.h>
#include <edjexx/EdjeObject.h>
#include <edjexx/EdjeBase.h>

/* EFL */
#include <Esmart/Esmart_Text_Entry.h>

namespace efl {

class EvasSmartTextEntry : public EvasSmart
{
public:
  EvasSmartTextEntry( EvasCanvas &canvas );
  EvasSmartTextEntry( EvasCanvas &canvas, const Point &pos );
  EvasSmartTextEntry( EvasCanvas &canvas, const Rect &rect );
  virtual ~EvasSmartTextEntry();

  void setText (const std::string &str);

  void isPassword (bool val);

  void setMaxChars (int max);

  void setEdjePart (EdjeObject *edje, const char *part);

  Evas_Object *getEdjeObject ();

  const string getEdjePart ();

  /* you've gotta free this resut */
  const string getText ();

  /*EAPI void esmart_text_entry_return_key_callback_set (Evas_Object * o,
                                                     void (*func) (void *data,
                                                                   const char *str),
                                                     void *data);*/

};

} // end namespace efl

#endif // EFLPP_ESMART_TEXTENTRY_H
