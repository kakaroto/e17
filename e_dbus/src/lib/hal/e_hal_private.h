#ifndef E_HAL_PRIVATE_H
#define E_HAL_PRIVATE_H

#ifndef E_DBUS_COLOR_DEFAULT
#define E_DBUS_COLOR_DEFAULT EINA_COLOR_CYAN
#endif
extern int _e_dbus_hal_log_dom;
#ifdef ERR
#undef ERR
#endif
#ifdef INF
#undef INF
#endif
#ifdef WARN
#undef WARN
#endif
#ifdef DBG
#undef DBG
#endif

#define DBG(...)   EINA_LOG_DOM_DBG(_e_dbus_hal_log_dom, __VA_ARGS__)
#define INFO(...)    EINA_LOG_DOM_INFO(_e_dbus_hal_log_dom, __VA_ARGS__)
#define WARN(...) EINA_LOG_DOM_WARN(_e_dbus_hal_log_dom, __VA_ARGS__)
#define ERR(...)   EINA_LOG_DOM_ERR(_e_dbus_hal_log_dom, __VA_ARGS__)

#endif
