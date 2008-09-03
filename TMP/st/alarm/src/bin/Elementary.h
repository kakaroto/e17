/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef ELEMENTARY_H
#define ELEMENTARY_H

/* What is this?
 * 
 * This is a VERY SIMPLE toolkit - VERY SIMPLE. It is not meant for writing
 * extensive applications. Small simple ones with simple needs. It is meant
 * to make the programmers work almost brainless.
 * 
 * I'm toying with backing this with etk or ewl - or not. right now I am
 * unsure as some of the widgets will be faily complex edje creations. Do
 * not consider any choices permanent - but the API should stay unbroken.
 * 
 */

// FIXME: this should become the simple widget/toolkit main header.

// FIXME: install elementary config.h for use later
#include "config.h"

/* Standard headers for standard system calls etc. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
#include <dlfcn.h>
#include <math.h>
#include <fnmatch.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <glob.h>
#include <locale.h>
#include <libintl.h>
#include <errno.h>
#include <signal.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

/* EFL headers */
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Ecore_Con.h>
#include <Ecore_Ipc.h>
#include <Ecore_Job.h>
#include <Ecore_Txt.h>
#include <Ecore_File.h>
#include <Eet.h>
#include <Edje.h>

/* allow usage from c++ */
#ifdef __cplusplus
extern "C" {
#endif
   
   /* Types here */
   typedef enum _Elm_Win_Type
     {
	ELM_WIN_BASIC,
	  ELM_WIN_DIALOG_BASIC,
	  ELM_WIN_SCROLLABLE
     } Elm_Win_Type;
   
   typedef enum _Elm_Callback_Type
     {
	ELM_CALLBACK_DEL,
	  ELM_CALLBACK_CHILD_ADD,
	  ELM_CALLBACK_CHILD_DEL
     } Elm_Callback_Type;
   
   typedef struct _Elm_Obj      Elm_Obj;
   typedef struct _Elm_Callback Elm_Callback;
   typedef struct _Elm_Win      Elm_Win;
   
   typedef void (*Elm_Callback_Func) (void *data, Elm_Obj *obj, Elm_Callback_Type type, void *info);
   
   /* API calls here */

   // FIXME: need a way to store previous call and chain method calls.
   // 
   /* General calls */
   EAPI void elm_init(int argc, char **argv);
   EAPI void elm_shutdown(void);
   EAPI void elm_run(void);
   EAPI void elm_exit(void);
   
   /* Generic Elm Object */
#define Elm_Obj_Class \
   void (*del)                        (Elm_Obj *obj); \
   void (*ref)                        (Elm_Obj *obj); \
   void (*unref)                      (Elm_Obj *obj); \
   Elm_Callback *(*callback_add)      (Elm_Obj *obj, Elm_Callback_Type type, Elm_Callback cb, void *data); \
   void (*child_add)                  (Elm_Obj *obj, Elm_Obj *child); \
   \
   int           refs; \
   Elm_Obj      *parent; \
   Evas_List    *children; \
   Evas_List    *callbacks; \
   unsigned char delete_me : 1
   
   struct _Elm_Obj
     {
	Elm_Obj_Class;
     };
#define ELM_OBJ(o) ((Elm_Obj *)o)   
   
   struct _Elm_Callback
     {
	Elm_Obj_Class;
	
	Elm_Callback_Type  type;
	Elm_Callback_Func  func;
	void              *data;
     };
   
   /* Object specific ones */
   EAPI Elm_Win *elm_win_new(void);
   struct _Elm_Win
     {
	Elm_Obj_Class;
	
	void (*name_set)  (Elm_Win *win, const char *name);
	void (*title_set) (Elm_Win *win, const char *title);
	void (*show)      (Elm_Win *win);
	void (*hide)      (Elm_Win *win);
	
	Elm_Win_Type  type;
	const char   *name;
	const char   *title;
	
	// FIXME: private - hide
	Ecore_Evas *ee;
	Evas       *evas;
     };
   
#ifdef __cplusplus
}
#endif

#endif
