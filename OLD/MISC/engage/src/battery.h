#ifndef BATTERY_H
#define BATTERY_H

#include <Ecore.h>
#include <Evas.h>

int           _battery_cb_check(void *data);

typedef struct _Battery      Battery;
typedef struct _Status       Status;

#define CHECK_NONE 0
#define CHECK_LINUX_ACPI 1
#define CHECK_LINUX_APM 2

struct _Battery
{
  Evas_Object         *object;
  int alarm_triggered;

  int                  battery_check_mode;
  Ecore_Timer         *battery_check_timer;
  int                  battery_prev_drain;
  int                  battery_prev_ac;
  int                  battery_prev_battery;
};

#define BATTERY_STATE_NONE 0
#define BATTERY_STATE_CHARGING 1
#define BATTERY_STATE_DISCHARGING 2

struct _Status
{
   /* Low battery */
   unsigned char alarm;
   /* Is there a battery? */
   unsigned char has_battery;
   /* charging, discharging, none */
   unsigned char state;
   /* Battery level */
   double level;
   /* Text */
   /* reading == % left */
   char *reading;
   /* time == time left to empty / full */
   char *time;
};

#endif

