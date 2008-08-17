#ifndef ENTRANCE_EDIT_H
#define ENTRANCE_EDIT_H

#define ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT "/entranced/attempts"
#define ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR "/entranced/xserver"

#define ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR "/entrance/xsession"
#define ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT "/entrance/auth"
#define ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT "/entrance/engine"
#define ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT "/entrance/system/reboot"
#define ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT "/entrance/system/halt"

#define ENTRANCE_EDIT_KEY_CLIENT_THEME_STR "/entrance/theme"
#define ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR "/entrance/background"
#define ENTRANCE_EDIT_KEY_CLIENT_POINTER_STR "/entrance/pointer"
#define ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR "/entrance/greeting/before"
#define ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR "/entrance/greeting/after"
#define ENTRANCE_EDIT_KEY_CLIENT_DATE_FORMAT_STR "/entrance/date_format"
#define ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR "/entrance/time_format"

#define ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT	"/entrance/autologin/mode"
#define ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR "/entrance/autologin/user"
#define ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT "/entrance/presel/mode" /*presel == preselect?*/
#define ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR "/entrance/presel/prevuser"

#define ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT "/entrance/user/remember"
#define ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT "/entrance/user/remember_n"
#define ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT "/entrance/user/count"

#define ENTRANCE_EDIT_KEY_CLIENT_SESSION_COUNT_INT "/entrance/session/count"
#define ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_SESSION_STR "/entrance/session/0/session"
#define ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_TITLE_STR "/entrance/session/0/title"
#define ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_ICON_STR "/entrance/session/0/icon"

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

EAPI int entrance_edit_init(const char *filename);
EAPI int entrance_edit_shutdown(void);
EAPI int entrance_edit_save(void);

EAPI void entrance_edit_list(void);

EAPI int entrance_edit_int_get(const char *key);
EAPI int entrance_edit_int_set(const char *key, int val);

EAPI char *entrance_edit_string_get(const char *key);
EAPI int   entrance_edit_string_set(const char *key, const char *val);

#endif
