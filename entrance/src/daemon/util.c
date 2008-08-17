#include <stdarg.h>
#include <Entranced.h>

/* extern Entranced_Config config; */

int entranced_debug_flag = 0;

void
entranced_debug(char *fmt, ...)
{
/*   if (config.debuglevel > 0) */
#ifdef ENTRANCED_DEBUG
   if (entranced_debug_flag)
   {
      va_list args;

      va_start(args, fmt);
      vprintf(fmt, args);
      va_end(args);
      fflush(stdout);
   }
#endif
}
