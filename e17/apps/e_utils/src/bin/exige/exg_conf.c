#include "exg_conf.h"

#include <stdio.h>

static int exg_conf_listener(const char *key, const Ecore_Config_Type type, const int tag, void *data);

int exg_conf_init(Exige *exg)
{
    ecore_config_init("exige");
    
    exg_conf_default_set();

    ecore_config_listen("on_mouse", "/settings/on_mouse", exg_conf_listener, EXG_CONF_TAG_ON_MOUSE, exg);
    ecore_config_listen("terminal", "/settings/terminal", exg_conf_listener, EXG_CONF_TAG_TERM, exg);

    return 1;
}

void exg_conf_default_set()
{
    ecore_config_theme_default("/settings/theme", "default");
    ecore_config_int_default("/settings/on_mouse", 1);
    ecore_config_string_default("/settings/terminal", "Eterm");
    ecore_config_load();
}

char *exg_conf_theme_get()
{
    char *theme = ecore_config_theme_get("/settings/theme");
    return theme;
}

void exg_conf_theme_set(char *theme)
{
    ecore_config_theme_set("/settings/theme", theme);
}

int exg_conf_on_mouse_get()
{
    int on_mouse = ecore_config_int_get("/settings/on_mouse");
    return on_mouse;
}

void exg_conf_on_mouse_set(int on_mouse)
{
    ecore_config_int_set("/settings/on_mouse", on_mouse);
}

char *exg_conf_term_get()
{
    char *term = ecore_config_string_get("/settings/terminal");
    return term;
}

void exg_conf_term_set(char *term)
{
    ecore_config_string_set("/settings/terminal", term);
}

static int exg_conf_listener(const char *key, const Ecore_Config_Type type, const int tag, void *data)
{
    printf("config change!\n");
    return 1;
}
