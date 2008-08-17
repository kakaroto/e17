#define _GNU_SOURCE
#include "epplet.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

/* Modified by Attila ZIMLER <hijaszu@hlfslinux.hu>, 2003/11/16
   Added ACPI power management support.
*/

/* Length of explain strings in /proc/acpi/battery/BAT0 data files */
#define DATA_EXPLAIN_STR_LEN	25

int                 prev_bat_val = 110;
int                 bat_val = 0;
int                 time_val = 0;
int                 prev_up[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0
};
int                 prev_count = 0;
Epplet_gadget       b_close, b_suspend, b_sleep, b_help, image, label;

static void         cb_timer(void *data);
static void         cb_timer_acpi(void *data);
static void         cb_timer_apm(void *data);
static void         cb_close(void *data);
static void         cb_in(void *data, Window w);
static void         cb_out(void *data, Window w);
static void         cb_help(void *data);

static void
cb_timer(void *data)
{
   struct stat         st;

   if ((stat("/proc/apm", &st) > -1) && S_ISREG(st.st_mode))
      cb_timer_apm(data);
   else if ((stat("/proc/acpi", &st) > -1) && S_ISDIR(st.st_mode))
      cb_timer_acpi(data);
}

static void
cb_timer_acpi(void *data)
{
   /* We don't have any data from the remaining percentage, and time directly,
    * so we have to calculate and measure them.
    * (Measure the time and calculate the percentage.)
    */
   static int          prev_bat_drain = 1;

   FILE               *f;
   DIR                *dirp;
   struct dirent      *dp;

   int                 bat_max = 0;
   int                 bat_filled = 0;
   int                 bat_level = 0;
   int                 bat_drain = 1;

   int                 bat_val = 0;

   char                current_status[256];
   char               *line = 0;
   size_t              lsize = 0;
   int                 discharging = 0;
   int                 charging = 0;
   int                 battery = 0;

   int                 design_cap_unknown = 0;
   int                 last_full_unknown = 0;
   int                 rate_unknown = 0;
   int                 level_unknown = 0;

   int                 hours, minutes;

   /* Read some information on first run. */
   dirp = opendir("/proc/acpi/battery");
   if (dirp)
     {
	while ((dp = readdir(dirp)))
	  {
	     char                buf[4096];

	     if ((!strcmp(dp->d_name, ".")) || (!strcmp(dp->d_name, "..")))
		continue;
	     snprintf(buf, sizeof(buf), "/proc/acpi/battery/%s/info",
		      dp->d_name);
	     f = fopen(buf, "r");
	     if (f)
	       {
		  int                 design_cap = 0;
		  int                 last_full = 0;

		  getline(&line, &lsize, f);
		  getline(&line, &lsize, f);
		  sscanf(line, "%*[^:]: %250s %*s", buf);
		  if (!strcmp(buf, "unknown"))
		     design_cap_unknown = 1;
		  else
		     sscanf(line, "%*[^:]: %i %*s", &design_cap);
		  getline(&line, &lsize, f);
		  sscanf(line, "%*[^:]: %250s %*s", buf);
		  if (!strcmp(buf, "unknown"))
		     last_full_unknown = 1;
		  else
		     sscanf(line, "%*[^:]: %i %*s", &last_full);
		  fclose(f);
		  bat_max += design_cap;
		  bat_filled += last_full;
	       }
	     snprintf(buf, sizeof(buf), "/proc/acpi/battery/%s/state",
		      dp->d_name);
	     f = fopen(buf, "r");
	     if (f)
	       {
		  char                present[256];
		  char                capacity_state[256];
		  char                charging_state[256];
		  int                 rate = 1;
		  int                 level = 0;

		  getline(&line, &lsize, f);
		  sscanf(line, "%*[^:]: %250s", present);
		  getline(&line, &lsize, f);
		  sscanf(line, "%*[^:]: %250s", capacity_state);
		  getline(&line, &lsize, f);
		  sscanf(line, "%*[^:]: %250s", charging_state);
		  getline(&line, &lsize, f);
		  sscanf(line, "%*[^:]: %250s %*s", buf);
		  if (!strcmp(buf, "unknown"))
		     rate_unknown = 1;
		  else
		     sscanf(line, "%*[^:]: %i %*s", &rate);
		  getline(&line, &lsize, f);
		  sscanf(line, "%*[^:]: %250s %*s", buf);
		  if (!strcmp(buf, "unknown"))
		     level_unknown = 1;
		  else
		     sscanf(line, "%*[^:]: %i %*s", &level);
		  fclose(f);
		  if (!strcmp(present, "yes"))
		     battery++;
		  if (!strcmp(charging_state, "discharging"))
		     discharging++;
		  if (!strcmp(charging_state, "charging"))
		     charging++;
		  bat_drain += rate;
		  bat_level += level;
	       }
	  }
	closedir(dirp);
     }

   if (prev_bat_drain < 1)
      prev_bat_drain = 1;
   if (bat_drain < 1)
      bat_drain = prev_bat_drain;
   prev_bat_drain = bat_drain;

   if (bat_filled > 0)
      bat_val = (100 * bat_level) / bat_filled;
   else
      bat_val = 100;

   if (discharging)
      minutes = (60 * bat_level) / bat_drain;
   else
     {
	if (bat_filled > 0)
	   minutes = (60 * (bat_filled - bat_level)) / bat_drain;
	else
	   minutes = 0;
     }
   hours = minutes / 60;
   minutes -= (hours * 60);

   if (charging)
     {
	if (level_unknown)
	   snprintf(current_status, sizeof(current_status),
		    "Level ???\n" "Bad Driver");
	else if (rate_unknown)
	   snprintf(current_status, sizeof(current_status),
		    "%i%% PWR\n" "Time ???", bat_val);
	else
	   snprintf(current_status, sizeof(current_status),
		    "%i%% PWR\n" "%02i:%02i", bat_val, hours, minutes);
     }
   else if (discharging)
     {
	if (level_unknown)
	   snprintf(current_status, sizeof(current_status),
		    "Level ???\n" "Bad Driver");
	else if (rate_unknown)
	   snprintf(current_status, sizeof(current_status),
		    "%i%%\n" "Time ???", bat_val);
	else
	   snprintf(current_status, sizeof(current_status),
		    "%i%%\n" "%02i:%02i", bat_val, hours, minutes);
     }
   else if (!battery)
      snprintf(current_status, sizeof(current_status), "No Bat");
   else
      snprintf(current_status, sizeof(current_status), "Full");

   /* Display current status */
   Epplet_change_label(label, current_status);
   sprintf(current_status, "E-Power-Bat-%i.png", ((bat_val + 5) / 10) * 10);
   Epplet_change_image(image, 44, 24, current_status);
   Epplet_timer(cb_timer, NULL, 5.0, "TIMER");

   /* Final steps before ending the status update. */
   data = NULL;
   if (lsize)
      free(line);
}

static void
cb_timer_apm(void *data)
{
   static FILE        *f;

   f = fopen("/proc/apm", "r");
   if (f)
     {
	char                s[256], s1[32], s2[32], s3[32];
	int                 apm_flags, ac_stat, bat_stat, bat_flags;
	int                 i, hours, minutes, up, up2;
	char               *s_ptr;

	fgets(s, 255, f);
	sscanf(s, "%*s %*s %x %x %x %x %s %s %s", &apm_flags, &ac_stat,
	       &bat_stat, &bat_flags, s1, s2, s3);
	s1[strlen(s1) - 1] = 0;
	bat_val = atoi(s1);
	if (!strcmp(s3, "sec"))
	   time_val = atoi(s2);
	else if (!strcmp(s3, "min"))
	   time_val = atoi(s2) * 60;
	fclose(f);

	up = bat_val - prev_bat_val;
	up2 = up;
	for (i = 0; i < 16; i++)
	   up2 = +prev_up[i];
	up2 = (up2 * 60) / 17;

	prev_up[prev_count] = up;

	prev_count++;
	if (prev_count >= 16)
	   prev_count = 0;

	s_ptr = s;

	if (bat_flags != 0xff && bat_flags & 0x80)
	  {
	     s_ptr += sprintf(s_ptr, "no battery");
	  }
	else
	  {
	     if (bat_val > 0)
		s_ptr += sprintf(s_ptr, "%i%%", bat_val);

	     switch (bat_stat)
	       {
	       case 0:
		  s_ptr += sprintf(s_ptr, ", high");
		  break;
	       case 1:
		  s_ptr += sprintf(s_ptr, ", low");
		  break;
	       case 2:
		  s_ptr += sprintf(s_ptr, ", crit.");
		  break;
	       case 3:
		  s_ptr += sprintf(s_ptr, ", charge");
		  break;
	       }
	  }
	s_ptr += sprintf(s_ptr, "\n");

	if (ac_stat == 1)
	  {
	     s_ptr += sprintf(s_ptr, "AC on-line");
	  }
	else
	  {
	     hours = time_val / 3600;
	     minutes = (time_val / 60) % 60;
	     if (up2 > 0)
		s_ptr += sprintf(s_ptr, "(%i:%02i)\n%i:%02i",
				 (((100 - bat_val) * 2 * 60) / up2) / 60,
				 (((100 - bat_val) * 2 * 60) / up2) % 60,
				 hours, minutes);
	     else
		s_ptr += sprintf(s_ptr, "%i:%02i", hours, minutes);
	  }
	Epplet_change_label(label, s);

	sprintf(s, "E-Power-Bat-%i.png", ((bat_val + 5) / 10) * 10);
	Epplet_change_image(image, 44, 24, s);
	Epplet_timer(cb_timer, NULL, 30.0, "TIMER");

	prev_bat_val = bat_val;
     }
   data = NULL;
}

static void
cb_close(void *data)
{
   Epplet_unremember();
   Esync();
   data = NULL;
   exit(0);
}

static void
cb_in(void *data, Window w)
{
   Epplet_gadget_show(b_close);
   Epplet_gadget_show(b_suspend);
   Epplet_gadget_show(b_sleep);
   Epplet_gadget_show(b_help);
   return;
   data = NULL;
   w = (Window) 0;
}

static void
cb_out(void *data, Window w)
{
   Epplet_gadget_hide(b_close);
   Epplet_gadget_hide(b_suspend);
   Epplet_gadget_hide(b_sleep);
   Epplet_gadget_hide(b_help);
   return;
   data = NULL;
   w = (Window) 0;
}

static void
cb_help(void *data)
{
   Epplet_show_about("E-Power");
   return;
   data = NULL;
}

static void
cb_suspend(void *data)
{
   system("/usr/bin/apm -s");
   return;
   data = NULL;
}

static void
cb_sleep(void *data)
{
   system("/usr/bin/apm -S");
   return;
   data = NULL;
}

int
main(int argc, char **argv)
{
   Epplet_Init("E-Power", "0.1", "Enlightenment Laptop Power Epplet",
	       3, 3, argc, argv, 0);
   atexit(Epplet_cleanup);
   Epplet_timer(cb_timer, NULL, 30.0, "TIMER");
   b_close = Epplet_create_button(NULL, NULL,
				  2, 2, 0, 0, "CLOSE", 0, NULL, cb_close, NULL);
   b_help = Epplet_create_button(NULL, NULL,
				 34, 2, 0, 0, "HELP", 0, NULL, cb_help, NULL);
   b_suspend = Epplet_create_button(NULL, NULL,
				    2, 34, 0, 0, "PAUSE", 0, NULL,
				    cb_suspend, NULL);
   b_sleep = Epplet_create_button(NULL, NULL,
				  34, 34, 0, 0, "STOP", 0, NULL,
				  cb_sleep, NULL);
   Epplet_gadget_show(image = Epplet_create_image(2, 2, 44, 24,
						  "E-Power-Bat-100.png"));
   Epplet_gadget_show(label =
		      Epplet_create_label(2, 28, "APM, ACPI\nmissing", 1));
   Epplet_register_focus_in_handler(cb_in, NULL);
   Epplet_register_focus_out_handler(cb_out, NULL);
   cb_timer(NULL);
   Epplet_show();
   Epplet_Loop();
   return 0;
}
