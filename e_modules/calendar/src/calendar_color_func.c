#include <string.h>
#include <stdio.h>
#include "e.h"
#include "e_mod_main.h"

/***************************************************
/ Function: 
/ Purpose:  Read the config file and assign values to
/           to Calendar Config variables.  Working...
/
*****************************************************/ 
void               
calendar_face_set_colors(Calendar * calendar) 
{
   
char               read_line[120];
   
int                r, g, b, a, r1, g1, b1, a1, r2, g2, b2, a2;
   
int                UseDefault = 0;
   
char               ValidName1[] = { "Today_s_back_colors" };
   
char               ValidName2[] = { "Weekend_back_colors" };
   
char               ValidName3[] = { "WeekDay_back_colors" };
   
char               ValidName4[] = { "Today_s_text_colors" };
   
char               ValidName5[] = { "Weekend_text_colors" };
   
char               ValidName6[] = { "WeekDay_text_colors" };
   
char               ValidName7[] = { "YearMon_numb_colors" };
   
char               ValidName8[] = { "DayWeek_text_colors" };
   

c_array * TBC_Ptr = calendar->conf->Today_s_back_colors->data;
   
c_array * TTC_Ptr = calendar->conf->Today_s_text_colors->data;
   
c_array * WeTC_Ptr = calendar->conf->Weekend_text_colors->data;
   
c_array * WdTC_Ptr = calendar->conf->WeekDay_text_colors->data;
   
c_array * WeBC_Ptr = calendar->conf->Weekend_back_colors->data;
   
c_array * WdBC_Ptr = calendar->conf->WeekDay_back_colors->data;
   
c_array * YNC_Ptr = calendar->conf->YearMon_numb_colors->data;
   
c_array * DTC_Ptr = calendar->conf->DayWeek_text_colors->data;
   

char              buff[20];

   
FILE * fp;
   

fp = fopen(calendar->conf->ConfigFile_path, "r");
   
if (fp)
      
     {
        
while ((fgets(read_line, sizeof(read_line), fp)))
           
          {
             
sscanf(read_line, "%20s %d %d %d %d %d %d %d %d %d %d %d %d",
                     
buff, &r, &g, &b, &a, &r1, &g1, &b1, &a1, &r2, &g2, &b2,
                     &a2);
             
if (strncmp(buff, ValidName1, 19) == 0)
                
               {
                  
TBC_Ptr->red = r;
                  
TBC_Ptr->green = g;
                  
TBC_Ptr->blue = b;
                  
TBC_Ptr->alpha = a;
                  
TBC_Ptr->red_o = r1;
                  
TBC_Ptr->red_s = r2;
                  
TBC_Ptr->green_o = g1;
                  
TBC_Ptr->green_s = g2;
                  
TBC_Ptr->blue_o = b1;
                  
TBC_Ptr->blue_s = b2;
                  
TBC_Ptr->alpha_o = a1;
                  
TBC_Ptr->alpha_s = a2;
                  
UseDefault++;
               
}
             
if (strncmp(buff, ValidName2, 19) == 0)
                
               {
                  
WeBC_Ptr->red = r;
                  
WeBC_Ptr->green = g;
                  
WeBC_Ptr->blue = b;
                  
WeBC_Ptr->alpha = a;
                  
WeBC_Ptr->red_o = r1;
                  
WeBC_Ptr->red_s = r2;
                  
WeBC_Ptr->green_o = g1;
                  
WeBC_Ptr->green_s = g2;
                  
WeBC_Ptr->blue_o = b1;
                  
WeBC_Ptr->blue_s = b2;
                  
WeBC_Ptr->alpha_o = a1;
                  
WeBC_Ptr->alpha_s = a2;
                  
UseDefault++;
               
}
             
if (strncmp(buff, ValidName3, 19) == 0)
                
               {
                  
WdBC_Ptr->red = r;
                  
WdBC_Ptr->green = g;
                  
WdBC_Ptr->blue = b;
                  
WdBC_Ptr->alpha = a;
                  
WdBC_Ptr->red_o = r1;
                  
WdBC_Ptr->red_s = r2;
                  
WdBC_Ptr->green_o = g1;
                  
WdBC_Ptr->green_s = g2;
                  
WdBC_Ptr->blue_o = b1;
                  
WdBC_Ptr->blue_s = b2;
                  
WdBC_Ptr->alpha_o = a1;
                  
WdBC_Ptr->alpha_s = a2;
                  
UseDefault++;
               
}
             
if (strncmp(buff, ValidName4, 19) == 0)
                
               {
                  
TTC_Ptr->red = r;
                  
TTC_Ptr->green = g;
                  
TTC_Ptr->blue = b;
                  
TTC_Ptr->alpha = a;
                  
TTC_Ptr->red_o = r1;
                  
TTC_Ptr->red_s = r2;
                  
TTC_Ptr->green_o = g1;
                  
TTC_Ptr->green_s = g2;
                  
TTC_Ptr->blue_o = b1;
                  
TTC_Ptr->blue_s = b2;
                  
TTC_Ptr->alpha_o = a1;
                  
TTC_Ptr->alpha_s = a2;
                  
UseDefault++;
               
}
             
if (strncmp(buff, ValidName5, 19) == 0)
                
               {
                  
WeTC_Ptr->red = r;
                  
WeTC_Ptr->green = g;
                  
WeTC_Ptr->blue = b;
                  
WeTC_Ptr->alpha = a;
                  
WeTC_Ptr->red_o = r1;
                  
WeTC_Ptr->red_s = r2;
                  
WeTC_Ptr->green_o = g1;
                  
WeTC_Ptr->green_s = g2;
                  
WeTC_Ptr->blue_o = b1;
                  
WeTC_Ptr->blue_s = b2;
                  
WeTC_Ptr->alpha_o = a1;
                  
WeTC_Ptr->alpha_s = a2;
                  
UseDefault++;
               
}
             
if (strncmp(buff, ValidName6, 19) == 0)
                
               {
                  
WdTC_Ptr->red = r;
                  
WdTC_Ptr->green = g;
                  
WdTC_Ptr->blue = b;
                  
WdTC_Ptr->alpha = a;
                  
WdTC_Ptr->red_o = r1;
                  
WdTC_Ptr->red_s = r2;
                  
WdTC_Ptr->green_o = g1;
                  
WdTC_Ptr->green_s = g2;
                  
WdTC_Ptr->blue_o = b1;
                  
WdTC_Ptr->blue_s = b2;
                  
WdTC_Ptr->alpha_o = a1;
                  
WdTC_Ptr->alpha_s = a2;
                  
UseDefault++;
               
}
             
if (strncmp(buff, ValidName7, 19) == 0)
                
               {
                  
YNC_Ptr->red = r;
                  
YNC_Ptr->green = g;
                  
YNC_Ptr->blue = b;
                  
YNC_Ptr->alpha = a;
                  
YNC_Ptr->red_o = r1;
                  
YNC_Ptr->red_s = r2;
                  
YNC_Ptr->green_o = g1;
                  
YNC_Ptr->green_s = g2;
                  
YNC_Ptr->blue_o = b1;
                  
YNC_Ptr->blue_s = b2;
                  
YNC_Ptr->alpha_o = a1;
                  
YNC_Ptr->alpha_s = a2;
                  
UseDefault++;
               
}
             
if (strncmp(buff, ValidName8, 19) == 0)
                
               {
                  
DTC_Ptr->red = r;
                  
DTC_Ptr->green = g;
                  
DTC_Ptr->blue = b;
                  
DTC_Ptr->alpha = a;
                  
DTC_Ptr->red_o = r1;
                  
DTC_Ptr->red_s = r2;
                  
DTC_Ptr->green_o = g1;
                  
DTC_Ptr->green_s = g2;
                  
DTC_Ptr->blue_o = b1;
                  
DTC_Ptr->blue_s = b2;
                  
DTC_Ptr->alpha_o = a1;
                  
DTC_Ptr->alpha_s = a2;
                  
UseDefault++;
               
}
          
}
        
fclose(fp);
     
}
   
if (TBC_Ptr->red == -1)
      
     {
        
TBC_Ptr->red = 150;
        
TBC_Ptr->green = 0;
        
TBC_Ptr->blue = 0;
        
TBC_Ptr->alpha = 20;
        
TBC_Ptr->red_o = 0;
        
TBC_Ptr->red_s = 0;
        
TBC_Ptr->green_o = 0;
        
TBC_Ptr->green_s = 0;
        
TBC_Ptr->blue_o = 0;
        
TBC_Ptr->blue_s = 0;
        
TBC_Ptr->alpha_o = 0;
        
TBC_Ptr->alpha_s = 0;
     
}
   
if (WeBC_Ptr->red == -1)
      
     {
        
WeBC_Ptr->red = 0;
        
WeBC_Ptr->green = 150;
        
WeBC_Ptr->blue = 0;
        
WeBC_Ptr->alpha = 20;
        
WeBC_Ptr->red_o = 0;
        
WeBC_Ptr->red_s = 0;
        
WeBC_Ptr->green_o = 0;
        
WeBC_Ptr->green_s = 0;
        
WeBC_Ptr->blue_o = 0;
        
WeBC_Ptr->blue_s = 0;
        
WeBC_Ptr->alpha_o = 0;
        
WeBC_Ptr->alpha_s = 0;
     
}
   
if (WdBC_Ptr->red == -1)
      
     {
        
WdBC_Ptr->red = 0;
        
WdBC_Ptr->green = 0;
        
WdBC_Ptr->blue = 150;
        
WdBC_Ptr->alpha = 20;
        
WdBC_Ptr->red_o = 0;
        
WdBC_Ptr->red_s = 0;
        
WdBC_Ptr->green_o = 0;
        
WdBC_Ptr->green_s = 0;
        
WdBC_Ptr->blue_o = 0;
        
WdBC_Ptr->blue_s = 0;
        
WdBC_Ptr->alpha_o = 0;
        
WdBC_Ptr->alpha_s = 0;
     
}
   
if (TTC_Ptr->red == -1)
      
     {
        
TTC_Ptr->red = 255;
        
TTC_Ptr->green = 255;
        
TTC_Ptr->blue = 255;
        
TTC_Ptr->alpha = 200;
        
TTC_Ptr->red_o = 0;
        
TTC_Ptr->red_s = 0;
        
TTC_Ptr->green_o = 0;
        
TTC_Ptr->green_s = 0;
        
TTC_Ptr->blue_o = 0;
        
TTC_Ptr->blue_s = 0;
        
TTC_Ptr->alpha_o = 0;
        
TTC_Ptr->alpha_s = 0;
     
}
   
if (WeTC_Ptr->red == -1)
      
     {
        
WeTC_Ptr->red = 255;
        
WeTC_Ptr->green = 255;
        
WeTC_Ptr->blue = 255;
        
WeTC_Ptr->alpha = 200;
        
WeTC_Ptr->red_o = 0;
        
WeTC_Ptr->red_s = 0;
        
WeTC_Ptr->green_o = 0;
        
WeTC_Ptr->green_s = 0;
        
WeTC_Ptr->blue_o = 0;
        
WeTC_Ptr->blue_s = 0;
        
WeTC_Ptr->alpha_o = 0;
        
WeTC_Ptr->alpha_s = 0;
     
}
   
if (WdTC_Ptr->red == -1)
      
     {
        
WdTC_Ptr->red = 255;
        
WdTC_Ptr->green = 255;
        
WdTC_Ptr->blue = 255;
        
WdTC_Ptr->alpha = 200;
        
WdTC_Ptr->red_o = 0;
        
WdTC_Ptr->red_s = 0;
        
WdTC_Ptr->green_o = 0;
        
WdTC_Ptr->green_s = 0;
        
WdTC_Ptr->blue_o = 0;
        
WdTC_Ptr->blue_s = 0;
        
WdTC_Ptr->alpha_o = 0;
        
WdTC_Ptr->alpha_s = 0;
     
}
   
if (YNC_Ptr->red == -1)
      
     {
        
YNC_Ptr->red = 255;
        
YNC_Ptr->green = 255;
        
YNC_Ptr->blue = 255;
        
YNC_Ptr->alpha = 200;
        
YNC_Ptr->red_o = 0;
        
YNC_Ptr->red_s = 0;
        
YNC_Ptr->green_o = 0;
        
YNC_Ptr->green_s = 0;
        
YNC_Ptr->blue_o = 0;
        
YNC_Ptr->blue_s = 0;
        
YNC_Ptr->alpha_o = 0;
        
YNC_Ptr->alpha_s = 0;
     
}
   
if (DTC_Ptr->red == -1)
      
     {
        
DTC_Ptr->red = 255;
        
DTC_Ptr->green = 255;
        
DTC_Ptr->blue = 255;
        
DTC_Ptr->alpha = 200;
        
DTC_Ptr->red_o = 0;
        
DTC_Ptr->red_s = 0;
        
DTC_Ptr->green_o = 0;
        
DTC_Ptr->green_s = 0;
        
DTC_Ptr->blue_o = 0;
        
DTC_Ptr->blue_s = 0;
        
DTC_Ptr->alpha_o = 0;
        
DTC_Ptr->alpha_s = 0;
     
}

}



/***************************************************
/ Function: 
/ Purpose:  Assign Calendar Config color variable to 
/           appropriate colorclass in edje.  
/
*****************************************************/ 
    void
update_colors(Calendar * calendar, Calendar_Face * face) 
{
   
int                x;

   

c_array * TBC_Ptr = calendar->conf->Today_s_back_colors->data;
   
c_array * TTC_Ptr = calendar->conf->Today_s_text_colors->data;
   
c_array * WeTC_Ptr = calendar->conf->Weekend_text_colors->data;
   
c_array * WdTC_Ptr = calendar->conf->WeekDay_text_colors->data;
   
c_array * WeBC_Ptr = calendar->conf->Weekend_back_colors->data;
   
c_array * WdBC_Ptr = calendar->conf->WeekDay_back_colors->data;
   
c_array * YNC_Ptr = calendar->conf->YearMon_numb_colors->data;
   
c_array * DTC_Ptr = calendar->conf->DayWeek_text_colors->data;
   
//set weekday colors
       for (x = 0; x < 35; x++)
      
     {
        
edje_object_color_class_set(face->weekday_object[x]->obj, "day_colors",
                                     WdBC_Ptr->red, 
WdBC_Ptr->green,
                                     WdBC_Ptr->blue, WdBC_Ptr->alpha,
                                     WdBC_Ptr->red_o, 
WdBC_Ptr->green_o,
                                     WdBC_Ptr->blue_o, WdBC_Ptr->alpha_o,
                                     WdBC_Ptr->red_s, 
WdBC_Ptr->green_s,
                                     WdBC_Ptr->blue_s, WdBC_Ptr->alpha_s);
        

edje_object_color_class_set(face->weekday_object[x]->obj,
                                      "text_colors", WdTC_Ptr->red,
                                      
WdTC_Ptr->green, WdTC_Ptr->blue,
                                      WdTC_Ptr->alpha, WdTC_Ptr->red_o,
                                      
WdTC_Ptr->green_o, WdTC_Ptr->blue_o,
                                      WdTC_Ptr->alpha_o, WdTC_Ptr->red_s,
                                      
WdTC_Ptr->green_s, WdTC_Ptr->blue_s,
                                      WdTC_Ptr->alpha_s);
     

}
   
//Set weekend colors
       for (x = 0; x < 35; x++)
      
     {
        
edje_object_color_class_set(face->weekend_object[x]->obj,
                                     "text_colors", WeTC_Ptr->red,
                                     
WeTC_Ptr->green, WeTC_Ptr->blue,
                                     WeTC_Ptr->alpha, WeTC_Ptr->red_o,
                                     
WeTC_Ptr->green_o, WeTC_Ptr->blue_o,
                                     WeTC_Ptr->alpha_o, WeTC_Ptr->red_s,
                                     
WeTC_Ptr->green_s, WeTC_Ptr->blue_s,
                                     WeTC_Ptr->alpha_s);
        

edje_object_color_class_set(face->weekend_object[x]->obj,
                                      "day_colors", WeBC_Ptr->red,
                                      
WeBC_Ptr->green, WeBC_Ptr->blue,
                                      WeBC_Ptr->alpha, WeBC_Ptr->red_o,
                                      
WeBC_Ptr->green_o, WeBC_Ptr->blue_o,
                                      WeBC_Ptr->alpha_o, WeBC_Ptr->red_s,
                                      
WeBC_Ptr->green_s, WeBC_Ptr->blue_s,
                                      WeBC_Ptr->alpha_s);
     
}
   
//Set Today colors
       for (x = 0; x < 35; x++)
      
     {
        
edje_object_color_class_set(face->today_object[x]->obj, "text_colors",
                                     TTC_Ptr->red, 
TTC_Ptr->green,
                                     TTC_Ptr->blue, TTC_Ptr->alpha,
                                     TTC_Ptr->red_o, 
TTC_Ptr->green_o,
                                     TTC_Ptr->blue_o, TTC_Ptr->alpha_o,
                                     TTC_Ptr->red_s, 
TTC_Ptr->green_s,
                                     TTC_Ptr->blue_s, TTC_Ptr->alpha_s);
        

edje_object_color_class_set(face->today_object[x]->obj, "day_colors",
                                      TBC_Ptr->red, 
TBC_Ptr->green,
                                      TBC_Ptr->blue, TBC_Ptr->alpha,
                                      TBC_Ptr->red_o, 
TBC_Ptr->green_o,
                                      TBC_Ptr->blue_o, TBC_Ptr->alpha_o,
                                      TBC_Ptr->red_s, 
TBC_Ptr->green_s,
                                      TBC_Ptr->blue_s, TBC_Ptr->alpha_s);
     
}
   
//Set Label colors
       edje_object_color_class_set(face->label_object, "YearMonth_colors",
                                   
YNC_Ptr->red, YNC_Ptr->green, YNC_Ptr->blue,
                                   YNC_Ptr->alpha, 
YNC_Ptr->red_o,
                                   YNC_Ptr->green_o, YNC_Ptr->blue_o,
                                   YNC_Ptr->alpha_o, 
YNC_Ptr->red_s,
                                   YNC_Ptr->green_s, YNC_Ptr->blue_s,
                                   YNC_Ptr->alpha_s);
   

edje_object_color_class_set(face->label_object, "Day_colors", DTC_Ptr->red,
                                 
DTC_Ptr->green, DTC_Ptr->blue, DTC_Ptr->alpha,
                                 DTC_Ptr->red_o, 
DTC_Ptr->green_o,
                                 DTC_Ptr->blue_o, DTC_Ptr->alpha_o,
                                 DTC_Ptr->red_s, 
DTC_Ptr->green_s,
                                 DTC_Ptr->blue_s, DTC_Ptr->alpha_s);

}

