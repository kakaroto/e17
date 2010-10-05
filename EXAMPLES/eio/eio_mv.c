#include <Ecore.h>
#include <Eio.h>

static void
_test_done_cb(void *data)
{
   printf("move done\n");
   ecore_main_loop_quit();
}

static void
_test_error_cb(int error, void *data)
{
   fprintf(stderr, "error: [%s]\n", strerror(error));
   ecore_main_loop_quit();
}

int
main(int argc, char **argv)
{
   Eio_File *cp;

   if (argc != 3)
     {
	fprintf(stderr, "eio_cp source_file destination_file\n");
	return -1;
     }

   ecore_init();
   eio_init();

   cp = eio_file_move(argv[1], argv[2],
		      NULL,
		      _test_done_cb,
		      _test_error_cb,
		      NULL);

   ecore_main_loop_begin();

   eio_shutdown();
   ecore_shutdown();

   return 0;
}
