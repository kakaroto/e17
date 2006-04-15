/*

 *      E-MoonClock v0.1 (C) 1999-2000 Michael Lea (mikelea@charm.net)
 *
 *              - Shows Moon Phase....
 *
 *      I am incredibly indebted to Mike Henderson, the original author of
 *      wmMoonClock.  He did the hard stuff, I ported it to an epplet.
 *
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2, or (at your option)
 *      any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see the file COPYING); if not, write to the
 *      Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *      Boston, MA  02111-1307, USA
 *
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <time.h>
#include "epplet.h"
#include "CalcEphem.h"

Epplet_gadget       close_button, help_button, moon_pixmap;
char               *moon_image = "E-MoonClock-01.png";
double              interval = 1000.0;

static void         close_cb(void *data);
static void         help_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);

static void
moonclock_cb(void *data)
{
   struct tm          *GMTTime, *LocalTime;
   int                 Year, Month, DayOfMonth;
   int                 ImageNumber;
   time_t              CurrentLocalTime, CurrentGMTTime, date;
   double              UT, LocalHour, hour24();
   double              TimeZone;
   CTrans              c;
   static char         buf[1024];

   CurrentGMTTime = time(CurrentTime);
   GMTTime = gmtime(&CurrentGMTTime);
   UT = GMTTime->tm_hour + GMTTime->tm_min / 60.0 + GMTTime->tm_sec / 3600.0;
   Year = GMTTime->tm_year + 1900;
   Month = GMTTime->tm_mon + 1;
   DayOfMonth = GMTTime->tm_mday;
   date = Year * 10000 + Month * 100 + DayOfMonth;
   CurrentLocalTime = CurrentGMTTime;
   LocalTime = localtime(&CurrentLocalTime);
   LocalHour =
      LocalTime->tm_hour + LocalTime->tm_min / 60.0 +
      LocalTime->tm_sec / 3600.0;
   TimeZone = UT - LocalHour;

   CalcEphem(date, UT, &c);

   ImageNumber = (int)(c.MoonPhase * 60.0 + 0.5);
   if (ImageNumber > 59)
      ImageNumber = 0;

   Esnprintf(buf, sizeof(buf), "E-MoonClock-%02d.png", ImageNumber);
   moon_pixmap = Epplet_create_image(2, 2, 43, 43, buf);
   Epplet_gadget_show(moon_pixmap);

   Epplet_timer(moonclock_cb, NULL, interval, "TIMER");

   return;

   data = NULL;

}
static void
close_cb(void *data)
{
   Epplet_unremember();
   Esync();
   exit(0);
   data = NULL;
}

static void
help_cb(void *data)
{
   Epplet_show_about("E-MoonClock");
   return;
   data = NULL;
}

static void
in_cb(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(close_button);
	Epplet_gadget_show(help_button);
     }
   return;
   data = NULL;
}

static void
out_cb(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(close_button);
	Epplet_gadget_hide(help_button);
     }
   return;
   data = NULL;
}

int
main(int argc, char **argv)
{
   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);
   Epplet_Init("E-MoonClock", "0.1", "Enlightenment MoonClock Epplet", 3, 3,
	       argc, argv, 0);
   Epplet_load_config();

   close_button =
      Epplet_create_button(NULL, NULL, 2, 2, 0, 0, "CLOSE", 0, NULL, close_cb,
			   NULL);
   help_button =
      Epplet_create_button(NULL, NULL, 18, 2, 0, 0, "HELP", 0, NULL, help_cb,
			   NULL);
   moon_pixmap = Epplet_create_image(2, 2, 43, 43, moon_image);
   Epplet_gadget_show(moon_pixmap);
   Epplet_show();

   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   moonclock_cb(NULL);
   Epplet_Loop();

   return 0;
}
