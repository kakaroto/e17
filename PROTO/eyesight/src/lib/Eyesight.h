#ifndef __EYESIGHT_H__
#define __EYESIGHT_H__

#include <Evas.h>


#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EYESIGHT_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EINA_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  EYESIGHT_BACKEND_NONE,
  EYESIGHT_BACKEND_IMG,
  EYESIGHT_BACKEND_PDF,
  EYESIGHT_BACKEND_PS,
  EYESIGHT_BACKEND_TXT
} Eyesight_Backend;

typedef enum
{
  EYESIGHT_ORIENTATION_PORTRAIT   = 0,   /**< Portrait orientation */
  EYESIGHT_ORIENTATION_LANDSCAPE  = 90,  /**< Landscape orientation */
  EYESIGHT_ORIENTATION_UPSIDEDOWN = 180, /**< Upside-down orientation */
  EYESIGHT_ORIENTATION_SEASCAPE   = 270  /**< Seascape orientation */
} Eyesight_Orientation;

typedef enum
{
  EYESIGHT_LINK_ACTION_UNKNOWN,          /**< */
  EYESIGHT_LINK_ACTION_GOTO,             /**< go to destination in the same file */
  EYESIGHT_LINK_ACTION_GOTO_REMOTE_FILE, /**< go to destination in a remote file */
  EYESIGHT_LINK_ACTION_LAUNCH,           /**< launch app or open docuent */
  EYESIGHT_LINK_ACTION_URI,              /**< URI */
  EYESIGHT_LINK_ACTION_NAMED,            /**< named action */
  EYESIGHT_LINK_ACTION_MOVIE,            /**< movie action */
  EYESIGHT_LINK_ACTION_RENDITION,        /**< */
  EYESIGHT_LINK_ACTION_SOUND,            /**< sound action */
  EYESIGHT_LINK_ACTION_JAVASCRIPT,       /**< JaveScript action */
  EYESIGHT_LINK_ACTION_OCG_STATE         /**< Set-OCG-State action */
} Eyesight_Link_Action_Kind;

typedef enum
{
  EYESIGHT_MOVIE_OPERATION_PLAY,
  EYESIGHT_MOVIE_OPERATION_PAUSE,
  EYESIGHT_MOVIE_OPERATION_RESUME,
  EYESIGHT_MOVIE_OPERATION_STOP
} Eyesight_Movie_Operation;


typedef struct _Eyesight_Index_Item Eyesight_Index_Item;
typedef struct _Eyesight_Link       Eyesight_Link;

typedef struct _Eyesight_Link_Action_Unknown       Eyesight_Link_Action_Unknown;
typedef struct _Eyesight_Link_Action_Goto          Eyesight_Link_Action_Goto;
typedef struct _Eyesight_Link_Action_Goto_Remote_File Eyesight_Link_Action_Goto_Remote_File;
typedef struct _Eyesight_Link_Action_Launch        Eyesight_Link_Action_Launch;
typedef struct _Eyesight_Link_Action_Uri           Eyesight_Link_Action_Uri;
typedef struct _Eyesight_Link_Action_Named         Eyesight_Link_Action_Named;
typedef struct _Eyesight_Link_Action_Movie         Eyesight_Link_Action_Movie;
typedef struct _Eyesight_Link_Action_Rendition     Eyesight_Link_Action_Rendition;
typedef struct _Eyesight_Link_Action_Sound         Eyesight_Link_Action_Sound;
typedef struct _Eyesight_Link_Action_Javascript    Eyesight_Link_Action_Javascript;
typedef struct _Eyesight_Link_Action_OCG_State     Eyesight_Link_Action_OCG_State;

struct _Eyesight_Link_Action_Unknown
{
  char *action;
};

struct _Eyesight_Link_Action_Goto
{
  int page;
};

struct _Eyesight_Link_Action_Goto_Remote_File
{
  char *filename;
};

struct _Eyesight_Link_Action_Launch
{
  char *app;
  char *params;
};

struct _Eyesight_Link_Action_Uri
{
  char *uri;
};

struct _Eyesight_Link_Action_Named
{
  char *name;
};

struct _Eyesight_Link_Action_Movie
{
  Eyesight_Movie_Operation operation;
  char *title;
};

struct _Eyesight_Link_Action_Rendition
{
  char *script;
};

struct _Eyesight_Link_Action_Sound
{
  double volume;
  Eina_Bool sync   : 1;
  Eina_Bool repeat : 1;
  Eina_Bool mix    : 1;
};

struct _Eyesight_Link_Action_Javascript
{
  char *script;
};

struct _Eyesight_Link_Action_OCG_State
{
  Eina_Bool preserved : 1;
};

struct _Eyesight_Link
{
  Eyesight_Link_Action_Kind action;
  Eina_Rectangle            rect;
  union {
    Eyesight_Link_Action_Unknown       action_unknown;
    Eyesight_Link_Action_Goto          action_goto;
    Eyesight_Link_Action_Goto_Remote_File action_goto_remote_file;
    Eyesight_Link_Action_Launch        action_launch;
    Eyesight_Link_Action_Uri           action_uri;
    Eyesight_Link_Action_Named         action_named;
    Eyesight_Link_Action_Movie         action_movie;
    Eyesight_Link_Action_Rendition     action_rendition;
    Eyesight_Link_Action_Sound         action_sound;
    Eyesight_Link_Action_Javascript    action_javascript;
    Eyesight_Link_Action_OCG_State     action_ocg_state;
  } dest;
};

EAPI int eyesight_init(void);
EAPI int eyesight_shutdown(void);

EAPI Evas_Object         *eyesight_object_add(Evas *evas);
EAPI Eina_Bool            eyesight_object_file_set(Evas_Object *obj,
                                                   const char *filename);
EAPI const char          *eyesight_object_file_get(Evas_Object *obj);
EAPI void                *eyesight_object_document_get(Evas_Object *obj);
EAPI const Eina_List     *eyesight_object_toc_get(Evas_Object *obj);
EAPI int                  eyesight_object_page_count(Evas_Object *obj);

EAPI void                 eyesight_object_page_set(Evas_Object *obj, int page);
EAPI int                  eyesight_object_page_get(Evas_Object *obj);
EAPI void                 eyesight_object_page_scale_set(Evas_Object *obj,
                                                         double hscale,
                                                         double vscale);
EAPI void                 eyesight_object_page_scale_get(Evas_Object *obj,
                                                         double *hscale,
                                                         double *vscale);
EAPI void                 eyesight_object_page_orientation_set(Evas_Object *obj,
                                                               Eyesight_Orientation orientation);
EAPI Eyesight_Orientation eyesight_object_page_orientation_get(Evas_Object *obj);
EAPI void                 eyesight_object_page_size_get(Evas_Object *obj,
                                                        int *width,
                                                        int *height);
EAPI void                 eyesight_object_page_render(Evas_Object *obj);
EAPI char                *eyesight_object_page_text_get(Evas_Object *obj,
                                                         Eina_Rectangle rect);
EAPI Eina_List           *eyesight_object_page_text_find(Evas_Object *obj,
                                                         const char *text,
                                                         Eina_Bool is_case_sensitive,
                                                         Eina_Bool backward);
EAPI Eina_List           *eyesight_object_page_links_get(Evas_Object *obj);

EAPI const char               *eyesight_index_item_title_get(const Eyesight_Index_Item *item);
EAPI Eyesight_Link_Action_Kind eyesight_index_item_action_kind_get(const Eyesight_Index_Item *item);
EAPI int                       eyesight_index_item_page_get(const Eyesight_Index_Item *item);
EAPI const Eina_List          *eyesight_index_item_children_get(const Eyesight_Index_Item *item);
Eina_Bool                      eyesight_index_item_is_open (const Eyesight_Index_Item *item);

#ifdef __cplusplus
}
#endif


#endif /* __EYESIGHT_H__ */
