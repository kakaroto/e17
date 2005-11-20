/** @file etk_statusbar.h */
#ifndef _ETK_STATUSBAR_H_
#define _ETK_STATUSBAR_H_

#include <Evas.h>
#include "etk_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Statusbar Etk_Statusbar
 * @{
 */

/** @brief Gets the type of a status bar */
#define ETK_STATUSBAR_TYPE       (etk_statusbar_type_get())
/** @brief Casts the object to an Etk_Statusbar */
#define ETK_STATUSBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_STATUSBAR_TYPE, Etk_Statusbar))
/** @brief Checks if the object is an Etk_Statusbar */
#define ETK_IS_STATUSBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_STATUSBAR_TYPE))

/**
 * @struct Etk_Statusbar
 * @brief TODO
 */
struct _Etk_Statusbar
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Etk_Bool has_resize_grip;
   Evas_List *msg_stack;
   int next_message_id;
   int next_context_id;
};

Etk_Type *etk_statusbar_type_get();
Etk_Widget *etk_statusbar_new();

int etk_statusbar_context_id_get(Etk_Statusbar *statusbar, const char *context);

int etk_statusbar_push(Etk_Statusbar *statusbar, const char *message, int context_id);
void etk_statusbar_pop(Etk_Statusbar *statusbar, int context_id);
void etk_statusbar_remove(Etk_Statusbar *statusbar, int message_id);

/** @} */

#endif
