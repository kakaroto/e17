#include "prefs.h"
#include <limits.h>
#include <Edb.h>
#include "util.h"
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

struct _Iconbar_Prefs {
    char *db;
    char *home;
    char *time_format;
    char *theme;
    Ecore_Evas *ee;
    Evas_List *fonts;
    Evas_List *icons;
    int x, y, w, h;
    int transparent;
    int raise;
    int borderless;
    int withdrawn;
    int sticky;
};
typedef struct _Iconbar_Prefs Iconbar_Prefs;

static Iconbar_Prefs *ibprefs = NULL;

static Iconbar_Prefs* _iconbar_prefs_new(void);
static void _iconbar_prefs_free(Iconbar_Prefs *ip);
static void iconbar_config_generate_original_db(char *file);

/* start/stop */
void
iconbar_config_init(void)
{
    char *str;
    int i = 0,count = 0;
    char buf[PATH_MAX];
    E_DB_File *db = NULL;

    if((ibprefs = _iconbar_prefs_new()))
    {
	snprintf(buf, PATH_MAX, "%s/.e/iconbar/config.db",
	getenv("HOME"));
	ibprefs->db = strdup(buf);
	if((db = e_db_open_read(ibprefs->db)) == NULL)
	    iconbar_config_generate_original_db(ibprefs->db);
	else
	    e_db_close(db);
	
	if((db = e_db_open_read(ibprefs->db)))
	{
	    if((str = e_db_str_get(db, "/iconbar/home")))
	    {
		if(ibprefs->home) free(ibprefs->home);
		ibprefs->home = str;
	    }
	    str = e_db_str_get(db, "/iconbar/theme");
	    iconbar_config_theme_set(str);
	    if(str) free(str);
	    if((str = e_db_str_get(db, "/iconbar/time_format")))
	    {
		if(ibprefs->time_format) free(ibprefs->time_format);
		ibprefs->time_format = str;
	    }
	    if(!e_db_int_get(db, "/iconbar/raise", &ibprefs->raise))
		ibprefs->raise = 0;
	    if(!e_db_int_get(db, "/iconbar/withdrawn", &ibprefs->withdrawn))
		ibprefs->withdrawn = 0;
	    if(!e_db_int_get(db, "/iconbar/sticky", &ibprefs->sticky))
		ibprefs->sticky = 1;
	    if(!e_db_int_get(db, "/iconbar/borderless", &ibprefs->borderless))
		ibprefs->borderless = 1;
	    if(!e_db_int_get(db, "/iconbar/x", &ibprefs->x))
		ibprefs->x = 0;
	    if(!e_db_int_get(db, "/iconbar/y", &ibprefs->y))
		ibprefs->y = 0;
	    if(!e_db_int_get(db, "/iconbar/w", &ibprefs->w))
		ibprefs->w = 80;
	    if(!e_db_int_get(db, "/iconbar/h", &ibprefs->h))
		ibprefs->h = 400;
	    if(e_db_int_get(db, "/iconbar/fonts/count", &count))
	    {
		for(i = 0; i < count; i++)
		{
		    snprintf(buf, PATH_MAX, "/iconbar/fonts/%d/path", i);
		    if((str = e_db_str_get(db, buf)))
			ibprefs->fonts = evas_list_append(ibprefs->fonts, str);
		}
	    }
	    if(e_db_int_get(db, "/iconbar/icons/count", &count))
	    {
		for(i = 0; i < count; i++)
		{
		    snprintf(buf, PATH_MAX, "/iconbar/icons/%d/path", i);
		    if((str = e_db_str_get(db, buf)))
			ibprefs->icons = evas_list_append(ibprefs->icons, str);
		}
	    }
	    e_db_close(db);
	}
    }
}
void
iconbar_config_free(void)
{
    int i = 0;
    char buf[PATH_MAX];
    Evas_List *l = NULL;
    E_DB_File *db = NULL;
    Ecore_Evas *ee = NULL;

    if(ibprefs && ibprefs->db)
    {
	ee = ibprefs->ee;
	if((db = e_db_open(ibprefs->db)))
	{
	    e_db_str_set(db, "/iconbar/home", ibprefs->home);
	    e_db_str_set(db, "/iconbar/theme", ibprefs->theme);
	    e_db_str_set(db, "/iconbar/time_format", ibprefs->time_format);
	    e_db_int_set(db, "/iconbar/raise", ibprefs->raise);
	    e_db_int_set(db, "/iconbar/sticky", ecore_evas_sticky_get(ee));
	    e_db_int_set(db, "/iconbar/withdrawn", 
					ecore_evas_withdrawn_get(ee));
	    e_db_int_set(db, "/iconbar/borderless", 
					ecore_evas_borderless_get(ee));
	    e_db_int_set(db, "/iconbar/x", ibprefs->x);
	    e_db_int_set(db, "/iconbar/y", ibprefs->y);
	    e_db_int_set(db, "/iconbar/w", ibprefs->w);
	    e_db_int_set(db, "/iconbar/h", ibprefs->h);
	    for(l = ibprefs->icons; l; l = l->next)
	    {
		snprintf(buf, PATH_MAX, "/iconbar/icons/%d/path", i++);
		e_db_str_set(db, buf, (char*)l->data);
		free((char*)l->data);
		e_db_int_set(db, "/iconbar/icons/count", i);
	    }
	    e_db_close(db);
	    e_db_flush();
	}
	_iconbar_prefs_free(ibprefs);
    }
}
/* modify */
void 
iconbar_config_ecore_evas_set(Ecore_Evas *ee)
{
    if(ibprefs && ee)
	ibprefs->ee = ee;
}
void
iconbar_config_home_set(char *home)
{
    if(ibprefs && home)
    {
	if(e_file_exists(home))
	{
	    if(ibprefs->home) free(ibprefs->home);
	    ibprefs->home = strdup(home);
	}
    }
}
void
iconbar_config_time_format_set(char *str)
{
    if(ibprefs && str)
    {
	if(ibprefs->time_format) free(ibprefs->time_format);
	    ibprefs->time_format = strdup(str);
    }
}
void
iconbar_config_font_path_append(char *str)
{
    if(ibprefs && str)
    {
	if(e_file_exists(str))
	    ibprefs->fonts = evas_list_append(ibprefs->fonts, str);
    }
}
void
iconbar_config_geometry_set(int x, int y, int w, int h)
{
    if(ibprefs)
    {
	if(x > 0) ibprefs->x = x;
	if(y > 0) ibprefs->y = y;
	if(w > 1) ibprefs->w = w;
	if(h > 1) ibprefs->h = h;
    }
}
void
iconbar_config_theme_set(const char *theme)
{
    if(ibprefs)
    {
	char buf[PATH_MAX];
	if(ibprefs->theme) free(ibprefs->theme);
	if(theme)
	    snprintf(buf, PATH_MAX, "%s", theme);
	else 
	    snprintf(buf,PATH_MAX,"%s/.e/iconbar/iconbar.eet",getenv("HOME"));
	ibprefs->theme = strdup(buf);
    }
}
void
iconbar_config_icons_set(Evas_List *list)
{
    Evas_List *l = NULL;
    if(ibprefs && list)
    {
	if(ibprefs->icons)
	{
	    for(l = ibprefs->icons; l; l = l->next)
	    {
		if(l->data) free((char*)l->data);
	    }
	}
	evas_list_free(ibprefs->icons);
	ibprefs->icons = list;
    }
}
void
iconbar_config_borderless_set(int on)
{
    if(ibprefs && ibprefs->ee)
    {
	ibprefs->borderless = ecore_evas_borderless_get(ibprefs->ee);
	if(ibprefs->borderless != on)
	    ecore_evas_borderless_set(ibprefs->ee, on);
	ibprefs->borderless = on;
    }
}
void
iconbar_config_raise_lower_set(int raise)
{
    if(ibprefs && ibprefs->ee)
    {
	raise ? ecore_evas_raise(ibprefs->ee) : ecore_evas_lower(ibprefs->ee);
    }
}
void
iconbar_config_withdrawn_set(int on)
{
    if(ibprefs && ibprefs->ee)
    {
	ibprefs->withdrawn = ecore_evas_withdrawn_get(ibprefs->ee);
	if(ibprefs->withdrawn != on)
	    ecore_evas_withdrawn_set(ibprefs->ee, on);
	ibprefs->withdrawn = on;
    }
}
void
iconbar_config_sticky_set(int on)
{
    if(ibprefs && ibprefs->ee)
    {
	ibprefs->sticky = ecore_evas_sticky_get(ibprefs->ee);
	if(ibprefs->sticky != on)
	    ecore_evas_sticky_set(ibprefs->ee, on);
	ibprefs->sticky = on;
    }
}
/* query */
const char*
iconbar_config_theme_get(void)
{
    const char *result = NULL;
    if(ibprefs && ibprefs->theme)
	result = ibprefs->theme;
    return(result);
}
const char*
iconbar_config_time_format_get(void)
{
    const char *result = NULL;
    if(ibprefs && ibprefs->time_format)
	result = ibprefs->time_format;
    return(result);
}
const char*
iconbar_config_home_get(void)
{
    const char *result = NULL;
    if(ibprefs && ibprefs->home)
	result = ibprefs->home;
    return(result);
}
Evas_List *
iconbar_config_font_path_get(void)
{
    if(ibprefs && ibprefs->fonts)
	return(ibprefs->fonts);
    return(NULL);
}
Evas_List *
iconbar_config_icons_get(void)
{
    if(ibprefs && ibprefs->icons)
	return(ibprefs->icons);
    return(NULL);
}
void
iconbar_config_geometry_get(int *x, int *y, int *w, int *h)
{
    if(ibprefs)
    {
	if(x) *x = ibprefs->x;
	if(y) *y = ibprefs->y;
	if(w) *w = ibprefs->w;
	if(h) *h = ibprefs->h;
    }
}
int
iconbar_config_raise_lower_get(void)
{
    if(ibprefs)
	return(ibprefs->raise);
    return(0);
}
int
iconbar_config_borderless_get(void)
{
    if(ibprefs)
	return(ibprefs->borderless);
    return(0);
}
int
iconbar_config_withdrawn_get(void)
{
    if(ibprefs)
	return(ibprefs->withdrawn);
    return(0);
}
int
iconbar_config_sticky_get(void)
{
    if(ibprefs)
	return(ibprefs->sticky);
    return(0);
}
/*==========================================================================
 * Private Functions 
 *========================================================================*/
static Iconbar_Prefs*
_iconbar_prefs_new(void)
{
    Iconbar_Prefs *ip = NULL;
    char buf[PATH_MAX];
    if((ip = (Iconbar_Prefs*)malloc(sizeof(Iconbar_Prefs))))
    {
	memset(ip, 0, sizeof(Iconbar_Prefs));
	snprintf(buf, PATH_MAX, "%s", getenv("HOME"));
	ip->home = strdup(buf);
    }
    return(ip);
}
static void
_iconbar_prefs_free(Iconbar_Prefs *ip)
{
    if(ip)
    {
	Evas_List *l = NULL;
	if(ip->home) free(ip->home);
	if(ip->time_format) free(ip->time_format);
	for(l = ip->fonts; l; l = l->next)
	{
	    if(l->data) free(l->data);
	}
	ip->fonts = evas_list_free(ip->fonts);
	free(ip);
    }
}
static void
iconbar_config_generate_original_db(char *file)
{
    int i = 0;
    char buf[PATH_MAX], key[PATH_MAX], buf2[PATH_MAX];
    struct stat status;
    char *dirs[] = { ".e", ".e/iconbar", ".e/iconbar/themes",
	".e/iconbar/icons", ".e/iconbar/fonts" };
    char *icons[] = { "eterm", "xmms", "evolution", "galeon", "gaim", "gimp",
    "irc", "abiword", };
    E_DB_File *db = NULL;

    for(i = 0; i < 5; i++)
    {
	snprintf(buf, PATH_MAX, "%s/%s", getenv("HOME"), dirs[i]); 
	if(!stat(buf, &status))
	    continue;
	mkdir(buf, S_IRUSR | S_IWUSR | S_IXUSR);
    }
    if((file) && (db = e_db_open(file)))
    {
	snprintf(buf, PATH_MAX, "%s", getenv("HOME"));
	e_db_str_set(db, "/iconbar/home", buf);
	e_db_str_set(db, "/iconbar/time_format", "%l:%M %p");
	snprintf(buf, PATH_MAX, "%s/iconbar.eet", PACKAGE_DATA_DIR);
	snprintf(buf2, PATH_MAX, "%s/.e/iconbar/iconbar.eet",
	getenv("HOME"));
	if(e_file_cp(buf, buf2))
	    e_db_str_set(db, "/iconbar/theme", buf2);
	else
	    e_db_str_set(db, "/iconbar/theme", buf);

	e_db_int_set(db, "/iconbar/raise", 0);
	e_db_int_set(db, "/iconbar/withdrawn", 0);
	e_db_int_set(db, "/iconbar/borderless", 1);

	e_db_int_set(db, "/iconbar/x", 0);
	e_db_int_set(db, "/iconbar/y", 0);
	e_db_int_set(db, "/iconbar/w", 80);
	e_db_int_set(db, "/iconbar/h", 400);

	e_db_int_set(db, "/iconbar/fonts/count", 2);
	e_db_str_set(db, "/iconbar/fonts/0/path", PACKAGE_DATA_DIR);
	snprintf(buf, PATH_MAX, "%s/.e/iconbar/fonts", getenv("HOME"));
	e_db_str_set(db, "/iconbar/fonts/1/path", buf);
	
	e_db_int_set(db, "/iconbar/icons/count", 8);
	for(i = 0; i < 8; i++)
	{
	    snprintf(key,PATH_MAX,"/iconbar/icons/%d/path", i);
	    
	    snprintf(buf,PATH_MAX,"%s/icons/%s.app.eet",
			PACKAGE_DATA_DIR, icons[i]);
	    snprintf(buf2, PATH_MAX, "%s/.e/iconbar/icons/%s.app.eet",
			getenv("HOME"), icons[i]);
	    if(e_file_cp(buf, buf2))
	    {
		snprintf(buf, PATH_MAX, "%s.app.eet", icons[i]);
		e_db_str_set(db, key,  buf);
	    }
	    else
		e_db_str_set(db, key, buf);
	}
	e_db_close(db);
	e_db_flush();
    }
}
