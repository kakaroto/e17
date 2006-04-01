#include <string.h>
#include <stdio.h>
#include "e.h"
#include "e_mod_main.h"

/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/
void
fill_in_caltable(int FirstDay, Calendar_Face * face, int StartUp)
{
   int x = 0;
   int i = 0;
   int row = 0;

   while (x < 35)
     {
        for (; i < 7; i++)
          {
             switch (FirstDay)
               {
               case 0:
                  if ((i == 0) || (i == 6))
                    {
                       evas_object_show(face->weekend_object[x]->obj);
                       evas_object_hide(face->weekday_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  else
                    {
                       evas_object_show(face->weekday_object[x]->obj);
                       evas_object_hide(face->weekend_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  break;
               case 1:
                  if ((i == 5) || (i == 6))
                    {
                       evas_object_show(face->weekend_object[x]->obj);
                       evas_object_hide(face->weekday_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  else
                    {
                       evas_object_show(face->weekday_object[x]->obj);
                       evas_object_hide(face->weekend_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  break;
               case 2:
                  if ((i == 4) || (i == 5))
                    {
                       evas_object_show(face->weekend_object[x]->obj);
                       evas_object_hide(face->weekday_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  else
                    {
                       evas_object_show(face->weekday_object[x]->obj);
                       evas_object_hide(face->weekend_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  break;
               case 3:
                  if ((i == 3) || (i == 4))
                    {
                       evas_object_show(face->weekend_object[x]->obj);
                       evas_object_hide(face->weekday_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  else
                    {
                       evas_object_show(face->weekday_object[x]->obj);
                       evas_object_hide(face->weekend_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  break;
               case 4:
                  if ((i == 2) || (i == 3))
                    {
                       evas_object_show(face->weekend_object[x]->obj);
                       evas_object_hide(face->weekday_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  else
                    {
                       evas_object_show(face->weekday_object[x]->obj);
                       evas_object_hide(face->weekend_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  break;
               case 5:
                  if ((i == 1) || (i == 2))
                    {
                       evas_object_show(face->weekend_object[x]->obj);
                       evas_object_hide(face->weekday_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  else
                    {
                       evas_object_show(face->weekday_object[x]->obj);
                       evas_object_hide(face->weekend_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  break;
               case 6:
                  if ((i == 0) || (i == 1))
                    {
                       evas_object_show(face->weekend_object[x]->obj);
                       evas_object_hide(face->weekday_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  else
                    {
                       evas_object_show(face->weekday_object[x]->obj);
                       evas_object_hide(face->weekend_object[x]->obj);
                       evas_object_hide(face->today_object[x]->obj);
                    }
                  break;
               default:
                  break;
               }
             x++;
          }
        i = 0;
        row++;
     }

}

/***************************************************
/ Function: 
/ Purpose:  
/
/
*****************************************************/
int
calculate_skew(Calendar * calendar)
{
//skew correction for month -  http://klausler.com/new-dayofweek.html
   int yearsfrom1900 = calendar->conf->view_year - 1900;
   int onefourthyear = yearsfrom1900 / 4;
   int onefourthyearmod = yearsfrom1900 % 4;
   int skew;

   switch (calendar->conf->view_month)
     {
     case 0:                   //Jan
        skew = (yearsfrom1900 + onefourthyear) % 7;
        if (onefourthyearmod == 0)
           skew = skew - 1;
        skew = (skew + 1) % 7;
        break;
     case 1:                   //Feb
        skew = (yearsfrom1900 + onefourthyear) % 7;
        if (onefourthyearmod == 0)
           skew = skew - 1;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 3) % 7;
        break;
     case 2:                   //March
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 3) % 7;
        break;
     case 3:                   //April
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 6) % 7;
        break;
     case 4:                   //May
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 1) % 7;
        break;
     case 5:                   //June
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 4) % 7;
        break;
     case 6:                   //July
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 6) % 7;
        break;
     case 7:                   //Aug
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 2) % 7;
        break;
     case 8:                   //Sept
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 5) % 7;
        break;
     case 9:                   //Oct
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        break;
     case 10:                  //Nov
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 3) % 7;
        break;
     case 11:                  //Dec
        skew = (yearsfrom1900 + onefourthyear) % 7;
        skew = (skew + 1) % 7;  //1st day of month
        skew = (skew + 5) % 7;
        break;
     default:
        break;
     }                          //End Switch
   if (calendar->conf->DayofWeek_Start != 0)
     {
        if (calendar->conf->DayofWeek_Start == 1)
           skew = ((skew - 1) + 7) % 7;
        else if (calendar->conf->DayofWeek_Start == 2)
           skew = ((skew - 2) + 7) % 7;
        else if (calendar->conf->DayofWeek_Start == 3)
           skew = ((skew - 3) + 7) % 7;
        else if (calendar->conf->DayofWeek_Start == 4)
           skew = ((skew - 4) + 7) % 7;
        else if (calendar->conf->DayofWeek_Start == 5)
           skew = ((skew - 5) + 7) % 7;
        else if (calendar->conf->DayofWeek_Start == 6)
           skew = ((skew - 6) + 7) % 7;
     }
   return skew;
}
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/
void
calendar_add_dates(Calendar_Face * face, Calendar * calendar, int skew)
{
   time_t now;
   struct tm date;
   char buf[3];
   int NumDayMon[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
   time(&now);
   date = *localtime(&now);
   int DayOfMonth = 1;
   int Sun = 0;
   int Sat = 6;

   if ((calendar->conf->view_year % 4 == 0 ) && 
      !((calendar->conf->view_year % 100 == 0 ) && 
      (calendar->conf->view_year % 1000 != 0 )))
      NumDayMon[1] = 29;
   
   snprintf(buf, sizeof(buf), "%d", DayOfMonth);
   switch (calendar->conf->DayofWeek_Start)
     {
     case 0:
        Sat = 6;
        Sun = 0;
        break;
     case 1:
        Sat = 5;
        Sun = 6;
        break;
     case 2:
        Sat = 4;
        Sun = 5;
        break;
     case 3:
        Sat = 3;
        Sun = 4;
        break;
     case 4:
        Sat = 2;
        Sun = 3;
        break;
     case 5:
        Sat = 1;
        Sun = 2;
        break;
     case 6:
        Sat = 0;
        Sun = 1;
        break;
     default:
        break;
     }

   //blank out blocks before day 1
   int i = 0;

   for (; i < skew; i++)
     {
        evas_object_hide(face->weekday_object[i]->obj);
        evas_object_hide(face->weekend_object[i]->obj);
     }
   if (date.tm_mday == 1)
     {
        edje_object_part_text_set(face->today_object[skew]->obj, "date-text",
                                  buf);
        evas_object_show(face->today_object[skew]->obj);
        evas_object_hide(face->weekend_object[skew]->obj);
        evas_object_hide(face->weekday_object[skew]->obj);
     }
   else
     {
        switch (skew)           //Place the 1st on the proper day
          {
          case 0:
             if ((calendar->conf->DayofWeek_Start != 0)
                 && (calendar->conf->DayofWeek_Start != 6))
                edje_object_part_text_set(face->weekday_object[skew]->obj,
                                          "date-text", buf);
             else
                edje_object_part_text_set(face->weekend_object[skew]->obj,
                                          "date-text", buf);
             break;
          case 1:
             if ((calendar->conf->DayofWeek_Start != 5)
                 && (calendar->conf->DayofWeek_Start != 6))
                edje_object_part_text_set(face->weekday_object[skew]->obj,
                                          "date-text", buf);
             else
                edje_object_part_text_set(face->weekend_object[skew]->obj,
                                          "date-text", buf);
             break;
          case 2:
             if ((calendar->conf->DayofWeek_Start != 4)
                 && (calendar->conf->DayofWeek_Start != 5))
                edje_object_part_text_set(face->weekday_object[skew]->obj,
                                          "date-text", buf);
             else
                edje_object_part_text_set(face->weekend_object[skew]->obj,
                                          "date-text", buf);
             break;
          case 3:
             if ((calendar->conf->DayofWeek_Start != 3)
                 && (calendar->conf->DayofWeek_Start != 4))
                edje_object_part_text_set(face->weekday_object[skew]->obj,
                                          "date-text", buf);
             else
                edje_object_part_text_set(face->weekend_object[skew]->obj,
                                          "date-text", buf);
             break;
          case 4:
             if ((calendar->conf->DayofWeek_Start != 2)
                 && (calendar->conf->DayofWeek_Start != 3))
                edje_object_part_text_set(face->weekday_object[skew]->obj,
                                          "date-text", buf);
             else
                edje_object_part_text_set(face->weekend_object[skew]->obj,
                                          "date-text", buf);
             break;
          case 5:
             if ((calendar->conf->DayofWeek_Start != 1)
                 && (calendar->conf->DayofWeek_Start != 2))
                edje_object_part_text_set(face->weekday_object[skew]->obj,
                                          "date-text", buf);
             else
                edje_object_part_text_set(face->weekend_object[skew]->obj,
                                          "date-text", buf);
             break;
          case 6:
             if ((calendar->conf->DayofWeek_Start != 0)
                 && (calendar->conf->DayofWeek_Start != 1))
                edje_object_part_text_set(face->weekday_object[skew]->obj,
                                          "date-text", buf);
             else
                edje_object_part_text_set(face->weekend_object[skew]->obj,
                                          "date-text", buf);
             break;
          default:
             break;
          }
     }
   int counter = skew + 1;
   int c_col = counter;
   int row = 0;

   while (DayOfMonth < NumDayMon[calendar->conf->view_month])
     {
        while ((c_col < 7)
               && (DayOfMonth < NumDayMon[calendar->conf->view_month]))
          {
             DayOfMonth++;
             snprintf(buf, sizeof(buf), "%d", DayOfMonth);
             if ((DayOfMonth == date.tm_mday)
                 && (calendar->conf->view_month == date.tm_mon)
                 && (calendar->conf->view_year == (date.tm_year + 1900)))
               {
                  edje_object_part_text_set(face->today_object[counter]->obj,
                                            "date-text", buf);
                  evas_object_show(face->today_object[counter]->obj);
                  evas_object_hide(face->weekend_object[counter]->obj);
                  evas_object_hide(face->weekday_object[counter]->obj);
               }
             else if (((counter % 7) == Sat) || ((counter % 7) == Sun))
               {
                  edje_object_part_text_set(face->weekend_object[counter]->obj,
                                            "date-text", buf);
               }
             else
                edje_object_part_text_set(face->weekday_object[counter]->obj,
                                          "date-text", buf);
             c_col++;
             counter++;
          }                     //End inside while
        c_col = 0;
        row++;
        // 6th week dates..
        if ((counter >= 35)
            && (DayOfMonth <= NumDayMon[calendar->conf->view_month]))
          {
             DayOfMonth++;
             counter = counter - 7;
             int week_prior = 0;
             char DateBox[24];

             while (DayOfMonth <= NumDayMon[calendar->conf->view_month])
               {
                  week_prior = DayOfMonth - 7;
                  snprintf(DateBox, sizeof(DateBox), "%i/%i", week_prior,
                           DayOfMonth);
                  if ((DayOfMonth == date.tm_mday)
                      && (calendar->conf->view_month == date.tm_mon)
                      && (calendar->conf->view_year == date.tm_year + 1900))
                    {
                       edje_object_part_text_set(face->today_object[counter]->
                                                 obj, "date-text", DateBox);
                       evas_object_show(face->today_object[counter]->obj);
                       evas_object_hide(face->weekend_object[counter]->obj);
                       evas_object_hide(face->weekday_object[counter]->obj);
                    }
                  else if (((counter % 7) == Sat) || ((counter % 7) == Sun))
                    {
                       edje_object_part_text_set(face->weekend_object[counter]->
                                                 obj, "date-text", DateBox);
                    }
                  else
                    {
                       edje_object_part_text_set(face->weekday_object[counter]->
                                                 obj, "date-text", DateBox);
                    }
                  counter++;
                  DayOfMonth++;
               }
             counter = 35;
          }
     }                          //End outside while */
   if (counter < 35)
     {
        while (counter < 35)
          {
             evas_object_hide(face->weekday_object[counter]->obj);
             evas_object_hide(face->weekend_object[counter]->obj);
             counter++;
          }
     }
}                               //End function
