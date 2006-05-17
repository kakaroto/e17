#include <Enhance.h>
#include <Etk.h>

#include <string.h>

Enhance *en;

void
rip_only_clicked(Etk_Object *obj, void **data)
{
   char *foo = *data;
   printf("rip_only_clicked %s\n", foo);
}
    
int
on_window1_delete_event(void *data)
{
   etk_main_quit();
   enhance_shutdown();
   return 1;
}

int main(void)
{      
   char *string1;
   char *string2;
   char *data;
   char **data2;
   
   string1 = strdup("this is some data\n");
   string2 = strdup("we should be seeing this\n");
   
   data = string1;
   
   enhance_init();
   etk_init();
   
   en = enhance_new();      
   enhance_callback_data_set(en, "rip_only_clicked", &data);
   
   enhance_file_load(en, "window1", "combo_box.glade");
   
   data2 = enhance_callback_data_get(en, "rip_only_clicked");
   *data2 = string2;
   
   etk_main();
   
   etk_main_quit();
   etk_shutdown();
   
   enhance_free(en);  
   enhance_shutdown();
   
   return 0;
}
