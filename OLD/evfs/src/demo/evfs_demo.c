#include <evfs.h>
#include <string.h>

static int mon_current = 0;     /*A demo of stopping monitoring, after 10 events */
evfs_file_uri_path *dir_path;
evfs_connection *con;

void
callback(EvfsEvent* data, void *obj)
{
   if (data->type == EVFS_EV_FILE_MONITOR)
     {
        printf("DEMO: Received a file monitor notification\n");
        printf("DEMO: For file: '%s'\n", EVFS_EVENT_FILE_MONITOR(data)->file->path);
        mon_current++;
     }
   else if (data->type == EVFS_EV_STAT)
     {
        printf("Received stat event for file '%s'!\n",
               evfs_command_first_file_get(data->command)->path);
        printf("File size: %llu\n", EVFS_EVENT_STAT(data)->stat->st_size);
        printf("File uid: %d\n", EVFS_EVENT_STAT(data)->stat->st_uid);
        printf("File gid: %d\n", EVFS_EVENT_STAT(data)->stat->st_gid);
        printf("Last access: %d\n", EVFS_EVENT_STAT(data)->stat->ist_atime);
        printf("Last modify : %d\n", EVFS_EVENT_STAT(data)->stat->ist_mtime);
     }
   else if (data->type == EVFS_EV_DIR_LIST)
     {
        EvfsFilereference *ref;
	Eina_List* l;

        printf("Received a directory listing..\nFiles:\n\n");

        for (l = EVFS_EVENT_DIR_LIST(data)->files; l; ) 
          {
	     ref = l->data;
             printf("(%s) Received file type for file: %d\n", ref->path,
                    ref->file_type);

	     l = l->next;
          }

     } else if (data->type == EVFS_EV_METADATA)  {
	     Eina_List* l;
	     EvfsMetaObject* o;
	     
	     printf("Received metadata:\n");
	     
	     for (l = EVFS_EVENT_METADATA(data)->meta_list;l;) {		     
		     o = l->data;
		     l = l->next;
	
		     printf("%s -> %s\n", o->key, o->value);
	     }
	     
			     
	     
     } else if (data->type == EVFS_EV_METAALL) {
     	Eina_List* l;
	EvfsMetaObject* m;

	printf("Metaall event\n");
	for (l=EVFS_EVENT_META_ALL(data)->meta;l;) {
		m = l->data;
		printf("%s\n", m->key);
		l=l->next;
	}

     }

   /*if (mon_current == 2) {
    * static char str_data[1024];
    * snprintf(str_data,1024,"file://%s/newfile", getenv("HOME"));
    * 
    * evfs_file_uri_path* path = evfs_parse_uri(str_data);
    * printf("Removing monitor...\n");
    * evfs_monitor_remove(con, dir_path->files[0]);
    * 
    * printf("DEMO: Removing HOME/newfile\n");
    * evfs_client_file_remove(con, path->files[0]);
    * 
    * 
    * } */

   exit(0);
}

int
main(int argc, char **argv)
{

   char pathi[1024];
   char *patharg = NULL;
   char *cmd = NULL;
   int i;

   for (i = 1; i < argc; i++)
     {
        if (!strcmp(argv[i], "-u"))
          {
             if (++i < argc)
               {
                  patharg = strdup(argv[i]);
               }
             else
               {
                  printf("The option \"-u\" requires a valid URI\n");
                  return 1;
               }
          }
        else
          {
             if (!cmd)
               {
                  cmd = strdup(argv[i]);
               }
             else
               {
                  printf("Error: Enter only one command.\n");
                  return 1;
               }
          }

     }

   printf("EVFS Demo system..\n");

   /*Check if the user entered a command.  TODO: Add command functionality. */
   if (!cmd)
     {
        printf("You did not enter a command. Defaulting to DIR.\n");
        cmd = strdup("DIR");
     }

   if (!patharg)
     {
        snprintf(pathi, 1024, "file://%s", getenv("HOME"));
     }
   else
     {
        snprintf(pathi, 1024, "%s", patharg);
     }

   con = evfs_connect(&callback, NULL);

   //path = evfs_parse_uri("file:///dev/ttyS0");

   printf("Listing dir: %s\n", pathi);
   dir_path = evfs_parse_uri(pathi);

   printf("Plugin uri is '%s', for path '%s'\n\n",
          dir_path->files[0]->plugin_uri, dir_path->files[0]->path);

   /*evfs_monitor_add(con, dir_path->files[0]);
    * evfs_client_file_copy(con, dir_path->files[0], NULL); */

   if (!strcmp(cmd, "DIR")) {
	   evfs_client_dir_list(con, dir_path->files[0],0);
   } else if (!strcmp(cmd, "STAT")) {
	   evfs_client_file_stat(con, dir_path->files[0]);
   } else if (!strcmp(cmd, "META")) {
	   evfs_client_metadata_retrieve(con, dir_path->files[0]);
   } else if (!strcmp(cmd, "METASET")) {
	   evfs_client_metadata_string_file_set(con, dir_path->files[0], "entropy_folder_preference", "icon");
   } else if (!strcmp(cmd, "METAGET")) {
           evfs_client_metadata_string_file_get(con, dir_path->files[0], "entropy_folder_preference");
   } else if (!strcmp(cmd, "MIME")) {
	   evfs_client_mime_request(con, dir_path->files[0]);
   } else if (!strcmp(cmd, "VFOLDER")) {
	   evfs_command* cm = evfs_vfolder_create_command_new("Pictures");
	   evfs_vfolder_command_entry_add(cm, 'M', "mimetype","image/jpeg");
	   evfs_vfolder_command_send(con,cm);
   } else if (!strcmp(cmd, "METAALL")) {
   	   evfs_client_meta_list_all(con);
   }
   ecore_main_loop_begin();
   evfs_disconnect(con);
}
