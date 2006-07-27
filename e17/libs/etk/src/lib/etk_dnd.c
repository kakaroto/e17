/** @file etk_dnd.c */
#include "etk_dnd.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>

#include "config.h"

#include "etk_engine.h"

/* Some notes for when we're developing this:
 * Signals:
 * - source:
 * drag_begin: drag has started
 * drag_end: drag has ended
 * 
 * - destination:
 * drag_leave: mouse leaves widget
 * drag_motion: mouse is moving on widget
 * drag_drop: object dropped on widget
 * 
 * selection_received: when our widget gets the selection it has asked for
 *                     (ie, we want to get a selection)
 * selection_get: when a request for a selection has been made to our widget
 *                (ie, someone is getting our selection from us)
 * selection_clear_event: the selection has been cleared
 * 
 * clipboard_received: when our widget gets the clipboard data it has asked for
 *                     (ie, we want to get a clipboard's text / image)
 * 
 */

/**************************
 *
 * Implementation
 *
 **************************/
 
/**
 * @brief Inits the drag and drop system
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure
 */
Etk_Bool etk_dnd_init()
{
   return etk_engine_dnd_init();   
}

/**
 * @brief Shutdowns the drag and drop system
 */
void etk_dnd_shutdown()
{
   etk_engine_dnd_shutdown();   
}

/** @} */
