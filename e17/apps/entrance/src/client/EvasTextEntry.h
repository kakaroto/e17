#ifndef EVAS_TEXT_ENTRY_H
#define EVAS_TEXT_ENTRY_H
#include <Evas.h>
#include <Edje.h>
#include <Ecore.h>
#include <limits.h>

struct _Evas_Text_Entry
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
typedef struct _Evas_Text_Entry Evas_Text_Entry;

/* create a new text entry */
Evas_Object *evas_text_entry_new(Evas * e);

void evas_text_entry_text_set(Evas_Object * o, const char *str);
void evas_text_entry_is_password_set(Evas_Object * o, int val);
void evas_text_entry_focus_set(Evas_Object * o, int val);
int evas_text_entry_is_focused(Evas_Object * o);
void evas_text_entry_max_chars_set(Evas_Object * o, int max);
void evas_text_entry_edje_part_set(Evas_Object * o, Evas_Object * edje,
                                   char *part);


/* you've gotta free this resut */
//char * evas_text_entry_string_get(Evas_Object *o)

void evas_text_entry_return_key_callback_set(Evas_Object * o,
                                             void (*func) (void *data,
                                                           const char *str),
                                             void *data);

#endif
