#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "../ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>
#include <Ecore.h>
#include <Ecore_Data.h>

#include "EtkTypes.h"
#include "EtkSignals.h"


MODULE = Etk::Theme	PACKAGE = Etk::Theme	PREFIX = etk_theme_
	
void
etk_theme_init()
	ALIAS:
	Init=1

void
etk_theme_shutdown()
      ALIAS:
	Shutdown=1

const char *
etk_theme_widget_path_get()
      ALIAS:
	WidgetPathGet=1

const char *
etk_theme_widget_name_get()
      ALIAS:
	WidgetNameGet=1

Etk_Bool
etk_theme_widget_set_from_name(theme_name)
	char *	theme_name
      ALIAS:
	WidgetSetFromName=1

Etk_Bool
etk_theme_widget_set_from_path(theme_path)
	char *	theme_path
      ALIAS:
	WidgetSetFromPath=1

void
etk_theme_widget_available_themes_get()
	ALIAS:
	WidgetAvailableThemesGet=1
	PPCODE:
	Evas_List * list;

	list = etk_theme_widget_available_themes_get();
	XPUSHs(sv_2mortal(newSVCharEvasList(list)));

char *
etk_theme_widget_find(theme_name)
	const char * 	theme_name
	ALIAS:
	WidgetFind=1

const char *
etk_theme_icon_path_get()
	ALIAS:
	IconPathGet=1

const char *
etk_theme_icon_name_get()
	ALIAS:
	IconNameGet=1

Etk_Bool
etk_theme_icon_set_from_path(theme)
	const char * theme
	ALIAS:
	IconSetFromPath=1

Etk_Bool
etk_theme_icon_set_from_name(theme)
	const char * theme
	ALIAS:
	IconSetFromName=1

void
etk_theme_icon_available_themes_get()
	ALIAS:
	IconAvailableThemesGet=1
	PPCODE:
	Evas_List * list;

	list = etk_theme_icon_available_themes_get();
	XPUSHs(sv_2mortal(newSVCharEvasList(list)));
	
char *
etk_theme_icon_find(theme_name)
	const char * 	theme_name
	ALIAS:
	IconFind=1



Etk_Bool
etk_theme_group_exists(file, group, parent)
	const char * file
	const char * group
	const char * parent
	ALIAS:
	GroupExists=1


