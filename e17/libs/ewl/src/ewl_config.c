
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif


static E_DB_File *user_config = NULL;
static E_DB_File *system_config = NULL;
static E_DB_File *config_db = NULL;

static int create_user_config(void);
static int open_user_config(void);
static int open_system_config(void);


int
ewl_config_init(void)
{
	if ((open_user_config()) != -1)
	  {
		  config_db = user_config;
		  return 1;
	  }
	else if ((open_system_config()) != -1)
	  {
		  config_db = system_config;
		  return 1;
	  }
	else
	  {
		  if ((create_user_config()) != -1)
		    {
			    if ((open_user_config()) != -1)
			      {
				      config_db = user_config;
				      return 1;
			      }
			    else
			      {
				      DERROR("Failed to open user config, I was able" "to create it tough.");
				      return -1;
			      }
		    }
		  else
		    {
			    DERROR("Failed to create user config.");
			    return -1;
		    }
	  }
}

static int
create_user_config(void)
{
	char *home;
	char path[256];

	home = getenv("HOME");

	if (!home)
	  {
		  DWARNING("Failed to fetch environment variable HOME\n");
		  return -1;
	  }

	snprintf(path, 256, "%s/.e/ewl/config/system.db", home);

	user_config = e_db_open(path);

	if (!user_config)
		DWARNING("Failed to open user config, fix permissions\n");

	ewl_config_set_str("/evas/render_method", "software");
	ewl_config_set_float("/fx/max_fps", 25.0);
	ewl_config_set_float("/fx/timeout", 2.0);
	ewl_config_set_str("/evas/render_method", "default");

	return 1;
}

static int
open_user_config(void)
{
	char *home;
	char path[256];

	home = getenv("HOME");

	if (!home)
	  {
		  DWARNING("Failed to fetch environment variable HOME\n");
		  return -1;
	  }

	snprintf(path, 256, "%s/.e/ewl/config/system.db", home);

	user_config = e_db_open(path);

	if (user_config)
		return 1;

	return -1;
}

static int
open_system_config(void)
{
	char path[256];

	snprintf(path, 256, PACKAGE_DATA_DIR "/config/system.db");

	system_config = e_db_open_read(path);

	if (system_config)
		return 1;

	return -1;
}

int
ewl_config_set_str(const char *k, const char *v)
{
	int kl, vl;

	kl = strlen(k);
	vl = strlen(v);

	{
		char k2[kl], v2[kl];

		snprintf(k2, kl, "%s", k);
		snprintf(v2, vl, "%s", v);

		if (!user_config)
		  {
			  if ((create_user_config()) != -1)
				  open_user_config();
		  }

		if (user_config)
		  {
			  e_db_str_set(user_config, k2, v2);
			  e_db_flush();
		  }
		else
		  {
			  DWARNING("Somehow i can't write to the user config, check permissions.");
			  return -1;
		  }
	}

	return 1;
}


int
ewl_config_set_int(const char *k, const int v)
{
	int kl, v2;

	kl = strlen(k);

	{
		char k2[kl];

		snprintf(k2, kl, "%s", k);
		v2 = v;

		if (!user_config)
		  {
			  if ((create_user_config()) != -1)
				  open_user_config();
		  }

		if (user_config)
		  {
			  e_db_int_set(user_config, k2, v2);
			  e_db_flush();
		  }
		else
		  {
			  DWARNING("Somehow i can't write to the user config, check permissions.");
			  return -1;
		  }
	}

	return 1;
}

int
ewl_config_set_float(const char *k, const float v)
{
	int kl;
	float v2;

	kl = strlen(k);

	{
		char k2[kl];

		snprintf(k2, kl, "%s", k);
		v2 = v;

		if (!user_config)
		  {
			  if ((create_user_config()) != -1)
				  open_user_config();
		  }

		if (user_config)
		  {
			  e_db_float_set(user_config, k2, v2);
			  e_db_flush();
		  }
		else
		  {
			  DWARNING("Somehow i can't write to the user config, check permissions.");
			  return -1;
		  }
	}

	return 1;
}

char *
ewl_config_get_str(const char *k)
{
	int kl;

	kl = strlen(k);

	{
		char k2[kl];
		snprintf(k2, kl, "%s", k);

		if (config_db)
			return e_db_str_get(config_db, k2);
		else
			return NULL;
	}
}

int
ewl_config_get_int(const char *k, int *v)
{
	int kl;

	kl = strlen(k);

	{
		char k2[kl];

		snprintf(k2, kl, "%s", k);

		if (config_db)
			return e_db_int_get(config_db, k2, v);
		else
			return -1;
	}
}

int
ewl_config_get_float(const char *k, float *v)
{
	int kl;

	kl = strlen(k);

	{
		char k2[kl];

		snprintf(k2, kl, "%s", k);

		if (config_db)
			return e_db_float_get(config_db, k2, v);
		else
			return -1;
	}
}

Evas_Render_Method
ewl_config_get_render_method()
{
	char *str = NULL;

	str = ewl_config_get_str("/evas/render_method");

	if (str)
	  {
		  if (!strcasecmp(str, "software"))
			  return RENDER_METHOD_ALPHA_SOFTWARE;
		  else if (!strcasecmp(str, "hardware"))
			  return RENDER_METHOD_3D_HARDWARE;
		  else if (!strcasecmp(str, "x11"))
			  return RENDER_METHOD_BASIC_HARDWARE;

		  FREE(str);
	  }

	return RENDER_METHOD_ALPHA_SOFTWARE;
}
