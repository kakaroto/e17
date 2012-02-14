#ifndef ELEV8_HTTP_H
#define ELEV8_HTTP_H

#include <Eina.h>
#include <Ecore.h>
#include <Elementary.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <v8.h>
#include <module.h>

extern int elev8_elm_log_domain;
#define ELM_DBG(...) EINA_LOG_DOM_DBG(elev8_elm_log_domain, __VA_ARGS__)
#define ELM_INF(...) EINA_LOG_DOM_INFO(elev8_elm_log_domain, __VA_ARGS__)
#define ELM_WRN(...) EINA_LOG_DOM_WARN(elev8_elm_log_domain, __VA_ARGS__)
#define ELM_ERR(...) EINA_LOG_DOM_ERR(elev8_elm_log_domain, __VA_ARGS__)
#define ELM_CRT(...) EINA_LOG_DOM_CRITICAL(elev8_elm_log_domain, __VA_ARGS__)

#endif
