#ifndef PLUGIN_PRIVATE_H_
#define PLUGIN_PRIVATE_H_

/*
 * = INTERNAL STUFF =
 */

/* 
 * _Plist_Data struct:
 * used internally to store plugin list and path groups for later use
 */
typedef struct _Plist_Data
{
   Ecore_List *plugin_list;
   Ecore_Path_Group *pg;
   Evas_List **startup_errors;
} Plist_Data;

Plist_Data *plugin_init(Evas_List **startup_errors);

#endif /*PLUGIN_PRIVATE_H_*/
