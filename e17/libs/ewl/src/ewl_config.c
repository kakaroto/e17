
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif


static E_DB_File *config_db = NULL;

static int create_user_config(void);
static int open_user_config(void);
static int close_config(void);

extern Ewd_List *ewl_window_list;

int
ewl_config_init(void)
{
	memset(&ewl_config, 0, sizeof(Ewl_Config));

	if ((open_user_config()) != -1)
	  {
		  close_config();
		  return 1;
	  }
	else
		create_user_config();

	return 1;
}

static int
create_user_config(void)
{
	char *home;
	char pe[256];

	home = getenv("HOME");

	if (!home)
	  {
		  DWARNING("Failed to fetch environment variable HOME\n");
		  return -1;
	  }

	snprintf(pe, 1024, "%s/.e", home);
	mkdir(pe, 0755);
	snprintf(pe, 1024, "%s/.e/ewl", home);
	mkdir(pe, 0755);
	snprintf(pe, 1024, "%s/.e/ewl/config", home);
	mkdir(pe, 0755);

	ewl_config_set_int("/debug/enable", 0);
	ewl_config_set_int("/debug/level", 0);
	ewl_config_set_str("/evas/render_method", "software");
	ewl_config_set_int("/evas/font_cache", 2097152);
	ewl_config_set_int("/evas/image_cache", 8388608);
	ewl_config_set_float("/fx/max_fps", 25.0);
	ewl_config_set_float("/fx/timeout", 2.0);
	ewl_config_set_str("/theme/name", "default");

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

	config_db = e_db_open(path);

	if (config_db)
		return 1;

	return -1;
}

static int
close_config(void)
{
	if (config_db)
		e_db_close(config_db);

	e_db_flush();

	return 1;
}

int
ewl_config_set_str(char *k, char *v)
{
	open_user_config();

	e_db_str_set(config_db, k, v);

	close_config();

	return 1;
}


int
ewl_config_set_int(char *k, int v)
{
	open_user_config();

	e_db_int_set(config_db, k, v);

	close_config();

	return 1;
}

int
ewl_config_set_float(char *k, float v)
{
	open_user_config();

	e_db_float_set(config_db, k, v);

	close_config();

	return 1;
}

char *
ewl_config_get_str(char *k)
{
	char *ret = NULL;

	if ((open_user_config()) != -1)
	  {
		  ret = e_db_str_get(config_db, k);

		  close_config();
	  }

	return ret;
}

int
ewl_config_get_int(char *k, int *v)
{
	int ret = -1;

	if ((open_user_config()) != -1)
	  {
		  ret = e_db_int_get(config_db, k, v);

		  close_config();
	  }

	return ret;
}

int
ewl_config_get_float(char *k, float *v)
{
	int ret = -1;

	if ((open_user_config()) != -1)
	  {
		  ret = e_db_float_get(config_db, k, v);

		  close_config();
	  }

	return ret;
}

Evas_Render_Method
ewl_config_get_render_method()
{
	Evas_Render_Method method = RENDER_METHOD_ALPHA_SOFTWARE;
	char *str = NULL;

	str = ewl_config_get_str("/evas/render_method");

	if (str)
	  {
		  if (!strncasecmp(str, "software", 8))
			  method = RENDER_METHOD_ALPHA_SOFTWARE;
		  else if (!strncasecmp(str, "hardware", 8))
			  method = RENDER_METHOD_3D_HARDWARE;
		  else if (!strncasecmp(str, "x11", 3))
			  method = RENDER_METHOD_BASIC_HARDWARE;

		  FREE(str);
	  }

	return method;
}

void
ewl_config_reread_and_apply(void)
{
	Ewl_Config nc;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_config_get_int("/debug/enable", &nc.debug.enable);
	ewl_config_get_int("/debug/level", &nc.debug.level);
	ewl_config_get_int("/evas/font_cache", &nc.evas.font_cache);
	ewl_config_get_int("/evas/image_cache", &nc.evas.image_cache);
	nc.evas.render_method = ewl_config_get_str("/evas/render_method");
	ewl_config_get_float("/fx/max_fps", &nc.fx.max_fps);
	ewl_config_get_float("/fx/timeout", &nc.fx.timeout);
	nc.theme.name = ewl_config_get_str("/theme/name");

	if (ewl_window_list && !ewd_list_is_empty(ewl_window_list))
	  {
		  Ewl_Widget *w;

		  ewd_list_goto_first(ewl_window_list);

		  while ((w = ewd_list_next(ewl_window_list)) != NULL)
		    {
			    if (!w->evas)
				    continue;

			    if (nc.evas.font_cache)
			      {
				      evas_flush_font_cache(w->evas);
				      evas_set_font_cache(w->evas,
							  nc.evas.font_cache);
			      }

			    if (nc.evas.image_cache)
			      {
				      evas_flush_image_cache(w->evas);
				      evas_set_image_cache(w->evas,
							   nc.evas.
							   image_cache);
			      }

			    evas_set_output_method(w->evas,
						   ewl_config_get_render_method
						   ());
		    }
	  }

	IF_FREE(ewl_config.evas.render_method);
	IF_FREE(ewl_config.theme.name);

	ewl_config.debug.enable = nc.debug.enable;
	ewl_config.debug.level = nc.debug.level;
	ewl_config.evas.font_cache = nc.evas.font_cache;
	ewl_config.evas.image_cache = nc.evas.image_cache;
	ewl_config.evas.render_method = nc.evas.render_method;
	ewl_config.fx.max_fps = nc.fx.max_fps;
	ewl_config.fx.timeout = nc.fx.timeout;
	ewl_config.theme.name = nc.theme.name;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
