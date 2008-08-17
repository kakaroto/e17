#include "battery.h"
#include <Edje.h>
#include <Ecore_File.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Status       *_battery_linux_acpi_check(Battery *bat);
static Status       *_battery_linux_apm_check(Battery *bat);
static void          _battery_level_set(Battery *bat, double level);

int _battery_cb_check(void* data)
{
  Status *ret = NULL;

  Battery* bat = (Battery*)data;

  if (bat->battery_check_mode == 0)
    {
      if (ecore_file_is_dir("/proc/acpi"))
	bat->battery_check_mode = CHECK_LINUX_ACPI;
      else if (ecore_file_exists("/proc/apm"))
	bat->battery_check_mode = CHECK_LINUX_APM;
    }

  switch (bat->battery_check_mode)
    {
    case CHECK_LINUX_ACPI:
      ret = _battery_linux_acpi_check(bat);
      break;
    case CHECK_LINUX_APM:
      ret = _battery_linux_apm_check(bat);
      break;
    default:
      break;
    }
  if (ret)
    {
      if (ret->has_battery) {
	if (ret->state == BATTERY_STATE_CHARGING) {
	  edje_object_signal_emit(bat->object, "charge", "");
	  edje_object_signal_emit(bat->object, "pulsestop", "");
	  edje_object_part_text_set(bat->object, "reading", ret->reading);
	  edje_object_part_text_set(bat->object, "time", ret->time);
	  _battery_level_set(bat, ret->level);
	  bat->battery_prev_ac = 1;
	}
	else if (ret->state == BATTERY_STATE_DISCHARGING) {
	  if (bat->battery_prev_ac != 0)
	    edje_object_signal_emit(bat->object, "discharge", "");
	  if (ret->alarm) {
	    if (!bat->alarm_triggered) {
	      //e_error_dialog_show("Battery Running Low",
	      //		  "Your battery is running low.\n"
	      //		  "You may wish to switch to an AC source.");
	    }
	    edje_object_signal_emit(bat->object, "pulse", "");
	  }
	  edje_object_part_text_set(bat->object, "reading", ret->reading);
	  edje_object_part_text_set(bat->object, "time", ret->time);
	  _battery_level_set(bat, ret->level);


	  bat->battery_prev_ac = 0;
	  if (ret->alarm)
	    bat->alarm_triggered = 1;
	}
	else {
	  /* ret->state == BATTERY_STATE_NONE */
	  if (bat->battery_prev_ac != 1)
	    edje_object_signal_emit(bat->object, "charge", "");
	  if (bat->battery_prev_battery == 0)
	    edje_object_signal_emit(bat->object, "charge", "");
	  edje_object_part_text_set(bat->object, "reading", ret->reading);
	  edje_object_part_text_set(bat->object, "time", ret->time);
	  _battery_level_set(bat, ret->level);


	  bat->battery_prev_ac = 1;
	  bat->battery_prev_battery = 1;
	}
      }
      else {
	/* Hasn't battery */
	if (bat->battery_prev_battery != 0)
	  edje_object_signal_emit(bat->object, "unknown", "");
	edje_object_part_text_set(bat->object, "reading", ret->reading);
	edje_object_part_text_set(bat->object, "time", ret->time);
	_battery_level_set(bat, ret->level);
	
	bat->battery_prev_battery = 0;
      }
      free(ret->reading);
      free(ret->time);
      free(ret);
    }
  else {
    /* Error reading status */
    if (bat->battery_prev_battery != -2)
      edje_object_signal_emit(bat->object, "unknown", "");
    edje_object_part_text_set(bat->object, "reading", "NO INFO");
    edje_object_part_text_set(bat->object, "time", "--:--");
    _battery_level_set(bat, (double)(rand() & 0xff) / 255.0);

    bat->battery_prev_battery = -2;
    bat->battery_check_mode = CHECK_NONE;
  }
  return 1;
}

static Status *
_battery_linux_acpi_check(Battery *ef)
{
  Ecore_List *bats;
  char buf[4096], buf2[4096];
  char *name;

  int bat_max = 0;
  int bat_filled = 0;
  int bat_level = 0;
  int bat_drain = 1;

  int bat_val = 0;

  int discharging = 0;
  int charging = 0;
  int battery = 0;

  int design_cap_unknown = 0;
  int last_full_unknown = 0;
  int rate_unknown = 0;
  int level_unknown = 0;

  int hours, minutes;

  Status *stat;
  
  //stat = E_NEW(Status, 1);
  stat = malloc(sizeof(Status));
  memset(stat,0,sizeof(Status));

  //printf("doing acpi\n");

  /* Read some information on first run. */
  bats = ecore_file_ls("/proc/acpi/battery");
  while ((name = ecore_list_next(bats)))
    {
      FILE *f;

      if ((!strcmp(name, ".")) || (!strcmp(name, "..")))
	  continue;

      snprintf(buf, sizeof(buf), "/proc/acpi/battery/%s/info", name);
      f = fopen(buf, "r");
      if (f)
	{
	  int design_cap = 0;
	  int last_full = 0;

	  fgets(buf2, sizeof(buf2), f); buf2[sizeof(buf2) - 1] = 0;
	  fgets(buf2, sizeof(buf2), f); buf2[sizeof(buf2) - 1] = 0;
	  sscanf(buf2, "%*[^:]: %250s %*s", buf);
	  if (!strcmp(buf, "unknown")) design_cap_unknown = 1;
	  else sscanf(buf2, "%*[^:]: %i %*s", &design_cap);
	  fgets(buf2, sizeof(buf2), f); buf2[sizeof(buf2) - 1] = 0;
	  sscanf(buf2, "%*[^:]: %250s %*s", buf);
	  if (!strcmp(buf, "unknown")) last_full_unknown = 1;
	  else sscanf(buf2, "%*[^:]: %i %*s", &last_full);
	  fclose(f);
	  bat_max += design_cap;
	  bat_filled += last_full;
	}
      snprintf(buf, sizeof(buf), "/proc/acpi/battery/%s/state", name);
      f = fopen(buf, "r");
      if (f)
	{
	  char present[256];
	  char capacity_state[256];
	  char charging_state[256];
	  int rate = 1;
	  int level = 0;

	  fgets(buf2, sizeof(buf2), f); buf2[sizeof(buf2) - 1] = 0;
	  sscanf(buf2, "%*[^:]: %250s", present);
	  fgets(buf2, sizeof(buf2), f); buf2[sizeof(buf2) - 1] = 0;
	  sscanf(buf2, "%*[^:]: %250s", capacity_state);
	  fgets(buf2, sizeof(buf2), f); buf2[sizeof(buf2) - 1] = 0;
	  sscanf(buf2, "%*[^:]: %250s", charging_state);
	  fgets(buf2, sizeof(buf2), f); buf2[sizeof(buf2) - 1] = 0;
	  sscanf(buf2, "%*[^:]: %250s %*s", buf);
	  if (!strcmp(buf, "unknown")) rate_unknown = 1;
	  else sscanf(buf2, "%*[^:]: %i %*s", &rate);
	  fgets(buf2, sizeof(buf2), f); buf2[sizeof(buf2) - 1] = 0;
	  sscanf(buf2, "%*[^:]: %250s %*s", buf);
	  if (!strcmp(buf, "unknown")) level_unknown = 1;
	  else sscanf(buf2, "%*[^:]: %i %*s", &level);
	  fclose(f);
	  if (!strcmp(present, "yes")) battery++;
	  if (!strcmp(charging_state, "discharging")) discharging++;
	  if (!strcmp(charging_state, "charging")) charging++;
	  if (!strcmp(charging_state, "charged")) rate_unknown = 0;
	  bat_drain += rate;
	  bat_level += level;
	}
      ecore_list_destroy(bats);
    }

  if (ef->battery_prev_drain < 1) ef->battery_prev_drain = 1;
  if (bat_drain < 1) bat_drain = ef->battery_prev_drain;
  ef->battery_prev_drain = bat_drain;

  if (bat_filled > 0) bat_val = (100 * bat_level) / bat_filled;
  else bat_val = 100;

  if (discharging) minutes = (60 * bat_level) / bat_drain;
  else
    {
      if (bat_filled > 0)
	minutes = (60 * (bat_filled - bat_level)) / bat_drain;
      else
	minutes = 0;
    }
  hours = minutes / 60;
  minutes -= (hours * 60);

  if (hours < 0) hours = 0;
  if (minutes < 0) minutes = 0;

  if (!battery)
    {
      stat->has_battery = 0;
      stat->state = BATTERY_STATE_NONE;
      stat->reading = strdup("NO BAT");
      stat->time = strdup("--:--");
      stat->level = 1.0;
    }
  else if ((charging) || (discharging))
    {
      ef->battery_prev_battery = 1;
      stat->has_battery = 1;
      if (charging)
	{
	  stat->state = BATTERY_STATE_CHARGING;
	  ef->alarm_triggered = 0;
	}
      else if (discharging)
	{
	  stat->state = BATTERY_STATE_DISCHARGING;
	  if (stat->level < 0.1)
	    {
	      if (((hours * 60) + minutes) <= 10)
		stat->alarm = 1;
	    }
	}
      if (level_unknown)
	{
	  stat->reading = strdup("BAD DRIVER");
	  stat->time = strdup("--:--");
	  stat->level = 0.0;
	}
      else if (rate_unknown)
	{
	  snprintf(buf, sizeof(buf), "%i%%", bat_val);
	  stat->reading = strdup(buf);
	  stat->time = strdup("--:--");
	  stat->level = (double)bat_val / 100.0;
	}
      else
	{
	  snprintf(buf, sizeof(buf), "%i%%", bat_val);
	  stat->reading = strdup(buf);
	  snprintf(buf, sizeof(buf), "%i:%02i", hours, minutes);
	  stat->time = strdup(buf);
	  stat->level = (double)bat_val / 100.0;
	}
    }
  else
    {
      stat->has_battery = 1;
      stat->state = BATTERY_STATE_NONE;
      stat->reading = strdup("FULL");
      stat->time = strdup("--:--");
      stat->level = 1.0;
    }
  return stat;
}

static Status *
_battery_linux_apm_check(Battery *ef)
{
  FILE *f;
  char s[256], s1[32], s2[32], s3[32], buf[4096];
  int  apm_flags, ac_stat, bat_stat, bat_flags, bat_val, time_val;
  int  hours, minutes;

  Status *stat;

  f = fopen("/proc/apm", "r");
  if (!f) return NULL;

  fgets(s, sizeof(s), f); s[sizeof(s) - 1] = 0;
  if (sscanf(s, "%*s %*s %x %x %x %x %s %s %s",
	     &apm_flags, &ac_stat, &bat_stat, &bat_flags, s1, s2, s3) != 7)
    {
      fclose(f);
      return NULL;
    }
  s1[strlen(s1) - 1] = 0;
  bat_val = atoi(s1);
  if (!strcmp(s3, "sec")) time_val = atoi(s2);
  else if (!strcmp(s3, "min")) time_val = atoi(s2) * 60;
  fclose(f);

  //stat = E_NEW(Status, 1);
  stat = malloc(sizeof(Status));
  memset(stat,0,sizeof(Status));

  if ((bat_flags != 0xff) && (bat_flags & 0x80))
    {
      stat->has_battery = 0;
      stat->state = BATTERY_STATE_NONE;
      stat->reading = strdup("NO BAT");
      stat->time = strdup("--:--");
      stat->level = 1.0;
      return stat;
    }
   
   
  ef->battery_prev_battery = 1;
  stat->has_battery = 1;
  if (bat_val >= 0)
    {
      if (bat_val > 100) bat_val = 100;
      snprintf(buf, sizeof(buf), "%i%%", bat_val);
      stat->reading = strdup(buf);
      stat->level = (double)bat_val / 100.0;
    }
  else
    {
      switch (bat_stat)
	{
	case 0:
	  stat->reading = strdup("High");
	  stat->level = 1.0;
	  break;
	case 1:
	  stat->reading = strdup("Low");
	  stat->level = 0.5;
	  break;
	case 2:
	  stat->reading = strdup("Danger");
	  stat->level = 0.25;
	  break;
	case 3:
	  stat->reading = strdup("Charging");
	  stat->level = 1.0;
	  break;
	}
    }

  if (ac_stat == 1)
    {
      stat->state = BATTERY_STATE_CHARGING;
      stat->time = strdup("--:--");
    }
  else
    {
      /* ac_stat == 0 */
      stat->state = BATTERY_STATE_DISCHARGING;

      hours = time_val / 3600;
      minutes = (time_val / 60) % 60;
      snprintf(buf, sizeof(buf), "%i:%02i", hours, minutes);
      stat->time = strdup(buf);
      if (stat->level < 0.1)
	{
	  if (((hours * 60) + minutes) <= 10)
	    stat->alarm = 1;
	}
    }

  return stat;
}

static void
_battery_level_set(Battery *bat, double level)
{
  Edje_Message_Float msg;
  
  if (level < 0.0) level = 0.0;
  else if (level > 1.0) level = 1.0;
  msg.val = level;
  edje_object_message_send(bat->object, EDJE_MESSAGE_FLOAT, 1, &msg);
}


