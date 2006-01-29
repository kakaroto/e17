#include "e.h"
#include "e_mod_main.h"

/***************************************************
/ Function: 
/ Purpose:  Assign Calendar Config Fonts to appropriate
/           Font classes in edje
/
*****************************************************/ 
void               
calendar_update_text_fonts(Calendar * calendar, Calendar_Face * face) 
{
   
CalFonts * YTC_Ptr = calendar->conf->YM_text_class->data;
   
CalFonts * DTCs_Ptr = calendar->conf->Day_text_class->data;
   
CalFonts * TC_Ptr = calendar->conf->text_class->data;
   
int                x;

   

for (x = 0; x < 35; x++)
      
edje_object_text_class_set(face->weekday_object[x]->obj, "text_class",
                                  
TC_Ptr->font, TC_Ptr->size);
   
for (x = 0; x < 35; x++)
      
edje_object_text_class_set(face->weekend_object[x]->obj, "text_class",
                                  
TC_Ptr->font, TC_Ptr->size);
   
for (x = 0; x < 35; x++)
      
edje_object_text_class_set(face->today_object[x]->obj, "text_class",
                                  
TC_Ptr->font, TC_Ptr->size);
   

edje_object_text_class_set(face->label_object, "YM_text_class",
                                
YTC_Ptr->font, YTC_Ptr->size);
   

edje_object_text_class_set(face->label_object, "Day_text_class",
                                
DTCs_Ptr->font, DTCs_Ptr->size);



}


/***************************************************
/ Function: 
/ Purpose:  REad config file and assign values to 
/           Calendar Config Variables -- working
/
*****************************************************/ 
void               
calendar_face_set_text(Calendar * calendar) 
{
   
char               read_line[120];
   
char               ValidName1[] = { "YM_text_class" };
   
char               ValidName2[] = { "Day_text_class" };
   
char               ValidName3[] = { "text_class" };
   
CalFonts * YTC_Ptr = calendar->conf->YM_text_class->data;
   
CalFonts * DTCs_Ptr = calendar->conf->Day_text_class->data;
   
CalFonts * TC_Ptr = calendar->conf->text_class->data;
   
char               buff[20];
   
char               buff2[20];
   
int                fs;

   
FILE * fp;
   
fp = fopen(calendar->conf->ConfigFile_path, "r");
   
if (fp)
      
     {
        
while ((fgets(read_line, sizeof(read_line), fp)))
           
          {
             
sscanf(read_line, "%s %s %d", buff, buff2, &fs);
             
if (strncmp(buff, ValidName1, 19) == 0)
                
               {
                  
YTC_Ptr->size = fs;
                  
YTC_Ptr->font = strdup(buff2);
               
}
             
if (strncmp(buff, ValidName2, 19) == 0)
                
               {
                  
DTCs_Ptr->size = fs;
                  
DTCs_Ptr->font = strdup(buff2);
               
}
             
if (strncmp(buff, ValidName3, 19) == 0)
                
               {
                  
TC_Ptr->size = fs;
                  
TC_Ptr->font = strdup(buff2);
               
}
          
}
        
fclose(fp);
     
}
   
if (strncmp(YTC_Ptr->font, "", 5) == 0)
      
     {
        
YTC_Ptr->size = 14;
        
YTC_Ptr->font = "VeraMono";
     
}
   
if (strncmp(DTCs_Ptr->font, "", 5) == 0)
      
     {
        
DTCs_Ptr->size = 14;
        
DTCs_Ptr->font = "VeraMono";
     
}
   
if (strncmp(TC_Ptr->font, "", 5) == 0)
      
     {
        
TC_Ptr->size = 8;
        
TC_Ptr->font = "VeraMono";
     
}

}


/***************************************************
/ Function: 
/ Purpose:  Set Day/Month/Year text in the label.edje
/           Still needs to be tested.
/
*****************************************************/ 
void               
set_day_label(Evas_Object * label, int FirstDay, int month, int year) 
{
   
char               PYear[5];

   
switch (FirstDay)
      
     {
       
case 0:
          
edje_object_part_text_set(label, "Sunday", _("Su"));
          
edje_object_part_text_set(label, "Monday", _("Mo"));
          
edje_object_part_text_set(label, "Tuesday", _("Tu"));
          
edje_object_part_text_set(label, "Wednesday", _("We"));
          
edje_object_part_text_set(label, "Thursday", _("Th"));
          
edje_object_part_text_set(label, "Friday", _("Fr"));
          
edje_object_part_text_set(label, "Saturday", _("Sa"));
          
break;
       
case 1:
          
edje_object_part_text_set(label, "Sunday", _("Mo"));
          
edje_object_part_text_set(label, "Monday", _("Tu"));
          
edje_object_part_text_set(label, "Tuesday", _("We"));
          
edje_object_part_text_set(label, "Wednesday", _("Th"));
          
edje_object_part_text_set(label, "Thursday", _("Fr"));
          
edje_object_part_text_set(label, "Friday", _("Sa"));
          
edje_object_part_text_set(label, "Saturday", _("Su"));
          
break;
       

case 2:
          
edje_object_part_text_set(label, "Sunday", _("Tu"));
          
edje_object_part_text_set(label, "Monday", _("We"));
          
edje_object_part_text_set(label, "Tuesday", _("Th"));
          
edje_object_part_text_set(label, "Wednesday", _("Fr"));
          
edje_object_part_text_set(label, "Thursday", _("Sa"));
          
edje_object_part_text_set(label, "Friday", _("Su"));
          
edje_object_part_text_set(label, "Saturday", _("Mo"));
          
break;
       
case 3:
          
edje_object_part_text_set(label, "Sunday", _("We"));
          
edje_object_part_text_set(label, "Monday", _("Th"));
          
edje_object_part_text_set(label, "Tuesday", _("Fr"));
          
edje_object_part_text_set(label, "Wednesday", _("Sa"));
          
edje_object_part_text_set(label, "Thursday", _("Su"));
          
edje_object_part_text_set(label, "Friday", _("Mo"));
          
edje_object_part_text_set(label, "Saturday", _("Tu"));
          
break;
       
case 4:
          
edje_object_part_text_set(label, "Sunday", _("Th"));
          
edje_object_part_text_set(label, "Monday", _("Fr"));
          
edje_object_part_text_set(label, "Tuesday", _("Sa"));
          
edje_object_part_text_set(label, "Wednesday", _("Su"));
          
edje_object_part_text_set(label, "Thursday", _("Mo"));
          
edje_object_part_text_set(label, "Friday", _("Tu"));
          
edje_object_part_text_set(label, "Saturday", _("We"));
          
break;
       
case 5:
          
edje_object_part_text_set(label, "Sunday", _("Fr"));
          
edje_object_part_text_set(label, "Monday", _("Sa"));
          
edje_object_part_text_set(label, "Tuesday", _("Su"));
          
edje_object_part_text_set(label, "Wednesday", _("Mo"));
          
edje_object_part_text_set(label, "Thursday", _("Tu"));
          
edje_object_part_text_set(label, "Friday", _("We"));
          
edje_object_part_text_set(label, "Saturday", _("Th"));
          
break;
       
case 6:
          
edje_object_part_text_set(label, "Sunday", _("Sa"));
          
edje_object_part_text_set(label, "Monday", _("Su"));
          
edje_object_part_text_set(label, "Tuesday", _("Mo"));
          
edje_object_part_text_set(label, "Wednesday", _("Tu"));
          
edje_object_part_text_set(label, "Thursday", _("We"));
          
edje_object_part_text_set(label, "Friday", _("Th"));
          
edje_object_part_text_set(label, "Saturday", _("Fr"));
          
break;
       
default:
          
break;
     
}
   
snprintf(PYear, sizeof(PYear), "%d", year);
   
edje_object_part_text_set(label, "Year1", PYear);
   
edje_object_part_text_set(label, "Year2", PYear);
   
switch (month)
      
     {
       
case 0:
          
edje_object_part_text_set(label, "Month", _("JANUARY"));
          
break;
       
case 1:
          
edje_object_part_text_set(label, "Month", _("FEBRUARY"));
          
break;
       
case 2:
          
edje_object_part_text_set(label, "Month", _("MARCH"));
          
break;
       
case 3:
          
edje_object_part_text_set(label, "Month", _("APRIL"));
          
break;
       
case 4:
          
edje_object_part_text_set(label, "Month", _("MAY"));
          
break;
       
case 5:
          
edje_object_part_text_set(label, "Month", _("JUNE"));
          
break;
       
case 6:
          
edje_object_part_text_set(label, "Month", _("JULY"));
          
break;
       
case 7:
          
edje_object_part_text_set(label, "Month", _("AUGUST"));
          
break;
       
case 8:
          
edje_object_part_text_set(label, "Month", _("SEPTEMBER"));
          
break;
       
case 9:
          
edje_object_part_text_set(label, "Month", _("OCTOBER"));
          
break;
       
case 10:
          
edje_object_part_text_set(label, "Month", _("NOVEMBER"));
          
break;
       
case 11:
          
edje_object_part_text_set(label, "Month", _("DECEMBER"));
          
break;
       
default:
          
break;
     
}


}

/***************************************************
/ Function: 
/ Purpose:  free text classes
/
*****************************************************/ 
    void
free_Calfonts(Calendar * calendar) 
{
   
Evas_List * list;
   

       //need to free Calfonts & colorarray (calendar & *char)
       for (list = calendar->conf->YM_text_class; list; list = list->next)
      
     {
        
CalFonts * font_struct;
        
font_struct = list->data;
        
evas_stringshare_del(font_struct->font);
        
font_struct->calendar = NULL;
     
}
   
evas_list_free(calendar->conf->YM_text_class);
   


for (list = calendar->conf->Day_text_class; list; list = list->next)
      
     {
        
CalFonts * font_struct;
        
font_struct = list->data;
        
evas_stringshare_del(font_struct->font);
        
font_struct->calendar = NULL;
     
}
   
evas_list_free(calendar->conf->Day_text_class);
   


for (list = calendar->conf->text_class; list; list = list->next)
      
     {
        
CalFonts * font_struct;
        
font_struct = list->data;
        
evas_stringshare_del(font_struct->font);
        
font_struct->calendar = NULL;
     
}
   
evas_list_free(calendar->conf->text_class);


}

