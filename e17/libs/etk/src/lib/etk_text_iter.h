/** @file etk_text_iter.h */
#ifndef _ETK_TEXT_ITER_H_
#define _ETK_TEXT_ITER_H_

 #include "etk_object.h"
 #include "etk_types.h"
 
/**
 * @defgroup Etk_Text_Iter Etk_Text_Iter
 * @{
 */
 
/** @brief Gets the type of a text iterator */
#define ETK_TEXT_ITER_TYPE       (etk_text_iter_type_get())
/** @brief Casts the object to an Etk_Text_Iter */
#define ETK_TEXT_ITER(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXT_ITER_TYPE, Etk_Text_Iter))
/** @brief Checks if the object is an Etk_Text_Iter */
#define ETK_IS_TEXT_ITER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXT_ITER_TYPE))

/**
 * @struct Etk_Text_Iter
 * @brief A text iterator is an object that allow you to navigate through a text buffer. It's also needed t insert @n
 * some text at a specific place in a text buffer, to remove some text, ...
 */
struct _Etk_Text_Iter
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;
   
   int formatted_pos;
   int unformatted_pos;
   Etk_Text_Buffer *text_buffer;
};

Etk_Type *etk_text_iter_type_get();
Etk_Text_Iter *etk_text_iter_new(Etk_Text_Buffer *text_buffer);

#endif
