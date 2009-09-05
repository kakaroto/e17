#include "shelm.h"
#include "shelm_getopt.h"

// add values into struct got from ecore_getopt.
static Eina_Bool
_args_init(int argc, char **argv)
{
  int arg_index;
  int i;
  arguments = calloc(1, sizeof(Arguments_struct));

  // where to save stuff from shelm_getopt.h?
  Ecore_Getopt_Value general_values[] = {
    ECORE_GETOPT_VALUE_STR(arguments->window_title),
    ECORE_GETOPT_VALUE_STR(arguments->window_text),
    ECORE_GETOPT_VALUE_INT(arguments->window_width),
    ECORE_GETOPT_VALUE_INT(arguments->window_height),
    ECORE_GETOPT_VALUE_STR(arguments->window_bg),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_entry_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_error_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_warning_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_info_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_textinfo_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_list_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_question_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_clock_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->dlg_scale_enabled),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_NONE
  };

  Ecore_Getopt_Value entry_values[] = {
    ECORE_GETOPT_VALUE_STR(arguments->window_title),
    ECORE_GETOPT_VALUE_STR(arguments->window_text),
    ECORE_GETOPT_VALUE_INT(arguments->window_width),
    ECORE_GETOPT_VALUE_INT(arguments->window_height),
    ECORE_GETOPT_VALUE_STR(arguments->window_bg),
    ECORE_GETOPT_VALUE_STR(arguments->entry_entry_text),
    ECORE_GETOPT_VALUE_BOOL(arguments->entry_hide_text),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_NONE
  };

  Ecore_Getopt_Value textinfo_values[] = {
    ECORE_GETOPT_VALUE_STR(arguments->window_title),
    ECORE_GETOPT_VALUE_STR(arguments->window_text),
    ECORE_GETOPT_VALUE_INT(arguments->window_width),
    ECORE_GETOPT_VALUE_INT(arguments->window_height),
    ECORE_GETOPT_VALUE_STR(arguments->window_bg),
    ECORE_GETOPT_VALUE_STR(arguments->textinfo_filename),
    ECORE_GETOPT_VALUE_BOOL(arguments->textinfo_editable),
    ECORE_GETOPT_VALUE_BOOL(arguments->textinfo_nowrap),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_NONE
  };

  Ecore_Getopt_Value clock_values[] = {
    ECORE_GETOPT_VALUE_STR(arguments->window_title),
    ECORE_GETOPT_VALUE_STR(arguments->window_text),
    ECORE_GETOPT_VALUE_INT(arguments->window_width),
    ECORE_GETOPT_VALUE_INT(arguments->window_height),
    ECORE_GETOPT_VALUE_STR(arguments->window_bg),
    ECORE_GETOPT_VALUE_BOOL(arguments->clock_show_seconds),
    ECORE_GETOPT_VALUE_BOOL(arguments->clock_show_am_pm),
    ECORE_GETOPT_VALUE_STR(arguments->clock_time),
    ECORE_GETOPT_VALUE_BOOL(arguments->clock_editable),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_NONE
  };

  Ecore_Getopt_Value scale_values[] = {
    ECORE_GETOPT_VALUE_STR(arguments->window_title),
    ECORE_GETOPT_VALUE_STR(arguments->window_text),
    ECORE_GETOPT_VALUE_INT(arguments->window_width),
    ECORE_GETOPT_VALUE_INT(arguments->window_height),
    ECORE_GETOPT_VALUE_STR(arguments->window_bg),
    ECORE_GETOPT_VALUE_DOUBLE(arguments->scale_value),
    ECORE_GETOPT_VALUE_DOUBLE(arguments->scale_min_value),
    ECORE_GETOPT_VALUE_DOUBLE(arguments->scale_max_value),
    ECORE_GETOPT_VALUE_DOUBLE(arguments->scale_step),
    ECORE_GETOPT_VALUE_BOOL(arguments->scale_print_partial),
    ECORE_GETOPT_VALUE_BOOL(arguments->scale_hide_value),
    ECORE_GETOPT_VALUE_BOOL(arguments->scale_inverted),
    ECORE_GETOPT_VALUE_STR(arguments->scale_unit_format),
    ECORE_GETOPT_VALUE_STR(arguments->scale_label),
    ECORE_GETOPT_VALUE_STR(arguments->scale_icon),
    ECORE_GETOPT_VALUE_BOOL(arguments->scale_vertical),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_BOOL(arguments->quit_bool),
    ECORE_GETOPT_VALUE_NONE
  };

  // parse arguments with ecore_getopt
  if (argv[1])
    {
      if (!strcmp(argv[1], "--entry"))
	{
	  arguments->dlg_entry_enabled = EINA_TRUE;
	  for (i; i < argc + 1; i++)
	    {
	      argv[i] = argv[i + 1];
	      argv[argc] = NULL;
	    }
	  argc--;
	  arg_index = ecore_getopt_parse(&entry_opts, entry_values, argc, argv);
	}
      else if (!strcmp(argv[1], "--text-info"))
	{
	  arguments->dlg_entry_enabled = EINA_TRUE;
	  for (i; i < argc + 1; i++)
	    {
	      argv[i] = argv[i + 1];
	      argv[argc] = NULL;
	    }
	  argc--;
	  arg_index = ecore_getopt_parse(&textinfo_opts, textinfo_values, argc, argv);
	}
      else if (!strcmp(argv[1], "--clock"))
	{
	  arguments->dlg_entry_enabled = EINA_TRUE;
	  for (i; i < argc + 1; i++)
	    {
	      argv[i] = argv[i + 1];
	      argv[argc] = NULL;
	    }
	  argc--;
	  arg_index = ecore_getopt_parse(&clock_opts, clock_values, argc, argv);
	}
      else if (!strcmp(argv[1], "--scale"))
	{
	  arguments->dlg_entry_enabled = EINA_TRUE;
	  for (i; i < argc + 1; i++)
	    {
	      argv[i] = argv[i + 1];
	      argv[argc] = NULL;
	    }
	  argc--;
	  arg_index = ecore_getopt_parse(&scale_opts, scale_values, argc, argv);
	}
      else
        arg_index = ecore_getopt_parse(&general_opts, general_values, argc, argv);
    }
  else
    arg_index = ecore_getopt_parse(&general_opts, general_values, argc, argv);
  // exit when something bad happens
  if (arg_index < 0)
    return EINA_FALSE;

  if (arguments->quit_bool)
   exit(0);

  return EINA_TRUE;
}

#ifndef ELM_LIB_QUICKLAUNCH
EAPI int
elm_main(int argc, char **argv)
{
  if (!_args_init(argc, argv))
    {
      fprintf(stderr, "Cannot parse arguments, exiting.");
      return 1;
    }

  if (arguments->dlg_entry_enabled)
    shelm_entry_dialog(arguments->window_title, arguments->window_text, arguments->window_width, arguments->window_height, arguments->window_bg, arguments->entry_entry_text, arguments->entry_hide_text);
  else if (arguments->dlg_error_enabled)
    shelm_simple_dialog(arguments->window_title, arguments->window_text, arguments->window_width, arguments->window_height, arguments->window_bg, "errordialog", _("Error!"), "/usr/share/shellementary/icon-error.png");
  else if (arguments->dlg_warning_enabled)
    shelm_simple_dialog(arguments->window_title, arguments->window_text, arguments->window_width, arguments->window_height, arguments->window_bg, "warningdialog", _("Warning!"), "/usr/share/shellementary/icon-warning.png");
  else if (arguments->dlg_info_enabled)
    shelm_simple_dialog(arguments->window_title, arguments->window_text, arguments->window_width, arguments->window_height, arguments->window_bg, "infodialog", _("Information"), "/usr/share/shellementary/icon-info.png");
  else
    shelm_about_dialog();

  elm_run();
  elm_shutdown();

  if (arguments->quit_cancel_bool)
    return 1;
  else
    return 0; 
}
#endif
ELM_MAIN()
