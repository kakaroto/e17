/**
@brief Exquisite Splash screens Library

These routines are used for Edje.

@mainpage Exquisite Library Documentation
@version 1.6.0
@date 2011-2012

Please see the @ref authors page for contact details.


@section intro What is Exquisite?

Exquisite is a small convenience library that helps build
splash screens for your applications, providing a progress
bar as well as the ability to show messages to the user.
Exquisite uses Edje and is themeable using Edje files.

Exquisite is also a set of tools that can be used for showing
a proper Linux boot splash screen.

@section requirements What does Exquisite require?

The Exquisite library only requires Edje and its dependencies.
To use the Exquisite runtime library, you will need:

  - Evas (library)
  - Eina (library)

An additional dependency is used by the exquisite and exquisite-writer
tools :
  - Ecore (library)


*/

#ifndef _EXQUISITE_H
#define _EXQUISITE_H

#include <Evas.h>
#include <Edje.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EXQUISITE_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EINA_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
/**
 * @def EAPI
 * @brief Used to export functions(by changing visibility).
 */
#  define EAPI
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @typedef Exquisite_Exit_Callback
 * Type for a callback to be called when the theme has finished its
 * exit animation.
 * @param data The data passed to exquisite_object_exit_callback_set()
 */
typedef void (*Exquisite_Exit_Callback) (void *data);

/**
 * @enum _Exquisite_Status_Type
 * The type of messages to show to the user
 */
typedef enum _Exquisite_Status_Type {
  EXQUISITE_STATUS_TYPE_NORMAL = -1,
  EXQUISITE_STATUS_TYPE_SUCCESS = 1,
  EXQUISITE_STATUS_TYPE_FAILURE = 0,
} Exquisite_Status_Type;

/**
 * @brief Instantiate a new Exquisite object
 *
 * @param evas A valid Evas handle, the canvas to place the new object
 * in
 * @param theme The path to the theme file to use for the Exquisite object
 *
 * @return A handle to the new object created or @c NULL, on errors.
 *
 * This function creates a new Exquisite smart object, returning its @c
 * Evas_Object handle.
 */
EAPI Evas_Object *exquisite_object_add(Evas *evas, const char *theme);

/**
 * @brief Set the exit callback function
 *
 * @param obj A handle to an Exquisite object
 * @param func The function to call
 * @param data User data to pass to the callback
 *
 * This set the callback function to be called when the exit animation is
 * finished.
 *
 * @see exquisite_object_exit()
 *
 */
EAPI void exquisite_object_exit_callback_set(Evas_Object *obj,
    Exquisite_Exit_Callback func, void *data);

/**
 * @brief Start the exit animation of the theme
 *
 * @param obj A handle to an Exquisite object
 *
 * This will start the exit animation of the theme and the Exquisite object
 * will become unusable after that. You should set the exit callback then
 * destroy the object when it's done.
 *
 * @see exquisite_object_exit_callback_set()
 *
 */
EAPI void exquisite_object_exit(Evas_Object *obj);

/**
 * @brief Sets the title of the screen
 *
 * @param obj A handle to an Exquisite object
 * @param txt The title of the screen
 *
 * This will set the title of the screen of the Exquisite object
 *
 */
EAPI void exquisite_object_title_set(Evas_Object *obj, const char *txt);

/**
 * @brief Sets the message of the screen
 *
 * @param obj A handle to an Exquisite object
 * @param txt The message to show on the screen
 *
 * This will set the message being shown on the Exquisite screen
 *
 */
EAPI void exquisite_object_message_set(Evas_Object *obj, const char *txt);

/**
 * @brief Sets the position of the progress bar
 *
 * @param obj A handle to an Exquisite object
 * @param val A fraction between 0.0 and 1.0 that represents the position
 *            of the progress bar
 *
 * This will move the progress bar of the screen to the specified position
 *
 */
EAPI void exquisite_object_progress_set(Evas_Object *obj, double val);

/**
 * @brief Generate a heartbeat tick
 *
 * @param obj A handle to an Exquisite object
 *
 * This will make the screen tick to show to the user that the application
 * is not frozen
 *
 */
EAPI void exquisite_object_tick(Evas_Object *obj);

/**
 * @brief Pulsate the progress bar
 *
 * @param obj A handle to an Exquisite object
 *
 * Pulsates the progress bar to show an action that has no defined progress
 *
 */
EAPI void exquisite_object_pulsate(Evas_Object *obj);

/**
 * @brief Adds a message to the text box
 *
 * @param obj A handle to an Exquisite object
 * @param txt The message to show on the text box
 *
 * Appends a text message to the end of the text box of the screen.
 *
 * @see exquisite_object_status_set()
 *
 */
EAPI int exquisite_object_text_add(Evas_Object *obj, const char *txt);

/**
 * @brief Sets a status text on the last text box entry
 *
 * @param obj A handle to an Exquisite object
 * @param txt The status text
 * @param type The type of status to set
 *
 * Sets a status to the last text entry added into the text box. The status
 * has text associated to it which will have a different style depending on
 * the type of status set.
 *
 * @see exquisite_object_text_add()
 *
 */
EAPI void exquisite_object_status_set(Evas_Object *obj, int text,
      const char *status, Exquisite_Status_Type type);

/**
 * @brief Clears the text box
 *
 * @param obj A handle to an Exquisite object
 *
 * Clears all text in the text box from the screen
 *
 */
EAPI void exquisite_object_text_clear(Evas_Object *obj);

#ifdef __cplusplus
}
#endif

#endif
