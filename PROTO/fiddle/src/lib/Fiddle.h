/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef FIDDLE_H
#define FIDDLE_H

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ELEMENTARY_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EVAS_BUILD */
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
#endif /* ! _WIN32 */

/* allow usage from c++ */
#ifdef __cplusplus
extern "C" {
#endif

#define FIDDLE_VERSION 1
   
/**************************************************************************/   
   
   typedef int                                    Fiddle_Coord;
   typedef long long                              Fiddle_Timestamp;
   typedef unsigned int                           Fiddle_Button_Mask;
   
   typedef struct _Fiddle_Context                 Fiddle_Context;
   typedef struct _Fiddle_Touch                   Fiddle_Touch;
   typedef struct _Fiddle_Touch_Mouse_Move_Simple Fiddle_Touch_Mouse_Move_Simple;
   typedef struct _Fiddle_Touch_Mouse_Down_Simple Fiddle_Touch_Mouse_Down_Simple;
   typedef struct _Fiddle_Touch_Mouse_Up_Simple   Fiddle_Touch_Mouse_Up_Simple;
   typedef struct _Fiddle_Event                   Fiddle_Event;
   
   typedef enum _Fiddle_Touch_Type
     {
        FIDDLE_TOUCH_INVALID,
        FIDDLE_TOUCH_MOUSE_MOVE_SIMPLE,
        FIDDLE_TOUCH_MOUSE_DOWN_SIMPLE,
        FIDDLE_TOUCH_MOUSE_UP_SIMPLE,
          // FIXME: more input types. eg multitouch
        FIDDLE_TOUCH_LAST
     } Fiddle_Touch_Type;
   
   struct _Fiddle_Touch_Mouse_Move_Simple
     {
        Fiddle_Coord       x, y;
        Fiddle_Button_Mask buttons;
     };
   
   struct _Fiddle_Touch_Mouse_Down_Simple
     {
        Fiddle_Coord       x, y;
        int                button;
     };
   
   struct _Fiddle_Touch_Mouse_Up_Simple
     {
        Fiddle_Coord       x, y;
        int                button;
     };
   
   struct _Fiddle_Touch
     {
        int version; // don't touch. FIDDLE_TOUCH_INIT sets this
        Fiddle_Touch_Type type;
        Fiddle_Timestamp timestamp;
        union {
           Fiddle_Touch_Mouse_Move_Simple mouse_move_simple;
           Fiddle_Touch_Mouse_Down_Simple mouse_down_simple;
           Fiddle_Touch_Mouse_Up_Simple   mouse_up_simple;
           // FIXME: more input types. eg multitouch 
       } info;
     };
   
   struct _Fiddle_Event
     {
        int version; // just read
        // FIXME: fiddle events
     };
   
   #define FIDDLE_TOUCH_INIT(t) do { t->version = FIDDLE_VERSION; } while (0);
     
/**************************************************************************/   

   EAPI void fiddle_init                         (void);
   EAPI void fiddle_shutdown                     (void);

   EAPI Fiddle_Context      *fiddle_context_new  (void);
   EAPI void                 fiddle_context_free (Fiddle_Context *c);
   
   EAPI const Fiddle_Event *fiddle_touch_feed    (Fiddle_Context *c, const Fiddle_Touch *t);
   EAPI const Fiddle_Event *fiddle_event_get     (Fiddle_Context *c);
   EAPI void                fiddle_history_flush (Fiddle_Context *c);
   
/**************************************************************************/   

#ifdef __cplusplus
}
#endif

#endif
