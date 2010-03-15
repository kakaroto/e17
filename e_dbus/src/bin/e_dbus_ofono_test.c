#include "E_Ofono.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static void
_method_success_check(void *data, DBusMessage *msg, DBusError *error)
{
   const char *name = data;

   if ((!error) || (!dbus_error_is_set(error)))
     {
	printf("SUCCESS: method %s() finished successfully.\n", name);
	return;
     }

   printf("FAILURE: method %s() finished with error: %s %s\n",
	  name, error->name, error->message);
   dbus_error_free(error);
}

static void
_elements_print(E_Ofono_Element **elements, unsigned int count)
{
   unsigned int i;
   for (i = 0; i < count; i++)
     {
	printf("--- element %d:\n", i);
	e_ofono_element_print(stdout, elements[i]);
     }
   free(elements);
   printf("END: all elements count = %u\n", count);
}

static int
_on_element_add(void *data, int type, void *info)
{
   E_Ofono_Element *element = info;
   printf(">>> %s %s\n", element->path, element->interface);
   return 1;
}

static int
_on_element_del(void *data, int type, void *info)
{
   E_Ofono_Element *element = info;
   printf("<<< %s %s\n", element->path, element->interface);
   return 1;
}

static int
_on_element_updated(void *data, int type, void *info)
{
   E_Ofono_Element *element = info;
   printf("!!! %s %s\n", element->path, element->interface);
   e_ofono_element_print(stderr, element);
   return 1;
}
static int
_on_cmd_quit(char *cmd, char *args)
{
   fputs("Bye!\n", stderr);
   ecore_main_loop_quit();
   return 0;
}

static int
_on_cmd_sync(char *cmd, char *args)
{
   e_ofono_manager_sync_elements();
   return 1;
}

static char *
_tok(char *p)
{
   p = strchr(p, ' ');
   if (!p)
     return NULL;

   *p = '\0';
   p++;
   while (isspace(*p))
     p++;
   if (*p == '\0')
     return NULL;

   return p;
}

static int
_on_cmd_get_all(char *cmd, char *args)
{
   E_Ofono_Element **elements;
   char *type;
   unsigned int count;
   bool ret;

   if (!args)
     type = NULL;
   else
     type = args;

   if (type)
     ret = e_ofono_elements_get_all_type(type, &count, &elements);
   else
     ret = e_ofono_elements_get_all(&count, &elements);

   if (!ret)
     fputs("ERROR: could not get elements\n", stderr);
   else
     {
	printf("BEG: all elements type=%s count = %d\n", type, count);
	_elements_print(elements, count);
     }

   return 1;
}

static E_Ofono_Element *
_element_from_args(char *interface, char *args, char **next_args)
{
   E_Ofono_Element *element;

   if (!args)
     {
	fputs("ERROR: missing element path\n", stderr);
	*next_args = NULL;
	return NULL;
     }

   if (!interface)
     {
	interface = _tok(args);
	*next_args = _tok(interface);
     }
   else
     *next_args = _tok(args);

   element = e_ofono_element_get(args, interface);
   if (!element)
     fprintf(stderr, "ERROR: no element called \"%s %s\".\n", args, interface);

   return element;
}

static int
_on_cmd_print(char *cmd, char *args)
{
   char *next_args;
   E_Ofono_Element *element = _element_from_args(NULL, args, &next_args);
   if (element)
     e_ofono_element_print(stdout, element);
   return 1;
}

static int
_on_cmd_get_properties(char *cmd, char *args)
{
   char *next_args;
   E_Ofono_Element *element = _element_from_args(NULL, args, &next_args);
   if (element)
     e_ofono_element_properties_sync(element);
   return 1;
}

static int
_on_cmd_property_set(char *cmd, char *args)
{
   char *next_args, *name, *p;
   E_Ofono_Element *element = _element_from_args(NULL, args, &next_args);
   void *value;
   long vlong;
   unsigned short vu16;
   unsigned int vu32;
   int type;

   if (!element)
     return 1;

   if (!next_args)
     {
	fputs("ERROR: missing parameters name, type and value.\n", stderr);
	return 1;
     }

   name = next_args;
   p = _tok(name);
   if (!p)
     {
	fputs("ERROR: missing parameters type and value.\n", stderr);
	return 1;
     }

   next_args = _tok(p);
   if (!next_args)
     {
	fputs("ERROR: missing parameter value.\n", stderr);
	return 1;
     }

   type = p[0];
   switch (type)
     {
      case DBUS_TYPE_BOOLEAN:
	 vlong = !!atol(next_args);
	 value = &vlong;
	 fprintf(stderr, "DBG: boolean is: %ld\n", vlong);
	 break;
      case DBUS_TYPE_UINT16:
	 vu16 = strtol(next_args, &p, 0);
	 if (p == next_args)
	   {
	      fprintf(stderr, "ERROR: invalid number \"%s\".\n", next_args);
	      return 1;
	   }
	 value = &vu16;
	 fprintf(stderr, "DBG: u16 is: %hu\n", vu16);
	 break;
      case DBUS_TYPE_UINT32:
	 vu32 = strtol(next_args, &p, 0);
	 if (p == next_args)
	   {
	      fprintf(stderr, "ERROR: invalid number \"%s\".\n", next_args);
	      return 1;
	   }
	 value = &vu32;
	 fprintf(stderr, "DBG: u16 is: %u\n", vu32);
	 break;
      case DBUS_TYPE_STRING:
      case DBUS_TYPE_OBJECT_PATH:
	 p = next_args + strlen(next_args);
	 if (p > next_args)
	   p--;
	 while (p > next_args && isspace(*p))
	   p--;
	 if (p <= next_args)
	   {
	      fprintf(stderr, "ERROR: invalid string \"%s\".\n", next_args);
	   }
	 p[1] = '\0';
	 value = next_args;
	 fprintf(stderr, "DBG: string is: \"%s\"\n", next_args);
	 break;
      default:
	 fprintf(stderr, "ERROR: don't know how to parse type '%c' (%d)\n",
		 type, type);
	 return 1;
     }

   fprintf(stderr, "set_property %s [%p] %s %c %p...\n",
	   args, element, name, type, value);
   if (!e_ofono_element_property_set(element, name, type, value))
	fputs("ERROR: error setting property.\n", stderr);

   return 1;
}

/* Manager Commands */

static int
_on_cmd_manager_get(char *cmd, char *args)
{
   E_Ofono_Element *element;
   element = e_ofono_manager_get();
   e_ofono_element_print(stderr, element);
   return 1;
}

static int
_on_cmd_manager_modems_get(char *cmd, char *args)
{
   char *path;
   Eina_Array_Iterator iterator;
   unsigned int i;
   Eina_Array *modems = eina_array_new(1);

   if(e_ofono_manager_modems_get(&modems))
     {
	printf("[");
	EINA_ARRAY_ITER_NEXT(modems, i, path, iterator)
	   printf(" %s", path);
	printf(" ]\n");
     }

   return 1;
}

/* Modem Commands */

static int
_on_cmd_modem_set_powered(char *cmd, char *args)
{
   char *next_args;
   Eina_Bool powered;
   E_Ofono_Element *element = _element_from_args("org.ofono.Modem", args, &next_args);

   if (!element)
	   return 1;

   if (!args)
     {
	fputs("ERROR: missing the powered value\n", stderr);
	return 1;
     }

   powered = !!atol(next_args);

   if (e_ofono_modem_powered_set
       (element, powered, _method_success_check, "modem_set_powered"))
     printf(":::Modem %s Powered set to %hhu\n", element->path, powered);
   else
     fputs("ERROR: can't set Modem Powered\n", stderr);
   return 1;
}

static int
_on_input(void *data, Ecore_Fd_Handler *fd_handler)
{
   char buf[256];
   char *cmd, *args;
   const struct {
      const char *cmd;
      int (*cb)(char *cmd, char *args);
   } *itr, maps[] = {
     {"quit", _on_cmd_quit},
     {"sync", _on_cmd_sync},
     {"get_all", _on_cmd_get_all},
     {"print", _on_cmd_print},
     {"get_properties", _on_cmd_get_properties},
     {"set_property", _on_cmd_property_set},
     {"manager_get", _on_cmd_manager_get},
     {"manager_modems_get", _on_cmd_manager_modems_get},
     {"modem_set_powered", _on_cmd_modem_set_powered},
     {NULL, NULL}
   };


   if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_ERROR))
     {
	fputs("ERROR: reading from stdin, exit\n", stderr);
	return 0;
     }

   if (!ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_READ))
     {
	fputs("ERROR: nothing to read?\n", stderr);
	return 0;
     }

   if (!fgets(buf, sizeof(buf), stdin))
     {
	fprintf(stderr, "ERROR: could not read command: %s\n", strerror(errno));
	ecore_main_loop_quit();
	return 0;
     }

   cmd = buf;
   while (isspace(*cmd))
     cmd++;

   args = strchr(cmd, ' ');
   if (args)
     {
	char *p;

	*args = '\0';
	args++;

	while (isspace(*args))
	  args++;

	p = args + strlen(args) - 1;
	if (*p == '\n')
	  *p = '\0';
     }
   else
     {
	char *p;

	p = cmd + strlen(cmd) - 1;
	if (*p == '\n')
	  *p = '\0';
     }

   if (strcmp(cmd, "help") == 0)
     {
	if (args)
	  {
	     printf("Commands with '%s' in the name:\n", args);
	     for (itr = maps; itr->cmd != NULL; itr++)
	       if (strstr(itr->cmd, args))
		 printf("\t%s\n", itr->cmd);
	  }
	else
	  {
	     fputs("Commands:\n", stdout);
	     for (itr = maps; itr->cmd != NULL; itr++)
	       printf("\t%s\n", itr->cmd);
	  }
	fputc('\n', stdout);
	return 1;
     }

   for (itr = maps; itr->cmd != NULL; itr++)
     if (strcmp(itr->cmd, cmd) == 0)
       return itr->cb(cmd, args);

   printf("unknown command \"%s\", args=%s\n", cmd, args);
   return 1;
}

int
main(int argc, char *argv[])
{
   E_DBus_Connection *c;

   ecore_init();
   e_dbus_init();
   eina_init();

   c = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (!c) {
      printf("ERROR: can't connect to system session\n");
      return -1;
   }

   e_ofono_system_init(c);

   ecore_event_handler_add(E_OFONO_EVENT_ELEMENT_ADD, _on_element_add, NULL);
   ecore_event_handler_add(E_OFONO_EVENT_ELEMENT_DEL, _on_element_del, NULL);
   ecore_event_handler_add(E_OFONO_EVENT_ELEMENT_UPDATED,
			   _on_element_updated, NULL);

   ecore_main_fd_handler_add
     (0, ECORE_FD_READ | ECORE_FD_ERROR, _on_input, NULL, NULL, NULL);

   ecore_main_loop_begin();

   e_ofono_system_shutdown();

   e_dbus_connection_close(c);
   eina_shutdown();
   e_dbus_shutdown();
   ecore_shutdown();

   fputs("DBG: clean exit.\n", stderr);

   return 0;
}
