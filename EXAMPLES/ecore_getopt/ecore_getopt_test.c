/* gcc -o ecore_getopt_test ecore_getopt_test.c `pkg-config  --cflags --libs eina ecore` */

#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Getopt.h>

static unsigned char _callback(const Ecore_Getopt *parser, const Ecore_Getopt_Desc *desc, const char *str, void *data, Ecore_Getopt_Value *storage);

static const Ecore_Getopt optdesc = {
  "ecore_getopt_test",
  NULL,
  "0.1",
  "(C) 2011 Enlightenment",
  "DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE\n"
  "   Version 2, May 2011\n\n"
  "Everyone is permitted to copy and distribute verbatim or modified\n"
  "copies of this license document, and changing it is allowed as long\n"
  "as the name is changed.\n\n"
  "DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE\n"
  "TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n\n"
  "0. You just DO WHAT THE FUCK YOU WANT TO.",
  "ecore_getopt_test is a test program for ecore_getopt.\n"
  " It allows to list and manipulate objects.\n",
  0,
  {
    ECORE_GETOPT_STORE_INT('i', "int", "Specify an int value."),
    ECORE_GETOPT_STORE_STR('s', "string", "Specify a string."),
    ECORE_GETOPT_STORE_BOOL('b', "bool", "Specify a bool."),
    ECORE_GETOPT_APPEND_METAVAR('l', "list", "Specify a list of strings, use me multiple times", "STRING", ECORE_GETOPT_TYPE_STR),
    ECORE_GETOPT_STORE_TRUE('o', "option", "Specify this if you want to check param."),
    ECORE_GETOPT_CALLBACK_ARGS('c', "callback", "Calls a callback if this arg is specified, receives a string", "STRING", _callback, NULL),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

int main(int argc, char **argv)
{
   Eina_Bool opt_bool = EINA_FALSE;
   Eina_Bool opt_true = EINA_FALSE;
   Eina_Bool opt_quit = EINA_FALSE;
   int opt_int = 0;
   char *opt_str = NULL;
   char *opt_list_str = NULL;
   Eina_List *opt_list = NULL;

   eina_init();
   ecore_init();

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_INT(opt_int),
     ECORE_GETOPT_VALUE_STR(opt_str),
     ECORE_GETOPT_VALUE_BOOL(opt_bool),
     ECORE_GETOPT_VALUE_LIST(opt_list),
     ECORE_GETOPT_VALUE_BOOL(opt_true),
     ECORE_GETOPT_VALUE_BOOL(opt_quit),
     ECORE_GETOPT_VALUE_BOOL(opt_quit),
     ECORE_GETOPT_VALUE_BOOL(opt_quit),
     ECORE_GETOPT_VALUE_BOOL(opt_quit),
     ECORE_GETOPT_VALUE_NONE
   };

   if (ecore_getopt_parse(&optdesc, values, argc, argv) < 0)
     {
        fprintf(stderr, "Failed to parse args\n");
        return 1;
     }

   if (opt_quit)
     return 0;

   printf("Args result :\n");
   printf("\tINT\t= %d\n", opt_int);
   printf("\tSTRING\t= %s\n", opt_str);
   printf("\tBOOL\t= %d\n", opt_bool);
   printf("\tList\t=");

   EINA_LIST_FREE(opt_list, opt_list_str)
     {
        printf(" %s", opt_list_str);
        free(opt_list_str);
     }

   printf("\n");
   printf("\tOPTION\t= %d\n", opt_true);

   return 0;
}

static unsigned char
_callback(const Ecore_Getopt *parser, const Ecore_Getopt_Desc *desc, const char *str, void *data, Ecore_Getopt_Value *storage)
{
   printf("Callback received %s\n", str);

   return 0;
}
