#ifndef ESPIONNAGE_PRIVATE_H
# define ESPIONNAGE_PRIVATE_H

#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Emotion.h>
#include <Elementary.h>

#ifdef __cplusplus
extern "C" {
#endif

#define stringify(X) #X

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...)      EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...)      EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...)      EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...)      EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

  extern int _log_domain;

  Eina_List *face_search(Ecore_Thread *thread, char *data, int width, int height, int stride);
#ifdef __cplusplus
};
#endif
#endif
