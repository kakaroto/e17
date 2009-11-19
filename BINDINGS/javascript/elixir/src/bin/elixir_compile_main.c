#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <Eet.h>

#include "Elixir.h"

static const char *loader[] = {
  "eet",
  NULL
};

static const char *security[] = {
  "let",
  NULL
};

int
main(int argc, char **argv)
{
   Elixir_Runtime *er;
   Elixir_Script *es;
   Eet_File *eet;
   char *bytes;
   char *key;
   unsigned int length;

   if (argc != 3 && argc != 4)
     return 255;

   elixir_debug_print_switch(ELIXIR_DEBUG_STDERR, NULL);

   elixir_id_init();
   elixir_class_init();
   elixir_file_init();
   elixir_modules_init();
   er = elixir_init();
   if (!er) return 255;

   elixir_security_init(1, security);
   elixir_loader_init(1, loader);

   es = elixir_script_file(er, argc - 1, (const char**) argv + 1);
   if (!es) return 255;
   bytes = elixir_script_binary(es, &length);
   if (!bytes) return 255;

   eet = eet_open(argv[1], EET_FILE_MODE_READ_WRITE);
   if (!eet) return 255;

   key = alloca(strlen(argv[2]) + 2);
   snprintf(key, strlen(argv[2]) + 2, "%sx", argv[2]);

   if (argc == 3) eet_write(eet, key, bytes, length, 1);
   else eet_write_cipher(eet, key, bytes, length, 1, argv[3]);

   eet_close(eet);

   return 0;
}
