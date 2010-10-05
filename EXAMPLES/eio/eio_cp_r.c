#include <Ecore.h>
#include <Eio.h>

static void
_test_notify_cb(void *data, const Eio_Progress *info)
{
   switch (info->op)
     {
      case EIO_FILE_COPY:
         printf("[%s] %f%%\n", info->dest, info->percent);
         break;
      case EIO_DIR_COPY:
         printf("global [%li/%li] %f%%\n", info->current, info->max, info->percent);
         break;
     }
}

static void
_test_done_cb(void *data)
{
   printf("copy done\n");
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

   cp = eio_dir_copy(argv[1], argv[2],
                     _test_notify_cb,
                     _test_done_cb,
                     _test_error_cb,
                     NULL);

   ecore_main_loop_begin();

   eio_shutdown();
   ecore_shutdown();

   return 0;
}
