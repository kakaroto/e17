#ifndef SHELLEMENTARY_GETOPT_H
#define SHELLEMENTARY_GETOPT_H

#include "shelm.h"

// general options
const Ecore_Getopt general_opts = {
  "shellementary",
  NULL,
  PACKAGE_VERSION,
  "Copyright (c) 2011 Shellementary developers",
  "Licensed under the MIT license",
  "Zenity replacement written in Elementary.\n"
     "\n"
     "OPTIONS FOR ALL DIALOGS:\n"
     " These are options which you can use for all dialogs.\n"
     " You can show dialog-specific options by using:.\n"
     " shellementary --dialogtype --help (available for entry, textinfo, clock, scale)\n",
  0,
  {
     ECORE_GETOPT_STORE_STR
     ('t', "title", "Set window title."),
     ECORE_GETOPT_STORE_STR
     ('T', "text", "Set window text."),
     ECORE_GETOPT_STORE_INT
     ('W', "width", "Set window width in pixels."),
     ECORE_GETOPT_STORE_INT
     ('H', "height", "Set window height in pixels."),
     ECORE_GETOPT_STORE_STR
     ('b', "window-bg", "Set image as window background."),
     ECORE_GETOPT_COUNT('\0', "entry", "Show entry dialog."),
     ECORE_GETOPT_COUNT('\0', "error", "Show error dialog."),
     ECORE_GETOPT_COUNT('\0', "warning", "Show warning dialog."),
     ECORE_GETOPT_COUNT('\0', "info", "Show information dialog."),
     ECORE_GETOPT_COUNT('\0', "text-info", "Show text information dialog."),
     ECORE_GETOPT_COUNT('\0', "list", "Show list dialog."),
     ECORE_GETOPT_COUNT('\0', "question", "Show question dialog."),
     ECORE_GETOPT_COUNT('\0', "clock", "Show clock dialog."),
     ECORE_GETOPT_COUNT('\0', "scale", "Show scale dialog."),
     ECORE_GETOPT_LICENSE('L', "license"),
     ECORE_GETOPT_COPYRIGHT('C', "copyright"),
     ECORE_GETOPT_VERSION('V', "version"),
     ECORE_GETOPT_HELP('h', "help"),
     ECORE_GETOPT_SENTINEL
  }
};

// entry options
const Ecore_Getopt entry_opts = {
  "shellementary",
  NULL,
  PACKAGE_VERSION,
  "Copyright (c) 2011 Shellementary developers",
  "Licensed under the MIT license",
  "ENTRY DIALOG ARGUMENTS\n"
  " Arguments which can be used only with entry dialog.\n",
  0,
  {
     ECORE_GETOPT_STORE_STR
     ('t', "title", "Set window title."),
     ECORE_GETOPT_STORE_STR
     ('T', "text", "Set window text."),
     ECORE_GETOPT_STORE_INT
     ('W', "width", "Set window width in pixels."),
     ECORE_GETOPT_STORE_INT
     ('H', "height", "Set window height in pixels."),
     ECORE_GETOPT_STORE_STR
     ('b', "window-bg", "Set image as window background."),
     ECORE_GETOPT_STORE_STR
     ('\0', "entry-text", "Set entry text"),
     ECORE_GETOPT_COUNT('\0', "hide-text", "Password-like entry"),
     ECORE_GETOPT_LICENSE('L', "license"),
     ECORE_GETOPT_COPYRIGHT('C', "copyright"),
     ECORE_GETOPT_VERSION('V', "version"),
     ECORE_GETOPT_HELP('h', "help"),
     ECORE_GETOPT_SENTINEL
  }
};

// textinfo options
const Ecore_Getopt textinfo_opts = {
  "shellementary",
  NULL,
  PACKAGE_VERSION,
  "Copyright (c) 2011 Shellementary developers",
  "Licensed under the MIT license",
  "TEXT INFO DIALOG ARGUMENTS\n"
  " Arguments which can be used only with text info dialog.\n",
  0,
  {
     ECORE_GETOPT_STORE_STR
     ('t', "title", "Set window title."),
     ECORE_GETOPT_STORE_STR
     ('T', "text", "Set window text."),
     ECORE_GETOPT_STORE_INT
     ('W', "width", "Set window width in pixels."),
     ECORE_GETOPT_STORE_INT
     ('H', "height", "Set window height in pixels."),
     ECORE_GETOPT_STORE_STR
     ('b', "window-bg", "Set image as window background."),
     ECORE_GETOPT_STORE_STR
     ('\0', "filename", "Set file to show in text info, otherwise it reads from stdin."),
     ECORE_GETOPT_COUNT('\0', "editable", "Set it editable."),
     ECORE_GETOPT_COUNT('\0', "no-wrap", "Disable text wrapping."),
     ECORE_GETOPT_LICENSE('L', "license"),
     ECORE_GETOPT_COPYRIGHT('C', "copyright"),
     ECORE_GETOPT_VERSION('V', "version"),
     ECORE_GETOPT_HELP('h', "help"),
     ECORE_GETOPT_SENTINEL
  }
};

// clock options
const Ecore_Getopt clock_opts = {
  "shellementary",
  NULL,
  PACKAGE_VERSION,
  "Copyright (c) 2011 Shellementary developers",
  "Licensed under the MIT license",
  "CLOCK DIALOG ARGUMENTS\n"
  " Arguments which can be used only with clock dialog.\n",
  0,
  {
     ECORE_GETOPT_STORE_STR
     ('t', "title", "Set window title."),
     ECORE_GETOPT_STORE_STR
     ('T', "text", "Set window text."),
     ECORE_GETOPT_STORE_INT
     ('W', "width", "Set window width in pixels."),
     ECORE_GETOPT_STORE_INT
     ('H', "height", "Set window height in pixels."),
     ECORE_GETOPT_STORE_STR
     ('b', "window-bg", "Set image as window background."),
     ECORE_GETOPT_COUNT('\0', "show-seconds", "Show seconds in clock."),
     ECORE_GETOPT_COUNT('\0', "show-am-pm", "Show AM/PM switch."),
     ECORE_GETOPT_STORE_STR
     ('\0', "time", "Set time in format \"HOURS, MINUTES, SECONDS\". Works in combination with --clock-editable."),
     ECORE_GETOPT_COUNT('\0', "clock-editable", "Set it editable."),
     ECORE_GETOPT_LICENSE('L', "license"),
     ECORE_GETOPT_COPYRIGHT('C', "copyright"),
     ECORE_GETOPT_VERSION('V', "version"),
     ECORE_GETOPT_HELP('h', "help"),
     ECORE_GETOPT_SENTINEL
  }
};

// scale options
const Ecore_Getopt scale_opts = {
  "shellementary",
  NULL,
  PACKAGE_VERSION,
  "Copyright (c) 2011 Shellementary developers",
  "Licensed under the MIT license",
  "SCALE DIALOG ARGUMENTS\n"
  " Arguments which can be used only with scale dialog.\n",
  0,
  {
     ECORE_GETOPT_STORE_STR
     ('t', "title", "Set window title."),
     ECORE_GETOPT_STORE_STR
     ('T', "text", "Set window text."),
     ECORE_GETOPT_STORE_INT
     ('W', "width", "Set window width in pixels."),
     ECORE_GETOPT_STORE_INT
     ('H', "height", "Set window height in pixels."),
     ECORE_GETOPT_STORE_STR
     ('b', "window-bg", "Set image as window background."),
     ECORE_GETOPT_STORE_DOUBLE
     ('\0', "value", "Set slider value."),
     ECORE_GETOPT_STORE_DOUBLE
     ('\0', "min-value", "Set slider minimal value."),
     ECORE_GETOPT_STORE_DOUBLE
     ('\0', "max-value", "Set slider maximal value."),
     ECORE_GETOPT_STORE_STR
     ('\0', "step", "Set slider step."),
     ECORE_GETOPT_COUNT('\0', "print-partial", "Print partial values."),
     ECORE_GETOPT_COUNT('\0', "hide-value", "Hide value."),
     ECORE_GETOPT_COUNT('\0', "slider-inverted", "If set, slider will be inverted."),
     ECORE_GETOPT_STORE_STR
     ('\0', "unit-format", "Set slider unit format - for example \"value: %0.1f\", that will print value: 156.1 or %0.2f will print 156.12, %0.3f 156.125 etc."),
     ECORE_GETOPT_STORE_STR
     ('\0', "slider-label", "Set slider label."),
     ECORE_GETOPT_STORE_STR
     ('\0', "slider-icon", "Set slider icon."),
     ECORE_GETOPT_COUNT('\0', "slider-vertical", "If set, slider will be vertical."),
     ECORE_GETOPT_LICENSE('L', "license"),
     ECORE_GETOPT_COPYRIGHT('C', "copyright"),
     ECORE_GETOPT_VERSION('V', "version"),
     ECORE_GETOPT_HELP('h', "help"),
     ECORE_GETOPT_SENTINEL
  }
};

#endif
