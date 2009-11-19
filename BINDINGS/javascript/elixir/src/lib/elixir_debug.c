#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>

#include "Elixir.h"
#include "elixir_private.h"

static FILE*    display = NULL;

void
slprintf(char *str, size_t size, const char *format, ...)
{
   va_list      ap;

   va_start(ap, format);

   vsnprintf(str, size, format, ap);
   str[size - 1] = 0;

   va_end(ap);
}

static void
elixir_debug_print_void(const char* fmt, ...)
{
   (void) fmt;
}

void
(*elixir_debug_print)(const char* fmt, ...) = elixir_debug_print_void;

static void
elixir_debug_print_console(const char* fmt, ...)
{
   FILE*        f;
   va_list      ap;

   va_start(ap, fmt);

   f = fopen("/dev/console", "w");
   if (f)
     {
        vfprintf(f, fmt, ap);
        fprintf(f, "\n");
     }

   va_end(ap);

   fclose(f);
}

static void
elixir_debug_print_syslog(const char* fmt, ...)
{
   va_list      ap;
   int          len = strlen(fmt) + 5;
   char         format[len];

   snprintf(format, len, "js: %s", fmt);

   va_start(ap, fmt);

   vsyslog(LOG_INFO, format, ap);

   va_end(ap);
}

static void
elixir_debug_print_file(const char* fmt, ...)
{
   va_list      ap;

   va_start(ap, fmt);

   vfprintf(display, fmt, ap);
   fprintf(display, "\n");

   va_end(ap);
}

void
elixir_debug_print_switch(Elixir_Debug_Mode mode, void* data)
{
   display = NULL;
   switch (mode)
     {
     case ELIXIR_DEBUG_FILE:
        display = data;
        elixir_debug_print = elixir_debug_print_file;
        break;
     case ELIXIR_DEBUG_STDERR:
        display = stderr;
        elixir_debug_print = elixir_debug_print_file;
        break;
     case ELIXIR_DEBUG_STDOUT:
        display = stdout;
        elixir_debug_print = elixir_debug_print_file;
        break;
     case ELIXIR_DEBUG_CONSOLE:
        elixir_debug_print = elixir_debug_print_console;
        break;
     case ELIXIR_DEBUG_SYSLOG:
        elixir_debug_print = elixir_debug_print_syslog;
        break;
     case ELIXIR_DEBUG_NONE:
     default:
        elixir_debug_print = elixir_debug_print_void;
        break;
     }
}
