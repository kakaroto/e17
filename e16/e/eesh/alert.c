
/*
 Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies of the Software, its documentation and marketing & publicity
 materials, and acknowledgment shall be given in the documentation, materials
 and software packages that this Software was used.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
Alert(char *fmt, ...)
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
   TitleText = Estrdup(text);
}

void
AssignIgnoreText(char *text)
{
   if (IgnoreText)
      Efree(IgnoreText);
   IgnoreText = NULL;
   IgnoreText = Estrdup(text);
}

void
AssignRestartText(char *text)
{
   if (RestartText)
      Efree(RestartText);
   RestartText = NULL;
   RestartText = Estrdup(text);
}

void
AssignExitText(char *text)
{
   if (ExitText)
      Efree(ExitText);
   ExitText = NULL;
   ExitText = Estrdup(text);
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
