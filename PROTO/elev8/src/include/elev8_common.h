#ifndef ELEV8_COMMON_H
#define ELEV8_COMMON_H

#include <v8.h>
#include <Eina.h>
#include <assert.h>

extern int elev8_log_domain;

#define DBG(...) EINA_LOG_DOM_DBG(elev8_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(elev8_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(elev8_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(elev8_log_domain, __VA_ARGS__)
#define CRT(...) EINA_LOG_DOM_CRITICAL(elev8_log_domain, __VA_ARGS__)

#endif
