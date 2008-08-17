#ifndef ENTRANCE_X_SESSION_H
#define ENTRANCE_X_SESSION_H
/**
 * @file entrance_x_session.h
 * @brief Struct Definitions and shared function declarations
 */
#include <Evas.h>

typedef struct _Entrance_X_Session Entrance_X_Session;

/**
 * @brief This contains all of the information we keep about X Sessions
 */
struct _Entrance_X_Session
{
   char *name;
   char *icon;
   char *session;
   Evas_Object *edje;
};

Entrance_X_Session *entrance_x_session_new(char *name, char *icon,
                                           char *session);
void entrance_x_session_free(Entrance_X_Session * e);

Evas_Object *entrance_x_session_edje_get(Entrance_X_Session * e,
                                         Evas_Object * o,
                                         const char *themefile);
Evas_Object *entrance_x_session_button_new(Entrance_X_Session *, Evas_Object *);

#endif
