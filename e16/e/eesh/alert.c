#include "E.h"

static void         ShowAlert(char *text);

static int          (*IgnoreFunction) (void *) = NULL;
static void        *IgnoreParams = NULL;
static char        *IgnoreText = NULL;
static int          (*RestartFunction) (void *) = NULL;
static void        *RestartParams = NULL;
static char        *RestartText = NULL;
static int          (*ExitFunction) (void *) = NULL;
static void        *ExitParams = NULL;
static char        *ExitText = NULL;

static char        *TitleText = NULL;

int                 call_level;
int                 debug_level;

void
Alert(char *fmt,...)
{
   char                text[10240];
   va_list             ap;

   EDBUG(7, "Alert");
   va_start(ap, fmt);
#ifdef __USE_GNU
   vsnprintf(text, 10240, fmt, ap);
#else
   vsprintf(text, fmt, ap);
#endif
   va_end(ap);
   ShowAlert(text);
   EDBUG_RETURN_;
}

void
InitStringList(void)
{
   char               *debug_str;

   call_level = 0;
   debug_level = 0;
   debug_str = getenv("EDBUG");
   if (debug_str)
      debug_level = atoi(debug_str);
}

void
AssignTitleText(char *text)
{
   if (TitleText)
      Efree(TitleText);
   TitleText = NULL;
   TitleText = duplicate(text);
}

void
AssignIgnoreText(char *text)
{
   if (IgnoreText)
      Efree(IgnoreText);
   IgnoreText = NULL;
   IgnoreText = duplicate(text);
}

void
AssignRestartText(char *text)
{
   if (RestartText)
      Efree(RestartText);
   RestartText = NULL;
   RestartText = duplicate(text);
}

void
AssignExitText(char *text)
{
   if (ExitText)
      Efree(ExitText);
   ExitText = NULL;
   ExitText = duplicate(text);
}

void
AssignIgnoreFunction(int (*FunctionToAssign) (void *), void *params)
{
   IgnoreFunction = FunctionToAssign;
   IgnoreParams = params;
}

void
AssignRestartFunction(int (*FunctionToAssign) (void *), void *params)
{
   RestartFunction = FunctionToAssign;
   RestartParams = params;
}

void
AssignExitFunction(int (*FunctionToAssign) (void *), void *params)
{
   ExitFunction = FunctionToAssign;
   ExitParams = params;
}

static void
ShowAlert(char *text)
{
   EDBUG(8, "ShowAlert");
   if (!text)
      EDBUG_RETURN_;
   fprintf(stderr, text);
   fflush(stderr);
   EDBUG_RETURN_;
}
