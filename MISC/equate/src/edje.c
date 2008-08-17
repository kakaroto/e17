#include "Equate.h"
#include <Ecore_Config.h>

/**
 * Edjification by atmos and digitalfallout
 * edje_signals.[ch] handle the edje part of equate.  Translate signals
 * our edje emits into the backend.  All the operators, functions, and
 * input values must have a #define'd numeric constant so it can be
 * evaluated in _equate_interp.  
 *
 * Jan 10 DigitalFallout
 * Added theme path detection
 *
 */

#define EQ_EXIT 1
#define OP_CLR 2
#define OP_DIV 3
#define OP_MUT 4
#define OP_ADD 5
#define OP_SUB 6
#define OP_EQU 7
#define OP_DEC 8
#define OP_SIN 9
#define OP_COS 10
#define OP_TAN 11
#define OP_ROOT 12
#define OP_POW 13
#define OP_LOG 14
#define OP_LN 15
#define OP_EXP 16

#define NUM_0 20
#define NUM_1 21
#define NUM_2 22
#define NUM_3 23
#define NUM_4 24
#define NUM_5 25
#define NUM_6 26
#define NUM_7 27
#define NUM_8 28
#define NUM_9 29
#define NUM_PI 30

/**
 * defined in calc.tab.c
 */
extern void     equate_clear(void);
extern double   equate_eval(void);
extern int      equate_append(char *str);
extern const char *equate_string_get(void);

Evas_Object    *equate_edje_root;
Ecore_Evas     *equate_edje_window;
int             equate_edje_inited;

/**
 * Interpret all of our different signals, and do things !
 */
static void
_equate_interp(void *data, Evas_Object * o, const char *emission, const
               char *source)
{
   int             val = 0;

   if (data) {
      double          result;
      char            buf[BUFLEN];

      val = (int) data;
      switch (val) {
      case EQ_EXIT:
         ecore_main_loop_quit();
         break;
      case OP_CLR:
         equate_clear();
         break;
      case OP_DIV:
         equate_append("/");
         break;
      case OP_MUT:
         equate_append("*");
         break;
      case OP_ADD:
         equate_append("+");
         break;
      case OP_SUB:
         equate_append("-");
         break;
      case OP_EQU:
         snprintf(buf, BUFLEN, "%.10g", equate_eval());
         edje_object_part_text_set(o, "EquateAnswer", buf);
         return;
         break;
      case OP_DEC:
         equate_append(".");
         break;
      case OP_SIN:
         equate_append("sin");
         break;
      case OP_COS:
         equate_append("cos");
         break;
      case OP_TAN:
         equate_append("tan");
         break;
      case OP_ROOT:
         equate_append("sqrt");
         break;
      case OP_POW:
         equate_append("^");
         break;
      case OP_LOG:
         equate_append("log");
         break;
      case OP_LN:
         equate_append("ln");
         break;
      case OP_EXP:
         equate_append("exp");
         break;
      case NUM_0:
         equate_append("0");
         break;
      case NUM_1:
         equate_append("1");
         break;
      case NUM_2:
         equate_append("2");
         break;
      case NUM_3:
         equate_append("3");
         break;
      case NUM_4:
         equate_append("4");
         break;
      case NUM_5:
         equate_append("5");
         break;
      case NUM_6:
         equate_append("6");
         break;
      case NUM_7:
         equate_append("7");
         break;
      case NUM_8:
         equate_append("8");
         break;
      case NUM_9:
         equate_append("9");
         break;
      default:
         E(1, "Unknown edje signal operator %d", val);
         break;
         /* etc */
      }
      edje_object_part_text_set(o, "EquateAnswer", equate_string_get());

   }
}

/* equate_edje_callback_define - setup the edje signal interceptions
 * @o - the object we're assigning these callbacks to
 */
static void
equate_edje_callback_define(Evas_Object * o)
{

        /** Equate Application Callbacks **/
   /* EQ_EXIT - Quit Equate */
   edje_object_signal_callback_add(o,
                                   "EQ_EXIT", "", _equate_interp,
                                   (void *) EQ_EXIT);

        /** Equate Operation Callbacks **/
   /* OP_CLR - Clear Display */
   edje_object_signal_callback_add(o,
                                   "OP_CLR", "", _equate_interp,
                                   (void *) OP_CLR);

   /* OP_DIV - Division Operator */
   edje_object_signal_callback_add(o,
                                   "OP_DIV", "", _equate_interp,
                                   (void *) OP_DIV);

   /* OP_MUT - Mutplication Operator */
   edje_object_signal_callback_add(o,
                                   "OP_MUT", "", _equate_interp,
                                   (void *) OP_MUT);

   /* OP_ADD - Addition Operator */
   edje_object_signal_callback_add(o,
                                   "OP_ADD", "", _equate_interp,
                                   (void *) OP_ADD);

   /* OP_SUB - Subtraction Operator */
   edje_object_signal_callback_add(o,
                                   "OP_SUB", "", _equate_interp,
                                   (void *) OP_SUB);

   /* OP_EQU - Equals Operator */
   edje_object_signal_callback_add(o,
                                   "OP_EQU", "", _equate_interp,
                                   (void *) OP_EQU);

   /* OP_DEC - Decimal Operator */
   edje_object_signal_callback_add(o,
                                   "OP_DEC", "", _equate_interp,
                                   (void *) OP_DEC);
   /* OP_SIN - Sin of x in degrees */
   edje_object_signal_callback_add(o,
                                   "OP_SIN", "", _equate_interp,
                                   (void *) OP_SIN);
   /* OP_COS - Cos of x in degree */
   edje_object_signal_callback_add(o,
                                   "OP_COS", "", _equate_interp,
                                   (void *) OP_COS);
   /* OP_TAN - Tan of x in degrees */
   edje_object_signal_callback_add(o,
                                   "OP_TAN", "", _equate_interp,
                                   (void *) OP_TAN);
   /* OP_ROOT - The square root of x */
   edje_object_signal_callback_add(o,
                                   "OP_ROOT", "", _equate_interp,
                                   (void *) OP_ROOT);
   /* OP_POW - Raise x to the y power */
   edje_object_signal_callback_add(o,
                                   "OP_POW", "", _equate_interp,
                                   (void *) OP_POW);
   /* OP_LOG - Logarithm */
   edje_object_signal_callback_add(o,
                                   "OP_LOG", "", _equate_interp,
                                   (void *) OP_LOG);
   /* OP_LN - the natural logarithm */
   edje_object_signal_callback_add(o,
                                   "OP_LN", "", _equate_interp, (void *) OP_LN);
   /* OP_EXP - e to the x */
   edje_object_signal_callback_add(o,
                                   "OP_EXP", "", _equate_interp,
                                   (void *) OP_EXP);

        /** Equate Number Callbacks **/
   /* NUM_0 - Zero */
   edje_object_signal_callback_add(o,
                                   "NUM_0", "", _equate_interp, (void *) NUM_0);

   /* NUM_1 - One */
   edje_object_signal_callback_add(o,
                                   "NUM_1", "", _equate_interp, (void *) NUM_1);

   /* NUM_2 - Two */
   edje_object_signal_callback_add(o,
                                   "NUM_2", "", _equate_interp, (void *) NUM_2);

   /* NUM_3 - Three */
   edje_object_signal_callback_add(o,
                                   "NUM_3", "", _equate_interp, (void *) NUM_3);

   /* NUM_4 - Four */
   edje_object_signal_callback_add(o,
                                   "NUM_4", "", _equate_interp, (void *) NUM_4);

   /* NUM_5 - Five */
   edje_object_signal_callback_add(o,
                                   "NUM_5", "", _equate_interp, (void *) NUM_5);

   /* NUM_6 - Six */
   edje_object_signal_callback_add(o,
                                   "NUM_6", "", _equate_interp, (void *) NUM_6);

   /* NUM_7 - Seven */
   edje_object_signal_callback_add(o,
                                   "NUM_7", "", _equate_interp, (void *) NUM_7);

   /* NUM_8 - Eight */
   edje_object_signal_callback_add(o,
                                   "NUM_8", "", _equate_interp, (void *) NUM_8);

   /* NUM_9 - Nine */
   edje_object_signal_callback_add(o,
                                   "NUM_9", "", _equate_interp, (void *) NUM_9);
   /* NUM_PI - 3.14159 */
   edje_object_signal_callback_add(o, "NUM_PI", "", _equate_interp,
                                   (void *) NUM_PI);
}

/**
 * win_del_cb - when the window manager closes the window
 * @ee - the ecore_evas that was nuked
 */
static void
win_del_cb(Ecore_Evas * ee)
{
   equate_quit();
}

/**
 * win_resize_cb - When the ecore_evas is resized
 * @ee - the Ecore_Evas that was resized
 */
static void
win_resize_cb(Ecore_Evas * ee)
{
   int             w, h;
   int             minw, minh;
   int             maxw, maxh;
   Evas_Object    *o = NULL;

   if (ee) {
      ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
      ecore_evas_size_min_get(ee, &minw, &minh);
      ecore_evas_size_max_get(ee, &maxw, &maxh);
      if (w > minw && h > minh && h < maxh && w < maxw) {
         if ((o = evas_object_name_find(ecore_evas_get(ee), "bg")))
            evas_object_resize(o, w, h);
         if ((o = evas_object_name_find(ecore_evas_get(ee), "edje")))
            evas_object_resize(o, w, h);
      }
   }
}

/**
 * win_move_cb - When the ecore_evas is moved
 * @ee - the Ecore_Evas that moved
 */
static void
win_move_cb(Ecore_Evas * ee)
{
}

/**
 * exit_cb - I believe handles things like Ctrl-C
 * @ev - unused
 * @ev_type - unused
 * @data - unused
 */
static int
exit_cb(void *ev, int ev_type, void *data)
{
   equate_quit();
   return (0);
}

/**
 * equate_edje_init : Given the input struct, setup and handle our edje
 * This is the only entry point to the edje subsystem, it runs, and
 * returns when the app is closed/quit
 */
void
equate_edje_init(Equate * eq)
{
   extern Ecore_Config_Bundle *props;

   Evas           *evas = NULL;
   Evas_Object    *o = NULL;

   eq->gui.ee = NULL;
   eq->gui.edje = NULL;

   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);
   if (ecore_evas_init()) {
      equate_edje_inited = 1;
      eq->gui.ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 300, 120);
      equate_edje_window = eq->gui.ee;
      ecore_evas_title_set(eq->gui.ee, "Equate");
      ecore_evas_callback_delete_request_set(eq->gui.ee, win_del_cb);
      ecore_evas_callback_resize_set(eq->gui.ee, win_resize_cb);
      ecore_evas_callback_move_set(eq->gui.ee, win_move_cb);

      evas = ecore_evas_get(eq->gui.ee);
      ecore_config_evas_font_path_apply(evas);

      o = evas_object_rectangle_add(evas);
      evas_object_move(o, 0, 0);
      evas_object_resize(o, 99999, 99999);
      evas_object_layer_set(o, 0);
      evas_object_name_set(o, "bg");
      evas_object_show(o);

      edje_init();
      edje_frametime_set(1.0 / 30.0);

      equate_edje_root = edje_object_add(evas);
      evas_object_move(equate_edje_root, 0, 0);

      equate_edje_theme_set(eq->conf.theme);
      equate_edje_callback_define(equate_edje_root);
      ecore_evas_show(eq->gui.ee);
      ecore_main_loop_begin();
   }
}

int
equate_edje_theme_set(char *theme)
{
//  char         tmp[PATH_MAX];
   Evas_Coord      mw, mh;
   char           *text;

   if (!equate_edje_inited)
      return;
   text = edje_object_part_text_get(equate_edje_root, "EquateAnswer");
#if 0
   if ((strstr(theme, "/")))
      snprintf(tmp, PATH_MAX, theme);
   else
      snprintf(tmp, PATH_MAX, PACKAGE_DATA_DIR "/themes/%s.edj", theme);
#endif
   if (edje_object_file_set(equate_edje_root,
                            ecore_config_theme_with_path_from_name_get(theme),
                            "Main")) {
      evas_object_name_set(equate_edje_root, "edje");
      edje_object_size_min_get(equate_edje_root, &mw, &mh);
      ecore_evas_size_min_set(equate_edje_window, (int) mw, (int) mh);
      ecore_evas_resize(equate_edje_window, (int) mw, (int) mh);
      evas_object_resize(equate_edje_root, mw, mh);
      edje_object_size_max_get(equate_edje_root, &mw, &mh);
      ecore_evas_size_max_set(equate_edje_window, (int) mw, (int) mh);
      evas_object_show(equate_edje_root);

      edje_object_part_text_set(equate_edje_root, "EquateAnswer", text);
   } else {
      fprintf(stderr, "Unable to open %s for edje theme\n", theme);
      equate_quit();
   }
}

void
equate_edje_quit()
{
   ecore_main_loop_quit();
   edje_shutdown();
   equate_edje_inited = 0;
   ecore_evas_shutdown();
}
