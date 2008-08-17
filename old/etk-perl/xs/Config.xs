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

MODULE = Etk::Config	PACKAGE = Etk::Config	PREFIX = etk_config_

Etk_Bool
etk_config_init()
	ALIAS:
	Init=1

void
etk_config_shutdown()
	ALIAS:
	Shutdown=1

Etk_Bool
etk_config_load()
	ALIAS:
	Load=1

Etk_Bool
etk_config_save()
	ALIAS:
	Save=1

const char *
etk_config_widget_theme_get()
	ALIAS:
	WidgetThemeGet=1

void
etk_config_widget_theme_set(widget_theme)
	const char *widget_theme
	ALIAS:
	WidgetThemeSet=1

const char *
etk_config_font_get()
	ALIAS:
	FontGet=1

void
etk_config_font_set(font)
	const char * font
	ALIAS:
	FontSet=1

const char *
etk_config_engine_get()
	ALIAS:
	EngineGet=1

void
etk_config_engine_set(engine)
	const char *engine
	ALIAS:
	EngineSet=1


