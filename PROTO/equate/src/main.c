#include <Elementary.h>
#ifndef ELM_LIB_QUICKLAUNCH

#include "Equate.h"
#include "calc.h"

static Evas_Object *win;
static Evas_Object *ly;


static const struct
{
    const char *keyname;
    int op;
} _keymap[] = {
    {"KP_0",        NUM_0},
    {"0",           NUM_0},
    {"KP_1",        NUM_1},
    {"1",           NUM_1},
    {"KP_2",        NUM_2},
    {"2",           NUM_2},
    {"KP_3",        NUM_3},
    {"3",           NUM_3},
    {"KP_4",        NUM_4},
    {"4",           NUM_4},
    {"KP_5",        NUM_5},
    {"5",           NUM_5},
    {"KP_6",        NUM_6},
    {"6",           NUM_6},
    {"KP_7",        NUM_7},
    {"7",           NUM_7},
    {"KP_8",        NUM_8},
    {"8",           NUM_8},
    {"KP_9",        NUM_9},
    {"9",           NUM_9},
    {"KP_Divide",   OP_DIV},
    {"KP_Multiply", OP_MUT},
    {"KP_Subtract", OP_SUB},
    {"KP_Add",      OP_ADD},
    {"plus",        OP_ADD},
    {"minus",       OP_SUB},
    {"asterisk",    OP_MUT},
    {"slash",       OP_DIV},
    {"KP_Enter",    OP_EQU},
    {"Return",      OP_EQU},
    {"BackSpace",   OP_CLR},
    {"period",      OP_DEC},
    {"comma",       OP_DEC},
    NULL
};


static void
_signal_cb(void *data, Evas_Object * o, const char *emission, const
               char *source)
{
   int             val = 0;

   if (data)
     {
        double          result;
        char            buf[BUFLEN];

        val = (int) data;
        switch (val)
          {
           case EQ_EXIT:
              elm_exit();
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
          case OP_OBRAK:
              equate_append("(");
              break;
          case OP_CBRAK:
              equate_append(")");
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


static void
_edje_callbacks_define(Evas_Object * o)
{
   /** Equate Application Callbacks **/
   /* EQ_EXIT - Quit Equate */
   edje_object_signal_callback_add(o, "EQ_EXIT", "*", _signal_cb,
                                   (void *) EQ_EXIT);
   /** Equate Operation Callbacks **/
   /* OP_CLR - Clear Display */
   edje_object_signal_callback_add(o, "OP_CLR", "*", _signal_cb,
                                   (void *) OP_CLR);
   /* OP_DIV - Division Operator */
   edje_object_signal_callback_add(o, "OP_DIV", "*", _signal_cb,
                                   (void *) OP_DIV);
   /* OP_MUT - Mutplication Operator */
   edje_object_signal_callback_add(o, "OP_MUT", "*", _signal_cb,
                                   (void *) OP_MUT);
   /* OP_ADD - Addition Operator */
   edje_object_signal_callback_add(o, "OP_ADD", "*", _signal_cb,
                                   (void *) OP_ADD);
   /* OP_SUB - Subtraction Operator */
   edje_object_signal_callback_add(o, "OP_SUB", "*", _signal_cb,
                                   (void *) OP_SUB);
   /* OP_EQU - Equals Operator */
   edje_object_signal_callback_add(o, "OP_EQU", "*", _signal_cb,
                                   (void *) OP_EQU);
   /* OP_DEC - Decimal Operator */
   edje_object_signal_callback_add(o, "OP_DEC", "*", _signal_cb,
                                   (void *) OP_DEC);
   /* OP_SIN - Sin of x in degrees */
   edje_object_signal_callback_add(o, "OP_SIN", "*", _signal_cb,
                                   (void *) OP_SIN);
   /* OP_COS - Cos of x in degree */
   edje_object_signal_callback_add(o, "OP_COS", "*", _signal_cb,
                                   (void *) OP_COS);
   /* OP_TAN - Tan of x in degrees */
   edje_object_signal_callback_add(o, "OP_TAN", "*", _signal_cb,
                                   (void *) OP_TAN);
   /* OP_ROOT - The square root of x */
   edje_object_signal_callback_add(o, "OP_ROOT", "*", _signal_cb,
                                   (void *) OP_ROOT);
   /* OP_POW - Raise x to the y power */
   edje_object_signal_callback_add(o, "OP_POW", "*", _signal_cb,
                                   (void *) OP_POW);
   /* OP_LOG - Logarithm */
   edje_object_signal_callback_add(o, "OP_LOG", "*", _signal_cb,
                                   (void *) OP_LOG);
   /* OP_LN - the natural logarithm */
   edje_object_signal_callback_add(o, "OP_LN", "*", _signal_cb, (void *) OP_LN);
   /* OP_EXP - e to the x */
   edje_object_signal_callback_add(o, "OP_EXP", "*", _signal_cb,
                                   (void *) OP_EXP);
   /* OP_OBRAK - open bracket */
   edje_object_signal_callback_add(o, "OP_OBRAK", "*", _signal_cb,
                                   (void *) OP_OBRAK);
   /* OP_CBRAK - close bracket */
   edje_object_signal_callback_add(o, "OP_CBRAK", "*", _signal_cb,
                                   (void *) OP_CBRAK);

   /** Equate Number Callbacks **/
   edje_object_signal_callback_add(o, "NUM_0", "*", _signal_cb, (void *) NUM_0);
   edje_object_signal_callback_add(o, "NUM_1", "*", _signal_cb, (void *) NUM_1);
   edje_object_signal_callback_add(o, "NUM_2", "*", _signal_cb, (void *) NUM_2);
   edje_object_signal_callback_add(o, "NUM_3", "*", _signal_cb, (void *) NUM_3);
   edje_object_signal_callback_add(o, "NUM_4", "*", _signal_cb, (void *) NUM_4);
   edje_object_signal_callback_add(o, "NUM_5", "*", _signal_cb, (void *) NUM_5);
   edje_object_signal_callback_add(o, "NUM_6", "*", _signal_cb, (void *) NUM_6);
   edje_object_signal_callback_add(o, "NUM_7", "*", _signal_cb, (void *) NUM_7);
   edje_object_signal_callback_add(o, "NUM_8", "*", _signal_cb, (void *) NUM_8);
   edje_object_signal_callback_add(o, "NUM_9", "*", _signal_cb, (void *) NUM_9);
   /* NUM_PI - 3.14159 */
   edje_object_signal_callback_add(o, "NUM_PI", "*", _signal_cb, (void *) NUM_PI);
}

static Eina_Bool
_key_down_cb(void *data, int type, void *event)
{
    Ecore_Event_Key *ev = event;
    int i;

    printf("{\"%s\", },\n", ev->key);

    for (i = 0; _keymap[i].keyname; i++)
    {
        if (!strcmp(_keymap[i].keyname, ev->key))
        {
            _signal_cb((void*)_keymap[i].op, elm_layout_edje_get(ly), NULL, NULL);
        }
    }
     return EINA_TRUE;

}

static void
_create_gui(void)
{
   char buf[1024];

   win = elm_win_add(NULL, "equate", ELM_WIN_BASIC);
   elm_win_title_set(win, "Equate");
   elm_win_autodel_set(win, EINA_TRUE);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   ly = elm_layout_add(win);
   snprintf(buf, sizeof(buf), "%s/themes/equate.edj", elm_app_data_dir_get());
   elm_layout_file_set(ly, buf, "Main");
   evas_object_size_hint_weight_set(ly, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ly, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, ly);
   evas_object_show(ly);

  ecore_event_handler_add (ECORE_EVENT_KEY_DOWN, _key_down_cb, NULL);

   evas_object_show(win);
   evas_object_resize(win, 240, 320);

   _edje_callbacks_define(elm_layout_edje_get(ly));
}

EAPI_MAIN int
elm_main(int argc, char **argv)
{

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   elm_app_compile_bin_dir_set(PACKAGE_BIN_DIR);
   elm_app_compile_data_dir_set(PACKAGE_DATA_DIR);
   elm_app_info_set(elm_main, "equate", "images/equate.png");

   math_init();
   _create_gui();
   elm_run();
   elm_shutdown();

   return 0;
}
#endif
ELM_MAIN()
