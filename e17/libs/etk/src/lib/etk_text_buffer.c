#include "etk_text_buffer.h"
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_string.h"
#include "etk_text_iter.h"

enum _Etk_Text_Buffer_Signal_Id
{
   ETK_TEXT_BUFFER_TEXT_CHANGED_SIGNAL,
   ETK_TEXT_BUFFER_NUM_SIGNALS
};

static void _etk_text_buffer_constructor(Etk_Text_Buffer *text_buffer);
static void _etk_text_buffer_destructor(Etk_Text_Buffer *text_buffer);
static void _etk_text_buffer_formatted_buffer_changed_cb(Etk_Object *object, const char *property_name, void *data);

static Etk_Signal *_etk_text_buffer_signals[ETK_TEXT_BUFFER_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Text_Buffer
 * @return Returns the type on an Etk_Text_Buffer
 */
Etk_Type *etk_text_buffer_type_get()
{
   static Etk_Type *text_buffer_type = NULL;

   if (!text_buffer_type)
   {
      text_buffer_type = etk_type_new("Etk_Text_Buffer", ETK_OBJECT_TYPE, sizeof(Etk_Text_Buffer),
         ETK_CONSTRUCTOR(_etk_text_buffer_constructor), ETK_DESTRUCTOR(_etk_text_buffer_destructor));
      
      _etk_text_buffer_signals[ETK_TEXT_BUFFER_TEXT_CHANGED_SIGNAL] = etk_signal_new("text_changed", text_buffer_type, -1,
         etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return text_buffer_type;
}

/**
 * @brief Creates a new text buffer
 * @return Returns the new text buffer
 */
Etk_Text_Buffer *etk_text_buffer_new()
{
   return ETK_TEXT_BUFFER(etk_object_new(ETK_TEXT_BUFFER_TYPE, NULL));
}

/**
 * @brief Inserts @a text in the text buffer
 * @param text_buffer a text buffer
 * @param iter the iterator where we should insert the text
 * @param text the text to insert
 * @param length the length of the text to insert. -1 if all the text should be inserted
 */
void etk_text_buffer_insert(Etk_Text_Buffer *text_buffer, Etk_Text_Iter *iter, const char *text, int length)
{
   int text_length;
   int formatted_pos;
   int unformatted_pos;
   Evas_List *l;
   Etk_Text_Iter *i;
   
   if (!text_buffer || !iter || iter->text_buffer != text_buffer)
      return;
   if (!text || *text == 0 || length == 0)
      return;
   
   text_length = strlen(text);
   if (length < 0 || length > text_length)
      length = text_length;
   
   formatted_pos = iter->formatted_pos;
   unformatted_pos = iter->unformatted_pos;
   etk_string_insert_sized(text_buffer->formatted_buffer, formatted_pos, text, length);
   etk_string_insert_sized(text_buffer->unformatted_buffer, unformatted_pos, text, length);
   
   for (l = text_buffer->iterators; l; l = l->next)
   {
      i = l->data;
      if (i->formatted_pos >= formatted_pos)
         i->formatted_pos += length;
      if (i->unformatted_pos >= unformatted_pos)
         i->unformatted_pos += length;
   }
}

/**
 * @brief Inserts @a text in the text buffer at the cursor position
 * @param text_buffer a text buffer
 * @param text the text to insert
 * @param length the length of the text to insert. -1 if all the text should be inserted
 */
void etk_text_buffer_insert_at_cursor(Etk_Text_Buffer *text_buffer, const char *text, int length)
{
   if (!text_buffer)
      return;
   etk_text_buffer_insert(text_buffer, text_buffer->cursor, text, length);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the text buffer */
static void _etk_text_buffer_constructor(Etk_Text_Buffer *text_buffer)
{
   if (!text_buffer)
      return;
   
   text_buffer->formatted_buffer = etk_string_new(NULL);
   text_buffer->unformatted_buffer = etk_string_new(NULL);
   text_buffer->iterators = NULL;
   text_buffer->cursor = etk_text_iter_new(text_buffer);
   
   etk_object_notification_callback_add(ETK_OBJECT(text_buffer->formatted_buffer), "string", _etk_text_buffer_formatted_buffer_changed_cb, text_buffer);
}

/* Destroys the text buffer */
static void _etk_text_buffer_destructor(Etk_Text_Buffer *text_buffer)
{
   if (!text_buffer)
      return;
   
   etk_object_destroy(ETK_OBJECT(text_buffer->formatted_buffer));
   etk_object_destroy(ETK_OBJECT(text_buffer->unformatted_buffer));
   
   while (text_buffer->iterators)
      etk_object_destroy(ETK_OBJECT(text_buffer->iterators->data));
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the formatted buffer is changed */
static void _etk_text_buffer_formatted_buffer_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   etk_signal_emit(_etk_text_buffer_signals[ETK_TEXT_BUFFER_TEXT_CHANGED_SIGNAL], ETK_OBJECT(data), NULL);
}
