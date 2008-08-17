#ifndef _EDATA_H
#define _EDATA_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
#ifdef BUILDING_DLL
#define EAPI __declspec(dllexport)
#else
#define EAPI __declspec(dllimport)
#endif
#else
#ifdef __GNUC__
#if __GNUC__ >= 4
#define EAPI __attribute__ ((visibility("default")))
#else
#define EAPI
#endif
#else
#define EAPI
#endif
#endif

/* we need this for size_t */
#include <stddef.h>

/**
 * @mainpage Edata
 * @file Edata.h
 * @brief Contains threading, list, hash, debugging and tree functions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __sgi
#define __FUNCTION__ "unknown"
#ifndef __cplusplus
#define inline
#endif
#endif
  

#include "core.h"
#include "list.h"
#include "hash.h"
#include "lalloc.h"
#include "sheap.h"
#include "tree.h"

#ifdef __cplusplus
}
#endif

#endif /* _EDATA_DATA_H */
