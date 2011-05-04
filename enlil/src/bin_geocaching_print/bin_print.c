#include <Ecore_Getopt.h>
#include "Enlil.h"

#include "../../config.h"

static void _done_cb(void      *data,
                     Eina_Hash *db);

static const Ecore_Getopt options = {
   "Print the lis of geocaching way points",
   NULL,
   VERSION,
   "(C) 2009 Test Enlil Photo manager, see AUTHORS.",
   "LGPL with advertisement, see COPYING",
   "\n\n",
   1,
   {
      ECORE_GETOPT_VERSION('V', "version"),
      ECORE_GETOPT_COPYRIGHT('R', "copyright"),
      ECORE_GETOPT_LICENSE('L', "license"),
      ECORE_GETOPT_STORE_STR('i', "import", "Import the specified gpx file in the database and print the result."),
      ECORE_GETOPT_HELP('h', "help"),
      ECORE_GETOPT_SENTINEL
   }
};

int
main(int    argc,
     char **argv)
{
   unsigned char exit_option = 0;
   char *file = NULL;

   enlil_init();

   //ecore_getopt
   Ecore_Getopt_Value values[] = {
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_STR(file),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
   };
   ecore_app_args_set(argc, (const char **)argv);
   int nonargs = ecore_getopt_parse(&options, values, argc, argv);
   if (nonargs < 0)
     return 1;
   else if (nonargs != argc)
     {
        fputs("Invalid non-option argument", stderr);
        ecore_getopt_help(stderr, &options);
        return 1;
     }

   if(exit_option)
     return 0;
   //

   if(file)
     enlil_geocaching_import(file, _done_cb, NULL);
   else
     enlil_geocaching_get(_done_cb, NULL);

   ecore_main_loop_begin();

   enlil_shutdown();

   return 0;
}

static Eina_Bool
_print_cb(const Eina_Hash *hash __UNUSED__,
          const void      *key __UNUSED__,
          void            *data,
          void            *fdata __UNUSED__)
{
   Eina_List *l;
   Enlil_Geocaching_Log *log;
   Enlil_Geocaching_Travelbug *tb;
   Enlil_Geocaching *gp = data;

   printf("########################\n");
   printf("name : %s\n", enlil_geocaching_name_get(gp));
   printf("time : %s\n", enlil_geocaching_time_get(gp));
   printf("desc : %s\n", enlil_geocaching_description_get(gp));
   printf("url  : %s\n", enlil_geocaching_url_get(gp));
   printf("url_name  : %s\n", enlil_geocaching_url_name_get(gp));
   printf("sym  : %s\n", enlil_geocaching_sym_get(gp));
   printf("type  : %s\n", enlil_geocaching_type_get(gp));
   printf("gp_id : %s\n", enlil_geocaching_gp_id_get(gp));
   printf("gp_available  : %s\n", enlil_geocaching_gp_available_get(gp));
   printf("gp_archived  : %s\n", enlil_geocaching_gp_archived_get(gp));
   printf("gp name : %s\n", enlil_geocaching_gp_name_get(gp));
   printf("longitude : %f\n", enlil_geocaching_longitude_get(gp));
   printf("latitude : %f\n", enlil_geocaching_latitude_get(gp));
   printf("gp short_desc : %s\n", enlil_geocaching_gp_short_desc_get(gp));
   printf("gp long_desc : %s\n", enlil_geocaching_gp_long_desc_get(gp));
   printf("gp hints : %s\n", enlil_geocaching_gp_hints_get(gp));
   printf("gp state : %s\n", enlil_geocaching_gp_state_get(gp));
   printf("gp placed_by : %s\n", enlil_geocaching_gp_placed_by_get(gp));
   printf("gp owner (%s): %s\n", enlil_geocaching_gp_owner_get(gp), enlil_geocaching_gp_owner_id_get(gp));
   printf("gp type : %s\n", enlil_geocaching_gp_type_get(gp));
   printf("gp container : %s\n", enlil_geocaching_gp_container_get(gp));
   printf("gp difficulty : %s\n", enlil_geocaching_gp_difficulty_get(gp));
   printf("gp terrain : %s\n", enlil_geocaching_gp_terrain_get(gp));
   printf("gp country : %s\n", enlil_geocaching_gp_country_get(gp));

   printf("\nLOGS ! (%d)\n", eina_list_count(enlil_geocaching_logs_get(gp)));
   EINA_LIST_FOREACH(enlil_geocaching_logs_get(gp), l, log)
     {
        printf("\tid : %s\n", enlil_geocaching_log_id_get(log));
        printf("\tdate : %s\n", enlil_geocaching_log_date_get(log));
        printf("\ttype : %s\n", enlil_geocaching_log_type_get(log));
        printf("\tfinder : %s\n", enlil_geocaching_log_finder_get(log));
        printf("\tfinder id : %s\n", enlil_geocaching_log_finder_id_get(log));
        printf("\ttext (%s) : %s\n", enlil_geocaching_log_encoded_get(log),
               enlil_geocaching_log_text_get(log));
     }

   printf("\nTRAVELBUGS ! (%d)\n", eina_list_count(enlil_geocaching_tbs_get(gp)));
   EINA_LIST_FOREACH(enlil_geocaching_tbs_get(gp), l, tb)
     {
        printf("\tid : %s\n", enlil_geocaching_tb_id_get(tb));
        printf("\tref : %s\n", enlil_geocaching_tb_ref_get(tb));
        printf("\tname : %s\n", enlil_geocaching_tb_name_get(tb));
     }

   return EINA_TRUE;
}

static void
_done_cb(void      *data __UNUSED__,
         Eina_Hash *db)
{
   eina_hash_foreach(db, _print_cb, NULL);

   printf("\n\n Elements count : %d\n", eina_hash_population(db));

   ecore_main_loop_quit();
}

