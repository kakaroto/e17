#include <stdarg.h>
#include <Entranced.h>

/* extern Entranced_Config config; */

void entranced_debug (char *fmt, ...)
{
/*   if (config.debuglevel > 0) */
#ifdef DEBUG
   if (ENTRANCED_DEBUG)
   {
      va_list args;
      va_start (args, fmt);
      vprintf(fmt, args);
      va_end(args);
      fflush(stdout);
   }
#endif
}

