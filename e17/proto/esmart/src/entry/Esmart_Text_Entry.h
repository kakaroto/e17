#ifndef ESMART_TEXT_ENTRY_H
#define ESMART_TEXT_ENTRY_H

#include <Evas.h>
#include <Edje.h>
#include <Ecore.h>
#include <limits.h>

struct _Esmart_Text_Entry
{
   Evas_Object *clip;
   Evas_Object *base;
   struct
   {
      int size;
      char *text;
      int index;
   } buf;
   struct
   {
      Evas_Object *o;
      char *part;
   } edje;
   struct
   {
      void (*func) (void *data, const char *str);
      void *arg;
   } return_key;
   int passwd;
};
typedef struct _Esmart_Text_Entry Esmart_Text_Entry;

/* create a new text entry */
Evas_Object *esmart_text_entry_new(Evas * e);

void esmart_text_entry_text_set(Evas_Object * o, const char *str);
void esmart_text_entry_is_password_set(Evas_Object * o, int val);
void esmart_text_entry_focus_set(Evas_Object * o, int val);
int esmart_text_entry_is_focused(Evas_Object * o);
void esmart_text_entry_max_chars_set(Evas_Object * o, int max);
void esmart_text_entry_edje_part_set(Evas_Object * o, Evas_Object * edje,
                                   char *part);


/* you've gotta free this resut */
//char * esmart_text_entry_string_get(Evas_Object *o)

void esmart_text_entry_return_key_callback_set(Evas_Object * o,
                                             void (*func) (void *data,
                                                           const char *str),
                                             void *data);

#endif
