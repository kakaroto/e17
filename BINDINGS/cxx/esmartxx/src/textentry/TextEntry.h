#ifndef ESMARTXX_TEXTENTRY_H
#define ESMARTXX_TEXTENTRY_H

#include <evasxx/Smart.h>
#include <edjexx/Object.h>
#include <edjexx/Base.h>

/* EFL */
#include <Esmart/Esmart_Text_Entry.h>

namespace Esmartxx {

class TextEntry : public Evasxx::Smart
{
public:
  TextEntry( Evasxx::Canvas &canvas );
  TextEntry( Evasxx::Canvas &canvas, const Eflxx::Point &pos );
  TextEntry( Evasxx::Canvas &canvas, const Eflxx::Rect &rect );
  virtual ~TextEntry();

  void setText (const std::string &str);

  void isPassword (bool val);

  void setMaxChars (int max);

  void setEdjePart (Edjexx::Object *edje, const char *part);

  Evas_Object *getEdjeObject ();

  const string getEdjePart ();

  /* you've gotta free this resut */
  const string getText ();

  /*EAPI void esmart_text_entry_return_key_callback_set (Evas_Object * o,
                                                     void (*func) (void *data,
                                                                   const char *str),
                                                     void *data);*/

};

} // end namespace Esmartxx

#endif // ESMARTXX_TEXTENTRY_H
