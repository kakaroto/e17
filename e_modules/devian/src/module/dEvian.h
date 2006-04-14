#ifndef DEVIAN_H
#define DEVIAN_H

#include <e.h>
#include <config.h>
#include <E_Lib.h>

/* Changes in all the source, depend wich module we are building */
#ifdef HAVE_ALL
#define MODULE_NAME "dEvian"
#define MODULE_NAME_NOCASE "devian"
#define DEVIANN dEvian
#define DEVIANM dEvianM
#define DEVIAN_CONF dEvian_Conf
#define DEVIAN_MAIN dEvian_Main
#define DEVIAN_MAIN_CONF dEvian_Main_Conf
#define DEVIANF(x) devian_ ## x
#else
#ifdef HAVE_PICTURE
#define MODULE_NAME "dEphoto"
#define MODULE_NAME_NOCASE "dephoto"
#define DEVIANN dEphoto
#define DEVIANM dEphotoM
#define DEVIAN_CONF dEphoto_Conf
#define DEVIAN_MAIN dEphoto_Main
#define DEVIAN_MAIN_CONF dEphoto_Main_Conf
#define DEVIANF(x) dephoto_ ## x
#else
#ifdef HAVE_RSS
#define MODULE_NAME "dErss"
#define MODULE_NAME_NOCASE "derss"
#define DEVIANN dErss
#define DEVIANM dErssM
#define DEVIAN_CONF dErss_Conf
#define DEVIAN_MAIN dErss_Main
#define DEVIAN_MAIN_CONF dErss_Main_Conf
#define DEVIANF(x) derss_ ## x
#else
#ifdef HAVE_FILE
#define MODULE_NAME "dElog"
#define MODULE_NAME_NOCASE "delog"
#define DEVIANN dElog
#define DEVIANM dElogM
#define DEVIAN_CONF dElog_Conf
#define DEVIAN_MAIN dElog_Main
#define DEVIAN_MAIN_CONF dElog_Main_Conf
#define DEVIANF(x) delog_ ## x
#endif
#endif
#endif
#endif

/* Headers */
#define E_MOD_DEVIAN_TYPEDEFS 1
#include "dEvian_includes.h"
#undef E_MOD_DEVIAN_TYPEDEFS
#include "dEvian_includes.h"

/* Debuging things */
#ifdef DEBUG_MAIN
# define DMAIN(x)  do {printf("MAIN - %s:%d:  ", __FILE__, __LINE__); printf x; printf("\n"); fflush(stdout);} while (0)
#else
# define DMAIN(x)  ((void) 0)
#endif
#ifdef DEBUG_POPUP_WARN
# define DPOPW(x)  do {printf("POPW - %s:%d: ", __FILE__, __LINE__); printf x; printf ("\n"); fflush(stdout);} while (0)
#else
# define DPOPW(x)  ((void) 0)
#endif
#ifdef DEBUG_SOURCE
# define DSOURCE(x)  do {printf("SOURCE - %s:%d:  ", __FILE__, __LINE__); printf x; printf ("\n"); fflush(stdout);} while (0)
#else
# define DSOURCE(x)  ((void) 0)
#endif
#ifdef DEBUG_CONTAINER
# define DCONTAINER(x)  do {printf("CONTAINER - %s:%d:  ", __FILE__, __LINE__); printf x; printf ("\n"); fflush(stdout);} while (0)
#else
# define DCONTAINER(x)  ((void) 0)
#endif
#ifdef DEBUG_DATA
# define DDATA(x)  do {printf("DATA - %s:%d:  ", __FILE__, __LINE__); printf x; printf ("\n"); fflush(stdout);} while (0)
#else
# define DDATA(x)  ((void) 0)
#endif
#ifdef DEBUG_DATA_CACHE
# define DDATAC(x)  do {printf("DATA_CACHE %s:%d:  ", __FILE__, __LINE__); printf x; printf ("\n"); fflush(stdout);} while (0)
#else
# define DDATAC(x)  ((void) 0)
#endif
#ifdef DEBUG_DATA_RSS
# define DDATARSS(x)  do {printf("DATA_RSS %s:%d:  ", __FILE__, __LINE__); printf x; printf ("\n"); fflush(stdout);} while (0)
#else
# define DDATARSS(x)  ((void) 0)
#endif
#ifdef DEBUG_DATA_RSS_PARSER
# define DDATARSSP(x)  do {printf("DATA_RSS_PARSE - %s:%d:  ", __FILE__, __LINE__); printf x; printf ("\n"); fflush(stdout);} while (0)
#else
# define DDATARSSP(x)  ((void) 0)
#endif
#ifdef DEBUG_DATA_FILE
# define DDATAFILE(x)  do {printf("DATA_FILE - %s:%d:  ", __FILE__, __LINE__); printf x; printf ("\n"); fflush(stdout);} while (0)
#else
# define DDATAFILE(x)  ((void) 0)
#endif

#endif
