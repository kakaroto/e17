
#include <Azy.h>
#include "Demo_HowAreYou.azy_client.h"

#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static Eina_Error
_put(Azy_Client *cli, Azy_Content *content, void *ret)
{
    if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}


    printf("File uploaded !\n");

    return AZY_ERROR_NONE;
}

static void _upload(Azy_Client *cli)
{
    //this code is closed to the code of __download__
    const char *file = "./FILE_GET.jpg";

    printf("Sending the file %s\n", file);
    /*
       we are gonna use mmap to set the image as content
       */
    //copy the file content and sent it
    int fd = open(file, O_RDONLY);
    struct stat st;
    fstat(fd, &st); /* use fstat to get the size */
    unsigned char *content_file = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);

    Azy_Net_Data azy_data;
    azy_data.data = content_file;
    azy_data.size = st.st_size;

    //upload
    azy_net_uri_set(azy_client_net_get(cli), "/Photo/Put/4");
    Azy_Client_Call_Id id = azy_client_put(cli, &azy_data, NULL);
    azy_client_callback_set(cli, id, _put);
    //

    munmap(content_file, st.st_size); /* unmap mmapped data */
    close(fd);
}

/**
 * This method is called when we have received the photo
 */
static Eina_Error
_get(Azy_Client *cli, Azy_Content *content, void *ret)
{
	const char *FILEPATH = "./FILE_GET.jpg";
	int fd;
	int *map;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}

        //retrieve the size and the content
	int size = azy_content_return_size_get(content);
	unsigned char* data = (unsigned char*)azy_content_return_get(content);

        printf("FILE RECEIVED\n");
	printf("Write file in %s\n", FILEPATH);
	printf("File size : %d\n", size);

        //open the output file
	fd = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
        //write at the end of the file in order to really create it.
	lseek(fd, size-1, SEEK_SET);
	write(fd, "", 1);
	map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        //copy the content into the output file
	memcpy(map, data, size);

	munmap(map, size);
	close(fd);

        //upload the received file to the server
        _upload(cli);

	return AZY_ERROR_NONE;
}

/**
 * Bad we have been disconnected
 */
static Eina_Bool _disconnected(void *data, int type, void *data2)
{
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

/**
 * Yes we are connected !
 */
static Eina_Bool
connected(void *data, int type, Azy_Client *cli)
{
   Azy_Client_Call_Id id;

   //we ask the photo id to the server
   azy_net_uri_set(azy_client_net_get(cli), "/Photo/Get/4");
   azy_net_version_set(azy_client_net_get(cli), 0); /* disconnect after sending */
   id = azy_client_blank(cli, AZY_NET_TYPE_GET, NULL, NULL, NULL);
   azy_client_callback_set(cli, id, _get);

   return ECORE_CALLBACK_RENEW;
}

int main(int argc, char *argv[])
{
	   Ecore_Event_Handler *handler;
	   Azy_Client *cli;

	   azy_init();

           //eina_log_domain_level_set("azy", EINA_LOG_LEVEL_DBG);

	   /* create object for performing client connections */
	   cli = azy_client_new();

	   if (!azy_client_host_set(cli, "127.0.0.1", 3412))
	     return 1;

	   handler = ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)connected, cli);
	   handler = ecore_event_handler_add(AZY_CLIENT_DISCONNECTED, (Ecore_Event_Handler_Cb)_disconnected, cli);

	   /* connect to the servlet on the server specified by uri */
	   if (!azy_client_connect(cli, EINA_FALSE))
	     return 1;

	   ecore_main_loop_begin();

	   azy_client_free(cli);
	   azy_shutdown();
	   ecore_shutdown();
	   eina_shutdown();

	   return 0;
}
