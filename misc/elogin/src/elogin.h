#ifndef HAVE_ELOGIN_H
#define HAVE_ELOGIN_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <Evas.h>
#include <Ecore.h>


typedef struct _Elogin_Greeter Elogin_Greeter;

struct _Elogin_Greeter
{
   Evas_Object greet_win;
   Evas_Object user_input;
   Evas_Object pass_input;
   int w, h;
};

#endif
