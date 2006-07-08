#ifndef EDJE_MAIN_H
#define EDJE_MAIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Evas.h>
#include <Ecore.h>
#ifdef HAVE_ECORE_X_H
#include <Ecore_X.h>
#endif
#ifdef HAVE_ECORE_FB_H
#include <Ecore_Fb.h>
#endif
#include <Ecore_Evas.h>
#include <Ecore_Config.h>
#include <Eet.h>
#include <Edje.h>
#include <etk/Etk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <locale.h>
#include <ctype.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define FREE(ptr) do { if(ptr) { free(ptr); ptr = NULL; }} while (0);

#include "edje_etk.h"
#include "etk_gui.h"
#include "conf.h"
#define DAT PACKAGE_DATA_DIR"/"

#endif

