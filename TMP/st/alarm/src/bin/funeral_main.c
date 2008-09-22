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
#include <linux/rtc.h>

#define QUEUE_DIR "/var/spool/funeral"

#define SERVICE_NAME  "org.enlightenment.Funeral"
#define SERVICE_PATH  "/"
#define SERVICE_IFACE "Core"

static DBusMessage *request_method_List(E_DBus_Object *obj, DBusMessage *msg);
static DBusMessage *request_method_Add(E_DBus_Object *obj, DBusMessage *msg);
static DBusMessage *request_method_Del(E_DBus_Object *obj, DBusMessage *msg);
static void request_service(void *data, DBusMessage *msg, DBusError *err);

static void job_load(void);
static void job_save(void);
static dbus_uint32_t *job_list(void);
static dbus_uint32_t job_add(dbus_uint64_t time_at, dbus_uint32_t priority, const char *flags, const char *command);
static void job_del(dbus_uint32_t id);

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
	     dbus_message_iter_append_basic(&item, DBUS_TYPE_INT32, &(ids[i]));
	     dbus_message_iter_close_container(&array, &item);
	  }
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
   if (id > 0) job_save();
   
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &id);
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
   job_save();
   
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
   
   // FIXME: dbus signals - emit when things go off?
   
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
static void
job_load(void)
{
   // FIXME: load job queue (QUEUE_DIR - files)
}

static void
job_save(void)
{
   // FIXME: save job queue (write to QUEUE_DIR)
}

static dbus_uint32_t *
job_list(void)
{
   dbus_uint32_t *ids;
   
   // FIXME: list job queue
   ids = malloc(1 * sizeof(dbus_uint32_t));
   ids[0] = 0;
   return ids;
}

static dbus_uint32_t
job_add(dbus_uint64_t time_at, dbus_uint32_t priority, const char *flags, const char *command)
{
   // FIXME: add to queue
   return 0;
}

static void
job_del(dbus_uint32_t id)
{
   // FIXME: del from queue
   return;
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

/// Main code - init
int
main(int argc, char **argv)
{
   E_DBus_Connection *c;
   
   ecore_init();
   ecore_string_init();
   ecore_app_args_set(argc, (const char **)argv);
   e_dbus_init();
   
   c = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (!c)
     {
	fprintf(stderr, "ERROR: can't connect to system session\n");
	exit(-1);
     }
   
   declare_interface(c);

   ecore_main_loop_begin();
    
   e_dbus_connection_close(c);
   e_dbus_shutdown();
   ecore_string_shutdown();
   ecore_shutdown();
   
   return 0;
}
