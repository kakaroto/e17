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
#include <sys/stat.h>
#include <linux/rtc.h>

#define SERVICE_NAME  "org.enlightenment.Waker"
#define SERVICE_PATH  "/"
#define SERVICE_IFACE "Core"

static DBusMessage *request_method_List(E_DBus_Object *obj, DBusMessage *msg);
static DBusMessage *request_method_Add(E_DBus_Object *obj, DBusMessage *msg);
static DBusMessage *request_method_Del(E_DBus_Object *obj, DBusMessage *msg);
static void request_service(void *data, DBusMessage *msg, DBusError *err);

static void job_load(void);
static dbus_uint32_t *job_list(void);
static dbus_uint32_t job_add(dbus_uint64_t time_at, dbus_uint32_t priority, const char *flags, const char *command);
static void job_del(dbus_uint32_t id);

static void rtc_clear(void);
static void rtc_set(dbus_uint64_t time_at);

static int timer_callback(void *data);
static void timer_eval(void);

//// DBUS stuff
static E_DBus_Connection *conn = NULL;
static E_DBus_Object *obj = NULL;

static DBusMessage *
request_method_List(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessage *reply;
   DBusMessageIter iter, array, item;
   dbus_uint32_t *ids;
   int i;
   
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "(u)", &array);

   ids = job_list();
   if (ids)
     {
	for (i = 0; ids[i] > 0; i++)
	  {
	     dbus_message_iter_open_container(&array, DBUS_TYPE_STRUCT, NULL, &item);
	     dbus_message_iter_append_basic(&item, DBUS_TYPE_UINT32, &(ids[i]));
	     dbus_message_iter_close_container(&array, &item);
	  }
	dbus_message_iter_open_container(&array, DBUS_TYPE_STRUCT, NULL, &item);
	dbus_message_iter_append_basic(&item, DBUS_TYPE_UINT32, &(ids[i]));
	dbus_message_iter_close_container(&array, &item);
	free(ids);
     }
   
   dbus_message_iter_close_container(&iter, &array);
   return reply;
}

static DBusMessage *
request_method_Add(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   dbus_uint64_t time_at;
   dbus_uint32_t priority;
   char *flags, *command;
   dbus_uint32_t id;
   
   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &time_at);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &priority);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &flags);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &command);

   id = job_add(time_at, priority, flags, command);
   
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &id);
   return reply;
}

static DBusMessage *
request_method_Del(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   dbus_uint32_t id;

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &id);
   
   job_del(id);
   
   return dbus_message_new_method_return(msg);
}

static void
request_service(void *data, DBusMessage *msg, DBusError *err)
{
   E_DBus_Interface *iface;
   
   if (dbus_error_is_set(err))
     {
	fprintf(stderr, "%s\n", err->message);
	exit(-1);
     }
   
   obj = e_dbus_object_add(conn, SERVICE_PATH, NULL);
   iface = e_dbus_interface_new(SERVICE_NAME"."SERVICE_IFACE);
   e_dbus_interface_method_add(iface, "List", "", "a(u)",
			       request_method_List);
   e_dbus_interface_method_add(iface, "Add", "tuss", "u", 
			       request_method_Add);
   e_dbus_interface_method_add(iface, "Del", "u", "",
			       request_method_Del);
   
   e_dbus_object_interface_attach(obj, iface);

   job_load();
}

static void
declare_interface(E_DBus_Connection *c)
{
   conn = c;
   e_dbus_request_name(conn, SERVICE_NAME, 0, request_service, NULL);
}

//// JOB queue
typedef struct _Job Job;

struct _Job
{
   dbus_uint32_t id;
   dbus_uint64_t time_at;
   dbus_uint32_t priority;
   char *flags;
   Ecore_Exe *exe;
   Job *next;
};

static dbus_uint32_t job_max_id = 0;
static Job *jobs = NULL;

static void
job_load(void)
{
   const char *home;
   char buf[4096];
   Ecore_List *files;
   
   home = getenv("HOME");
   if (!home) home = "/";
   snprintf(buf, sizeof(buf), "%s/.waker/spool", home);
   ecore_file_mkpath(buf);
   files = ecore_file_ls(buf);
   job_max_id = 0;
   if (files)
     {
	char *file;
	
	ecore_list_first_goto(files);
	while ((file = ecore_list_current(files)))
	  {
	     Job *j, *jj, *jp;
	     char *newstr, *p;
	     
	     newstr = strdup(file);
	     if (newstr)
	       {
		  p = newstr;
		  while (*p)
		    {
		       if (*p == '_') *p = ' ';
		       p++;
		    }
		  j = calloc(1, sizeof(Job));
		  // filename job_ID_TIME_PRI_FLAGS
		  if (j)
		    {
		       char tmp[64];
		       
		       tmp[0] = 0;
		       sscanf(newstr, "job %u %llx %u %60s",
			      &(j->id), &(j->time_at),
			      &(j->priority), tmp);
		       j->flags = strdup(tmp);
		       if (j->id > job_max_id) job_max_id = j->id;
		       if (!jobs) jobs = j;
		       else
			 {
			    for (jp = NULL, jj = jobs; 
				 jj; 
				 jp = jj, jj = jj->next)
			      {
				 if (jj->time_at > j->time_at)
				   {
				      if (jp) jp->next = j;
				      else jobs = j;
				      j->next = jj;
				      break;
				   }
				 else if (!jj->next)
				   {
				      jj->next = j;
				      break;
				   }
			      }
			 }
		    }
		  free(newstr);
	       }
             ecore_list_next(files);
	  }
        ecore_list_destroy(files);
     }
   timer_eval();
}

static dbus_uint32_t *
job_list(void)
{
   dbus_uint32_t *ids;
   Job *j;
   int num = 0;
   
   for (j = jobs; j; j = j->next) num++;
   ids = calloc(num + 1, sizeof(dbus_uint32_t));
   for (num = 0, j = jobs; j; j = j->next, num++)
     {
	printf("add %p->%i\n", j, j->id);
	ids[num] = j->id;
     }
   ids[num] = 0;
   return ids;
}

static dbus_uint32_t
job_add(dbus_uint64_t time_at, dbus_uint32_t priority, const char *flags, const char *command)
{
   const char *home;
   char buf[4096];
   dbus_uint32_t id;
   FILE *f;
   Job *j, *jj, *jp;
   
   home = getenv("HOME");
   if (!home) home = "/";
   snprintf(buf, sizeof(buf), "%s/.waker/spool", home);
   ecore_file_mkpath(buf);
   j = calloc(1, sizeof(Job));
   if (!j) return 0;
   id = job_max_id + 1;
   snprintf(buf, sizeof(buf), 
	    "%s/.waker/spool/job_%u_%016llx_%u_%s", 
	    home, id, time_at, priority, flags);
   f = fopen(buf, "w");
   if (f)
     {
	fprintf(f, "%s\n", command);
	fclose(f);
	chmod(buf, S_IXUSR | S_IWUSR | S_IRUSR);
     }
   else
     {
	free(j);
	return 0;
     }
   
   j->id = id;
   j->time_at = time_at;
   j->priority = priority;
   j->flags = strdup(flags);
   if (!jobs) jobs = j;
   else
     {
	for (jp = NULL, jj = jobs; 
	     jj; 
	     jp = jj, jj = jj->next)
	  {
	     if (jj->time_at > j->time_at)
	       {
		  if (jp) jp->next = j;
		  else jobs = j;
		  j->next = jj;
		  break;
	       }
	     else if (!jj->next)
	       {
		  jj->next = j;
		  break;
	       }
	  }
     }
   job_max_id = id;
   timer_eval();
   return id;
}

static void
job_del(dbus_uint32_t id)
{
   const char *home;
   char buf[4096];
   FILE *f;
   Job *jj, *jp;
   
   home = getenv("HOME");
   if (!home) home = "/";
   snprintf(buf, sizeof(buf), "%s/.waker/spool", home);
   ecore_file_mkpath(buf);
   for (jp = NULL, jj = jobs; 
	jj; 
	jp = jj, jj = jj->next)
     {
	if (jj->id == id)
	  {
	     snprintf(buf, sizeof(buf), 
		      "%s/.waker/spool/job_%u_%016llx_%u_%s", 
		      home, jj->id, jj->time_at, jj->priority, jj->flags);
	     unlink(buf);
	     if (jp) jp->next = jj->next;
	     else jobs = jj->next;
	     free(jj->flags);
	     free(jj);
	     break;
	  }
     }
   if (!jobs) job_max_id = 0;
   timer_eval();
}

//// RTC settings
static void
rtc_clear(void)
{
   struct rtc_wkalrm alarm;
   int res, fd;
   time_t t;

   // open rtc
   fd = open("/dev/rtc", O_RDWR);
   if (fd < 0) return;
   // get alarm info
   res = ioctl(fd, RTC_WKALM_RD, &alarm);
   if (res < 0) {
      close(fd);
      return;
   }
   // disable
   alarm.enabled = 0;
   // set the disabled state
   res = ioctl(fd, RTC_WKALM_SET, &alarm);
   if (res < 0) {
      close(fd);
      return;
   }
   close(fd);
}

static void
rtc_set(dbus_uint64_t time_at)
{
   struct rtc_wkalrm alarm;
   struct tm tm, *tmp;
   int res, fd;
   struct rtc_time rtct;
   time_t t, t2;
   
   // open rtc
   fd = open("/dev/rtc", O_RDWR);
   if (fd < 0) return;
   // in case timezone change and get the time
   tzset();
   t = time(NULL);
   // if the time to tick off is less than or euqal to current time - dont do
   if (time_at <= t)
     {
	close(fd);
	return;
     }
   // the new time to tick off as a delta of the current time
   t = time_at - t;
   if (t <= 0)
     {
	close(fd);
	return;
     }
   // get the current rtc time - whatever world it lives in
   res = ioctl(fd, RTC_RD_TIME, &rtct);
   if (res < 0) {
      close(fd);
      return;
   }
   memset(&tm, 0, sizeof(tm));
   tm.tm_sec = rtct.tm_sec;
   tm.tm_min = rtct.tm_min;
   tm.tm_hour = rtct.tm_hour;
   tm.tm_mday = rtct.tm_mday;
   tm.tm_mon = rtct.tm_mon;
   tm.tm_year = rtct.tm_year;
   t2 = mktime(&tm);
   // add the delta to the current rtc
   t2 += t;
   tmp = localtime(&t);
   if (!tmp) {
      close(fd);
      return;
   }
   // get any alarm info
   res = ioctl(fd, RTC_WKALM_RD, &alarm);
   if (res < 0) {
      close(fd);
      return;
   }
   // set the alarm time with delta added
   alarm.time.tm_sec = tmp->tm_sec;
   alarm.time.tm_min = tmp->tm_min;
   alarm.time.tm_hour = tmp->tm_hour;
   alarm.time.tm_mday = tmp->tm_mday;
   alarm.time.tm_mon = tmp->tm_mon;
   alarm.time.tm_year = tmp->tm_year;
   alarm.enabled = 1;
   res = ioctl(fd, RTC_WKALM_SET, &alarm);
   if (res < 0) {
      close(fd);
      return;
   }
   close(fd);
}

//// Timer/timeouts and updates to check for sysclock changes
static Ecore_Timer *ticker = NULL;

static int
timer_setup(void *data)
{
   timer_eval();
   return 0;
}

static int
timer_callback(void *data)
{
   ticker = NULL;
   timer_eval();
   return 0;
}

static int
child_exit(void *data, int type, Ecore_Exe_Event_Del *event)
{
   if (event->exe)
     {
	Job *j;

	j = ecore_exe_data_get(event->exe);
	if (j)
	  {
	     j->exe = NULL;
	     j = ecore_exe_data_get(event->exe);
	     job_del(j->id);
	  }
     }
   return 1;
}

static void
timer_eval(void)
{
   double t, t_in;
   dbus_uint64_t next_at;
   Job *j;
   
   printf("timer_eval()\n");
   if (!jobs)
     {
	printf("no jobs. clear\n");
	rtc_clear();
	if (ticker) ecore_timer_del(ticker);
	ticker = NULL;
	return;
     }
   
   if (ticker) ecore_timer_del(ticker);
   t = ecore_time_get();
   t_in = 1.0 - (t - ((dbus_uint64_t)t));
   printf("tick in %3.3f\n", t_in);
   ticker = ecore_timer_add(t_in, timer_callback, NULL);
   next_at = 0;
   for (j = jobs; j; j = j->next)
     {
	if (!j->exe)
	  {
	     next_at = j->time_at;
	     break;
	  }
     }
   if ((next_at <= t) && (j))
     {
	char buf[4096];
	const char *home;
	
	home = getenv("HOME");
	if (!home) home = "/";
	snprintf(buf, sizeof(buf), 
		 "%s/.waker/spool/job_%u_%016llx_%u_%s", 
		 home, j->id, j->time_at, j->priority, j->flags);
	j->exe = ecore_exe_run(buf, j);
	printf("RUN: %u @ %llu\n", j->id, j->time_at - (dbus_uint64_t)t);
     }
   else if (j)
     {
	printf("next job in %llu\n", next_at - (dbus_uint64_t)t);
	rtc_set(next_at);
	printf("rtc done\n");
     }
}

/// Main code - init
int
main(int argc, char **argv)
{
   E_DBus_Connection *c;
   
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

   ecore_event_handler_add(ECORE_EXE_EVENT_DEL, child_exit, NULL);
   declare_interface(c);

   ecore_main_loop_begin();
    
   e_dbus_connection_close(c);
   e_dbus_shutdown();
   ecore_string_shutdown();
   ecore_shutdown();
   
   return 0;
}
