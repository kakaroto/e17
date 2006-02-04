#include <evfs.h>
#include <string.h>

evfs_connection *con;

void
callback(evfs_event * data, void *obj)
{

   switch (data->type)
     {
     case EVFS_EV_FILE_PROGRESS:
	printf(".");
	
	if (data->progress->type == EVFS_PROGRESS_TYPE_DONE) {
		evfs_disconnect(con);
		exit(0);
	}
        break;

     default:
        printf("Unknown event\n");
        break;
     }
}

int
main(int argc, char **argv)
{

   evfs_file_uri_path *path_from;
   evfs_file_uri_path *path_to;
   char pathi[1024];

   if (argc < 3)
     {
        return 0;
     }

   if ((con = evfs_connect(&callback, NULL)))
     {
        path_from = evfs_parse_uri(argv[1]);
	path_to = evfs_parse_uri(argv[2]);
	

        evfs_client_file_copy(con, path_from->files[0], path_to->files[0]);
     }
   else
     {
        printf("evfscopy: failure to connect to evfs server\n");
     }

   ecore_main_loop_begin();
   

   return 0;
}
