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
   typedef enum _Elm_Obj_Type
     {
	ELM_OBJ_OBJ,
	ELM_OBJ_CB,
	ELM_OBJ_WIN
     } Elm_Obj_Type;
   
   typedef enum _Elm_Cb_Type
     {
	ELM_CB_DEL,
	  ELM_CB_CHILD_ADD,
	  ELM_CB_CHILD_DEL,
	  ELM_CB_DEL_REQ,
	  ELM_CB_RESIZE
     } Elm_Cb_Type;
   
   typedef enum _Elm_Win_Type
     {
	ELM_WIN_BASIC,
	  ELM_WIN_DIALOG_BASIC,
	  ELM_WIN_SCROLLABLE
     } Elm_Win_Type;
   
   typedef struct _Elm_Obj_Class       Elm_Obj_Class;
   typedef struct _Elm_Obj             Elm_Obj;
   typedef struct _Elm_Cb        Elm_Cb;
   typedef struct _Elm_Win_Class       Elm_Win_Class;
   typedef struct _Elm_Win             Elm_Win;
   
   typedef void (*Elm_Cb_Func) (void *data, Elm_Obj *obj, Elm_Cb_Type type, void *info);
   
   /* API calls here */

/**************************************************************************/   
   /* General calls */
   EAPI void elm_init(int argc, char **argv);
   EAPI void elm_shutdown(void);
   EAPI void elm_run(void);
   EAPI void elm_exit(void);

/**************************************************************************/   
   /* Generic Elm Object */
#define Elm_Obj_Class_Methods \
   void (*del)                        (Elm_Obj *obj); \
   void (*ref)                        (Elm_Obj *obj); \
   void (*unref)                      (Elm_Obj *obj); \
   Elm_Cb *(*cb_add)                  (Elm_Obj *obj, Elm_Cb_Type type, Elm_Cb_Func func, void *data); \
   void (*child_add)                  (Elm_Obj *obj, Elm_Obj *child)
#define Elm_Obj_Class_All Elm_Obj_Class_Methods; \
   Elm_Obj_Type   type; \
   void          *clas; /* parent class fo those that inherit */ \
   Elm_Obj       *parent; \
   Evas_List     *children; \
   Evas_List     *cbs; \
   int            refs; \
   unsigned char  delete_me : 1; \
   unsigned char  delete_deferred : 1
   
   struct _Elm_Obj_Class
     {
	void *parent;
	Elm_Obj_Class_Methods;
     };
   struct _Elm_Obj
     {
	Elm_Obj_Class_All;
     };
#define ELM_OBJ(o) ((Elm_Obj *)o)   
   
/**************************************************************************/   
   /* Callback Object */
#define Elm_Cb_Class_Methods
#define Elm_Cb_Class_All Elm_Cb_Class_Methods; \
   Elm_Cb_Class_Methods; \
   Elm_Cb_Type  cb_type; \
   Elm_Cb_Func  func; \
   void              *data;
   struct _Elm_Cb_Class
     {
	void *parent;
	Elm_Cb_Class_Methods;
     };
   struct _Elm_Cb
     {
	Elm_Obj_Class_All;
	Elm_Cb_Class_All;
     };

/**************************************************************************/   
   /* Window Object */
#define Elm_Win_Class_Methods \
   void (*name_set)  (Elm_Win *win, const char *name); \
   void (*title_set) (Elm_Win *win, const char *title); \
   void (*show)      (Elm_Win *win); \
   void (*hide)      (Elm_Win *win)

// cover methods & state for:
// type, fullscreen, icon, activate, shaped, alpha, borderless, iconified
#define Elm_Win_Class_All Elm_Win_Class_Methods; \
   Elm_Win_Type  win_type; \
   const char   *name; \
   const char   *title; \
   int           w, h; \
   unsigned char autodel;
   
   /* Object specific ones */
   EAPI Elm_Win *elm_win_new(void);
   struct _Elm_Win_Class
     {
	void *parent;
	Elm_Win_Class_Methods;
     };
   struct _Elm_Win
     {
	Elm_Obj_Class_All;
	Elm_Win_Class_All;
	
	Ecore_Evas     *ee; /* private */
	Evas           *evas; /* private */
	Ecore_X_Window  xwin; /* private */
	Ecore_Job      *deferred_resize_job; /* private */
     };
   
#ifdef __cplusplus
}
#endif

#endif
