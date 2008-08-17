#include <string.h>
#include <unistd.h>
#include <Ecore_Config.h>
#include "../daemon/Entranced.h"
#include "Entrance_Edit.h"

#define _CONF_INIT_DOMAIN "entrance"
#define _CONF_FILENAME "entrance_config.cfg"



struct _Entranced_Edit {
	char *config_file;
};

typedef struct _Entranced_Edit Entranced_Edit;

static Entranced_Edit *_entrance_edit;

static int _entrance_edit_new(void);
static void _entrance_edit_free(void);
static void _entrance_edit_defaults_set(void);

EAPI int 
entrance_edit_init(const char *filename)
{
	int status;

	ecore_init();
	if (ecore_config_init(_CONF_INIT_DOMAIN) != ECORE_CONFIG_ERR_SUCC) 
	{
		ecore_shutdown();
		return 0;
	}


	_entrance_edit_defaults_set();

	if (!_entrance_edit_new()) 
	{
		ecore_shutdown();
		return 0;
	}

	if (filename) 
	{
		_entrance_edit->config_file = strdup(filename);
	} 
	else 
	{
		_entrance_edit->config_file = strdup(PACKAGE_CFG_DIR "/" _CONF_FILENAME);
	}

	status = ecore_config_file_load(_entrance_edit->config_file);

	if (status != ECORE_CONFIG_ERR_SUCC) 
	{
		return 0;
	}

	return 1;
}


EAPI int
entrance_edit_shutdown(void)
{
	_entrance_edit_free();
	ecore_config_shutdown();
	ecore_shutdown();

	/*TODO: Properly fix error codes */
	return 1;
}

EAPI int
entrance_edit_save(void)
{
	if (_entrance_edit) 
	{
		if (ecore_config_file_save(_entrance_edit->config_file) != ECORE_CONFIG_ERR_SUCC) 
		{
		   return 0;
		} 
		else 
		{
		   return 1;
		}
	}

	return 0;
}

EAPI void 
entrance_edit_list(void)
{
	/*TODO:eet_list anyone?*/
	printf("Entrance Daemon Settings\n");
	printf("  attempts = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT));
	printf("  xserver = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR));
	printf("Entrance Client Settings\n");
	printf("  xsession = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR));
	printf("  auth = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT));
	printf("  engine = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT));
	printf("  reboot = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT)); 
	printf("  halt = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT));
	printf("  theme = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_THEME_STR));
	printf("  background = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR));
	printf("  pointer = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_POINTER_STR));
	printf("  greeting-before = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR));
	printf("  greeting-after = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR));
	printf("  date-format = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_DATE_FORMAT_STR));
	printf("  time-format = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR));
	printf("  autologin-mode = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT));
	printf("  autologin-user = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR));
	printf("  presel-mode = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT));
	printf("  presel-prevuser = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR));
	printf("  user-remember = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT));
	printf("  user-remember-count = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT));
	printf("  user-count = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT));
	printf("  session-count = %d\n", entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_SESSION_COUNT_INT));
	printf("  default-session = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_SESSION_STR));
	printf("  default-session-title = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_TITLE_STR));
	printf("  default-session-icon = \"%s\"\n", entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_ICON_STR));
}

/*TODO: instead of bleh_TYPE_get/set, etc
 * since we know the ENTRANCE_EDIT_KEY_TYPE_foobar
 * contants, we can use a string search on the keys
 * and just have entrance_edit_value_set/get that
 * internally calls the right _type_get/set
 * this makes the api even more compact.
 * ALAS!!! There is no function overloading in C :(
 *
 * It may be neater to hide the data type specifics
 * behind functions like:
 * entrance_edit_auth_set/get, entrance_edit_theme_set/get
 * */

EAPI int 
entrance_edit_int_get(const char *key)
{
	return ecore_config_int_get(key);
}

EAPI int 
entrance_edit_int_set(const char *key, int val)
{
	return ecore_config_int_set(key, val);
}

EAPI char * 
entrance_edit_string_get(const char *key)
{
	return ecore_config_string_get(key);
}

EAPI int 
entrance_edit_string_set(const char *key, const char *val)
{
	return ecore_config_string_set(key, val);
}

/*private parts - oops!!!*/

static int 
_entrance_edit_new(void)
{
	_entrance_edit = calloc(1, sizeof(Entranced_Edit));
	if (!_entrance_edit) 
	{
		return 0;
	} 

	return 1;
}

static void 
_entrance_edit_free(void)
{
	if (_entrance_edit) 
	{
		if (_entrance_edit->config_file) 
		{
			free(_entrance_edit->config_file);
		}

		free(_entrance_edit);
	}
}


static void
_entrance_edit_defaults_set(void)
{
	ecore_config_string_default(ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR, DEFAULT_X_SERVER);
	ecore_config_int_default(ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT, 5);

	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR, ENTRANCE_XSESSION);
	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT, 1);
	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT, 0);
	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT, 1);
	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT, 1);

	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_THEME_STR, "default.edj");
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR, "");
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_POINTER_STR, PACKAGE_DATA_DIR "/images/pointer.png");
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR, "Welcome to");
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR, "");
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_DATE_FORMAT_STR, "%x");
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR, "%X");

	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT, 0);
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR, "");

	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT, 1);
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR, "");

	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT, 1);
	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT, 5);
	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT, 0);

	ecore_config_int_default(ENTRANCE_EDIT_KEY_CLIENT_SESSION_COUNT_INT, 1);
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_SESSION_STR, "default");
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_TITLE_STR, "Default");
	ecore_config_string_default(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_ICON_STR, "default.png");
}
