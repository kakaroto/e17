#ifndef ENTRANCE_X_SESSION_H
#define ENTRANCE_X_SESSION_H
/**
 * @file entrance_user.h
 * @brief Struct Definitions and shared function declarations
 */
#include <Evas.h>

Evas_Object *entrance_x_session_xsession_load(Evas_Object * o,
                                              const char *themefile,
                                              const char *icon,
                                              const char *name);

#endif
