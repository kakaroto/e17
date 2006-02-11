/** @file etk_text_buffer.h */
#ifndef _ETK_TEXT_BUFFER_H_
#define _ETK_TEXT_BUFFER_H_

 #include "etk_object.h"
 #include <Evas.h>
 #include "etk_types.h"
 
/**
 * @defgroup Etk_Text_Buffer Etk_Text_Buffer
 * @{
 */
 
/** @brief Gets the type of a text buffer */
#define ETK_TEXT_BUFFER_TYPE       (etk_text_buffer_type_get())
/** @brief Casts the object to an Etk_Text_Buffer */
#define ETK_TEXT_BUFFER(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXT_BUFFER_TYPE, Etk_Text_Buffer))
/** @brief Checks if the object is an Etk_Text_Buffer */
#define ETK_IS_TEXT_BUFFER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXT_BUFFER_TYPE))

/**
 * @struct Etk_Text_Buffer
 * @brief A text buffer is an object that manipulates a text buffer, that can next be displayed in an entry, a text view, ...
 */
struct _Etk_Text_Buffer
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;
   
   Etk_String *formatted_buffer;
   Etk_String *unformatted_buffer;
   
   Etk_Text_Iter *cursor;
   Evas_List *iterators;
};

Etk_Type *etk_text_buffer_type_get();
Etk_Text_Buffer *etk_text_buffer_new();

void etk_text_buffer_insert(Etk_Text_Buffer *text_buffer, Etk_Text_Iter *iter, const char *text, int length);
void etk_text_buffer_insert_at_cursor(Etk_Text_Buffer *text_buffer, const char *text, int length);



#endif
