/*
 * calculator-edje
 *
 * Copyright (C) 2010, ProFUSION Embedded Systems
 *
 * License BSD, see COPYING file at project folder.
 *
 * @author Raphael Kubo da Costa <kubo@profusion.mobi>
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <Elementary.h>
#include <limits.h>

typedef enum
{
   CALCULATOR_READ_OP1,
   CALCULATOR_READ_OP2
} Calculator_State;

typedef enum
{
   CALCULATOR_OP_SUM,
   CALCULATOR_OP_SUB,
   CALCULATOR_OP_MUL,
   CALCULATOR_OP_DIV
} Calculator_Operation;

typedef struct
{
   Evas_Object *win;
   Evas_Object *bg;

   Calculator_State state;
   Calculator_Operation operation;
   int op1;
   int op2;
} App;

static int _log_domain = -1;
#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

static void
on_win_del_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_exit();
}

static void
on_number_button_clicked(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source)
{
   App *app = (App*)data;
   unsigned long long result;
   int *operand;
   char str_operand[20];

   char number = source[0] - 48;

   if (app->state == CALCULATOR_READ_OP1)
     operand = &(app->op1);
   else if (app->state == CALCULATOR_READ_OP2)
     operand = &(app->op2);

   result = ((unsigned long long)*operand * 10) + number;

   if (result > INT_MAX)
     {
	DBG("%llu is too big to fit in an integer", result);
	return;
     }

   *operand = (int)result;

   snprintf(str_operand, 20, "%d", *operand);
   edje_object_part_text_set(obj, "NumberEntry", str_operand);
}

static void
on_operation_button_clicked(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source)
{
   App *app = (App*)data;

   if (app->state != CALCULATOR_READ_OP1)
     return;

   if (!strcmp(source, "OperationPlusButton"))
     app->operation = CALCULATOR_OP_SUM;
   else if (!strcmp(source, "OperationMinusButton"))
     app->operation = CALCULATOR_OP_SUB;
   else if (!strcmp(source, "OperationTimesButton"))
     app->operation = CALCULATOR_OP_MUL;
   else if (!strcmp(source, "OperationDividedButton"))
     app->operation = CALCULATOR_OP_DIV;

   app->state = CALCULATOR_READ_OP2;
   edje_object_part_text_set(obj, "NumberEntry", "0");
}

static void
on_clear_button_clicked(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   App *app = (App*)data;

   app->state = CALCULATOR_READ_OP1;
   app->op1 = 0;
   app->op2 = 0;

   edje_object_part_text_set(obj, "NumberEntry", "0");
}

static void
on_result_button_clicked(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   App *app = (App*)data;

   if (app->state != CALCULATOR_READ_OP2)
     return;

   long long int result;
   char str_result[20];

   switch (app->operation)
     {
      case CALCULATOR_OP_SUM:
	 result = (long long)app->op1 + app->op2;
	 break;
      case CALCULATOR_OP_SUB:
	 result = (long long)app->op1 - app->op2;
	 break;
      case CALCULATOR_OP_MUL:
	 result = (long long)app->op1 * app->op2;
	 break;
      case CALCULATOR_OP_DIV:
	 result = (long long)app->op1 / app->op2;
	 break;
     }

   app->op1 = 0;
   app->op2 = 0;
   app->state = CALCULATOR_READ_OP1;

   if (result < INT_MIN || result > INT_MAX)
     {
	DBG("%lld does not fit in an integer.", result);
	edje_object_part_text_set(obj, "NumberEntry", "ERROR!");
	return;
     }

   app->op1 = (int)result;
   snprintf(str_result, 20, "%d", app->op1);
   edje_object_part_text_set(obj, "NumberEntry", str_result);
}

static Eina_Bool
create_main_win(App *app)
{
   app->win = elm_win_add(NULL, "calculator-edje", ELM_WIN_BASIC);
   if (!app->win)
     {
	CRITICAL("Could not create window.");
	return EINA_FALSE;
     }
   elm_win_title_set(app->win, "Edje Calculator");
   evas_object_smart_callback_add
     (app->win, "delete-request", on_win_del_req, NULL);

   app->bg = elm_bg_add(app->win);
   if (!app->bg)
     {
	CRITICAL("Could not create background.");
	return EINA_FALSE;
     }
   evas_object_size_hint_weight_set(app->bg, 1.0, 1.0);
   elm_win_resize_object_add(app->win, app->bg);
   evas_object_show(app->bg);

   Evas_Object *layout = elm_layout_add(app->win);
   elm_win_resize_object_add(app->win, layout);
   elm_layout_file_set(layout, PACKAGE_DATA_DIR "/calculator-ui.edj", "main");
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(layout);

   Evas_Object *layout_edje = elm_layout_edje_get(layout);
   edje_object_signal_callback_add(layout_edje, "mouse,clicked,1", "[0-9]Button", on_number_button_clicked, app);
   edje_object_signal_callback_add(layout_edje, "mouse,clicked,1", "Operation*Button", on_operation_button_clicked, app);
   edje_object_signal_callback_add(layout_edje, "mouse,clicked,1", "ClearButton", on_clear_button_clicked, app);
   edje_object_signal_callback_add(layout_edje, "mouse,clicked,1", "EqualButton", on_result_button_clicked, app);

   evas_object_resize(app->win, 320, 240);
   evas_object_show(app->win);

   return EINA_TRUE;
}

EAPI int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   App app;
   int r = 0;

   _log_domain = eina_log_domain_register("calculator-edje", NULL);
   if (_log_domain < 0)
     {
	EINA_LOG_CRIT("could not create log domain 'calculator-edje'.");
	return -1;
     }

   memset(&app, 0, sizeof(app));
   app.state = CALCULATOR_READ_OP1;
   app.operation = CALCULATOR_OP_SUM;
   app.op1 = 0;
   app.op2 = 0;
   if (!create_main_win(&app))
     {
	r = -1;
	goto end;
     }

   elm_run();
 end:
   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;
   elm_shutdown();
   return r;
}
ELM_MAIN()

