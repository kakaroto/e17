#include "config.h"
#include <Ecore.h>
#include <Ecore_File.h>
#include <E_DBus.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define SERVICE_NAME  "org.enlightenment.Waker"
#define SERVICE_PATH  "/"
#define SERVICE_IFACE "Core"

extern char **environ;
static E_DBus_Connection *conn = NULL;

static void *
add_unmarhsall(DBusMessage *msg, DBusError *err)
{
   DBusMessageIter array, iter, item;
   dbus_uint32_t val;

   if (dbus_message_get_args(msg, NULL, DBUS_TYPE_UINT32, &val, DBUS_TYPE_INVALID))
     {  
	printf("%i\n", val);
     }
   return NULL;
}

static void *
del_unmarhsall(DBusMessage *msg, DBusError *err)
{
   return NULL;
}

static void *
list_unmarhsall(DBusMessage *msg, DBusError *err)
{
   DBusMessageIter array, iter, item;
   dbus_uint32_t val;
   
   if (dbus_message_iter_init(msg, &array))
     {  
	if (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_ARRAY)
	  {  
	     dbus_message_iter_recurse(&array, &iter);
	     do
	       {
		  val = 0;
		  dbus_message_iter_recurse(&iter, &item);
		  dbus_message_iter_get_basic(&item, &val);
		  if (val > 0) printf("%i\n", val);
	       }
	     while (dbus_message_iter_next(&iter));
	  }
     }
   return NULL;
}

static void
ret_cb(void *data, void *ret, DBusError *err)
{
   ecore_main_loop_quit();
}

static void
ret_free(void *data)
{ 
}

/// Main code - init
int
main(int argc, char **argv)
{
   E_DBus_Connection *c;      

   if (argc <= 1)
     {
	printf("Usage:\n"
	       "  waker list\n"
	       "  waker del JOB_ID\n"
	       "  waker add TIMESPEC PRIORITY FLAGS\n"
	       "\n"
	       "JOB_ID is the number of the job returned from add or in\n"
	       "list. TIMESPEC is YYYY-MM-DD HH:MM:SS or HH:MM:SS\n"
	       "and FLAGS right now just should be 'X'\n"
	       "\n"
	       "add takes the command from standard input.\n");
	exit(-1);
     }
   
   ecore_init();
   ecore_string_init();
   ecore_app_args_set(argc, (const char **)argv);
   e_dbus_init();
   
   c = e_dbus_bus_get(DBUS_BUS_SESSION);
   if (!c)
     {
	fprintf(stderr, "ERROR: can't connect to session dbus\n");
	exit(-1);
     }

   conn = c;
   
   if (!strcmp(argv[1], "add"))
     {
	DBusMessage *msg;
	DBusMessageIter iter;
	dbus_uint32_t priority;
	dbus_uint64_t time_at;
	char buf[4096];
	char *flags;
	char *command;
	char *shell;
	struct tm tm, *tm_now;
	time_t t, t_now;
	size_t num, size, alloc_size;
	FILE *tf;
	
	if (argc < 5)
	  {
	     fprintf(stderr, "ERROR: not enough parameters\n");
	     exit(-1);
	  }
	t_now = time(NULL);
	tm_now = localtime(&t_now);
	if (!strptime(argv[2], "%Y-%m-%d %H:%M:%S", &tm))
	  {
	     if (!strptime(argv[2], "%H:%M:%S", &tm))
	       {
		  fprintf(stderr, "ERROR: can't parse TIMESPEC # %s\n", argv[2]);
		  exit(-1);
	       }
	     tm.tm_year = tm_now->tm_year;
	     tm.tm_mon = tm_now->tm_mon;
	     tm.tm_mday = tm_now->tm_mday;
	     tm.tm_yday = tm_now->tm_yday;
	     tm.tm_wday = tm_now->tm_wday;
	     tm.tm_isdst = tm_now->tm_isdst;
	     t = mktime(&tm);
	     if (t <= t_now) t += (24 * 60 * 60);
	  }
	else
	  {
	     t = mktime(&tm);
	     if (t < t_now) return;
	  }
	time_at = t;
	if (sscanf(argv[3], "%u", &priority) != 1)
	  {
	     fprintf(stderr, "ERROR: can't parse ID # %s\n", argv[3]);
	     exit(-1);
	  }
	flags = argv[4];
	tf = tmpfile();
	if (!tf)
	  {
	     fprintf(stderr, "ERROR: can't open tempfile\n");
	     exit(-1);
	  }
	shell = getenv("SHELL");
	if (!shell) shell = "/bin/sh";
	fprintf(tf, "#!%s\n", shell);
	if (environ)
	  {
	     char **p;
	     
	     for (p = environ; *p; p++)
	       {
		  char *env, *val, *ep;
		  int do_export = 1;
		  
		  env = *p;
		  val = strchr(env, '=');
		  if (!val) val = env;
		  fwrite(env, 1, val - env, tf);
		  for (ep = val; *ep; ep++)
		    {
		       if (*ep == '\n') fprintf(tf, "\"\n\"");
		       else
			 {
			    if (!isalnum(*ep))
			      {
				 switch (*ep)
				   {
				    case '%':
				    case '/':
				    case '{':
				    case '[':
				    case ']':
				    case '=':
				    case '}':
				    case '@':
				    case '+':
				    case '#':
				    case ',':
				    case '.':
				    case ':':
				    case '-':
				    case '_':
				    case ';':
				      break;
				    default:
				      fputc('\\', tf);
				      break;
				   }
			      }
			    fputc(*ep, tf);
			 }
		    }
		  fputs("; export ", tf);
		  fwrite(env, 1, val - env, tf);
		  fputc('\n', tf);
	       }
	  }
	while ((num = fread(buf, 1, sizeof(buf), stdin)) > 0)
	  fwrite(buf, 1, num, tf);
	
	alloc_size = 4096;
	command = malloc(alloc_size);
	size = 0;
	rewind(tf);
	while ((num = fread(command + size, 1, 4096, tf)) > 0)
	  {
	     size += num;
	     command[size] = 0;
	     if (size == 4096)
	       {
		  char *tmp;
		  
		  alloc_size += 4096;
		  tmp = realloc(command, alloc_size);
		  if (!tmp)
		    {
		       fprintf(stderr, "ERROR: cannot alloc memory for command buffer\n");
		       exit(-1);
		    }
		  command = tmp;
	       }
	  }
	msg = dbus_message_new_method_call(SERVICE_NAME, SERVICE_PATH,
					   SERVICE_NAME"."SERVICE_IFACE,
					   "Add");
	if (msg)
	  {
	     dbus_message_iter_init_append(msg, &iter);
	     dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT64, &time_at);
	     dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &priority);
	     dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &flags);
	     dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &command);
	     e_dbus_method_call_send(conn, msg, add_unmarhsall, ret_cb, ret_free, -1, NULL);
	  }
     }
   else if (!strcmp(argv[1], "del"))
     {
	DBusMessage *msg;
	DBusMessageIter iter;
	dbus_uint32_t id;
	
	if (argc < 3)
	  {
	     fprintf(stderr, "ERROR: not enough parameters\n");
	     exit(-1);
	  }
	if (sscanf(argv[2], "%u", &id) != 1)
	  {
	     fprintf(stderr, "ERROR: can't parse ID # %s\n", argv[2]);
	     exit(-1);
	  }
	msg = dbus_message_new_method_call(SERVICE_NAME, SERVICE_PATH,
					   SERVICE_NAME"."SERVICE_IFACE,
					   "Del");
	if (msg)
	  {
	     dbus_message_iter_init_append(msg, &iter);
	     dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &id);
	     e_dbus_method_call_send(conn, msg, del_unmarhsall, ret_cb, ret_free, -1, NULL);
	  }
     }
   else if (!strcmp(argv[1], "list"))
     {
	DBusMessage *msg;
	
	msg = dbus_message_new_method_call(SERVICE_NAME, SERVICE_PATH,
					   SERVICE_NAME"."SERVICE_IFACE,
					   "List");
	if (msg)
	  e_dbus_method_call_send(conn, msg, list_unmarhsall, ret_cb, ret_free, -1, NULL);
     }
   else
     {
	fprintf(stderr, "ERROR: unknown waker operation\n");
	exit(-1);
     }
   
   ecore_main_loop_begin();
   
   e_dbus_connection_close(c);
   e_dbus_shutdown();
   ecore_string_shutdown();
   ecore_shutdown();
   
   return 0;
}
