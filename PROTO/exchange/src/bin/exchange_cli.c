/* EXCHANGE - a library to interact with exchange.enlightenment.org
 * Copyright (C) 2008 Massimiliano Calamelli
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Exchange.h>
#include <getopt.h>

static void help(void);

int
main(int argc, char **argv)
{
   int c;
   char *target, *object, *request;

   char *local_theme_req[] = { "name",
                                "author",
                                "version",
                                "license",
                                "updates" };

   char *remote_theme_req[] = { "id",
                                 "author",
                                 "license",
                                 "version",
                                 "description",
                                 "url",
                                 "thumbnail_url",
                                 "screenshot_url",
                                 "rating",
                                 "user_id",
                                 "created_at",
                                 "updated_at",
                                 "all_data",
                                 "list_by_user_id",
                                 "list_by_user_name",
                                 "list_by_theme_group_title",
                                 "list_by_theme_group_name",
                                 "list_by_application_id",
                                 "list_by_module_id" };

   char *remote_login_req[] = { "api_key",
                                 "all_data" };
                                 
   char *theme_group_req[] = { "list" };
   
   char *application_list_req[] = { "list_by_user_id",
                                     "list_by_user_name" };

   char *module_list_req[] = { "list_by_user_id",
                                "list_by_user_name",
                                "list_by_application_id" };
                                     
   exchange_init();
   target = object = request = NULL;

   if (argc < 2)
   {
      help();
      exchange_shutdown();
      return 0;
   }

	static struct option opts[] = {
      {"help", no_argument, 0, 'h'},
      {"target", required_argument, 0, 't'},
      {"object", required_argument, 0, 'o'},
      {"request", required_argument, 0, 'r'},
      {0, 0, 0, 0}
	};

   while((c = getopt_long_only(argc, argv, "ho:r:t:", opts, NULL)) != -1)
   {
      switch(c)
      {
         case 'h':
            help();
            break;
         case 'o':
            object = strdup(optarg);
            break;
         case 'r':
            request = strdup(optarg);
            break;
         case 't':
            target = strdup(optarg);
            break;
         default:
            help();
      }
   }

   if ((!target)||(!object)||(!request))
      printf("Missing option! See help for details\n");
   else if ((strcmp(target, "remote"))&&(strcmp(target, "local")))
      printf("Invalid target argument.\n");
   else if ((strcmp(object, "theme"))&&(strcmp(object, "login"))&&(strcmp(object, "theme_group"))&&(strcmp(object, "application"))&&(strcmp(object, "module")))
      printf("Invalid object argument.\n");
   else
   {
      if ((argc < 8)&&(strcmp(object, "theme_group")))
         printf("Missing <resource>.\n");
      else
      {
         if (!strcmp(target, "local"))
         {
            int i = 0;
            int found = 0;
            for (i = 0; i < sizeof(local_theme_req)/sizeof(local_theme_req[0]); i++)
            {
               if (!strcmp(local_theme_req[i], request))
               {
                  char *out;

                  found = 1;

                  if (!strcmp(request, "name"))
                  {
                     out = exchange_local_theme_name_get(argv[7]);
                     printf("Theme: %s, name: %s\n", argv[7], out);
                     free(out);
                  }
                  if (!strcmp(request, "author"))
                  {
                     out = exchange_local_theme_author_get(argv[7]);
                     printf("Theme: %s, author: %s\n", argv[7], out);
                     free(out);
                  }
                  if (!strcmp(request, "version"))
                  {
                     out = exchange_local_theme_version_get(argv[7]);
                     printf("Theme: %s, version: %s\n", argv[7], out);
                     free(out);
                  }
                  if (!strcmp(request, "license"))
                  {
                     out = exchange_local_theme_license_get(argv[7]);
                     printf("Theme: %s, license: %s\n", argv[7], out);
                     free(out);
                  }
                  if (!strcmp(request, "updates"))
                  {
                     char text[40];
                     int update;

                     update = exchange_local_theme_check_update(argv[7]);
                     switch (update)
                     {
                        case -1:
                           snprintf(text, sizeof(text), "%s", "Selected theme don't have version entry");
                           break;
                        case 0:
                           snprintf(text, sizeof(text), "%s", "none");
                           break;
                        case 1:
                           snprintf(text, sizeof(text), "%s", "YES!");
                           break;
                     }
                     printf("Theme: %s, update available: %s\n", argv[7], text);
                  }
               }
            }
            if (!found)
               printf("The request argument isn't valid for local target.\n");
         }
         else if ((!strcmp(target, "remote"))&&(!strcmp(object, "theme")))
         {
            int i = 0;
            int found = 0;
            for (i = 0; i < sizeof(remote_theme_req)/sizeof(remote_theme_req[0]); i++)
            {
               if (!strcmp(remote_theme_req[i], request))
               {
                  found = 1;

                  if (!strcmp(request, "id"))
                     printf("Theme: %s, id: %d\n", argv[7], exchange_remote_theme_id_get(argv[7]));
                  if (!strcmp(request, "author"))
                     printf("Theme: %s, author: %s\n", argv[7], exchange_remote_theme_author_get(argv[7]));
                  if (!strcmp(request, "license"))
                     printf("Theme: %s, license: %s\n", argv[7], exchange_remote_theme_license_get(argv[7]));
                  if (!strcmp(request, "version"))
                     printf("Theme: %s, version: %s\n", argv[7], exchange_remote_theme_version_get(argv[7]));
                  if (!strcmp(request, "description"))
                     printf("Theme: %s, description: %s\n", argv[7], exchange_remote_theme_description_get(argv[7]));
                  if (!strcmp(request, "url"))
                     printf("Theme: %s, url: %s\n", argv[7], exchange_remote_theme_url_get(argv[7]));
                  if (!strcmp(request, "thumbnail_url"))
                     printf("Theme: %s, thumbnail: %s\n", argv[7], exchange_remote_theme_thumbnail_url_get(argv[7]));
                  if (!strcmp(request, "screenshot_url"))
                     printf("Theme: %s, screenshot: %s\n", argv[7], exchange_remote_theme_screenshot_url_get(argv[7]));
                  if (!strcmp(request, "rating"))
                     printf("Theme: %s, rating: %f\n", argv[7], exchange_remote_theme_rating_get(argv[7]));
                  if (!strcmp(request, "user_id"))
                     printf("Theme: %s, user id: %d\n", argv[7], exchange_remote_theme_user_id_get(argv[7]));
                  if (!strcmp(request, "created_at"))
                     printf("Theme: %s, created at: %s\n", argv[7], exchange_remote_theme_created_get(argv[7]));
                  if (!strcmp(request, "updated_at"))
                     printf("Theme: %s, updated at: %s\n", argv[7], exchange_remote_theme_updated_get(argv[7]));
                  if (!strcmp(request, "all_data"))
                  {
                        Theme_Data *tdata;
                        tdata = (Theme_Data *)exchange_remote_theme_all_data_get(argv[7]);
                        printf("Theme: %s\n", argv[7]);
                        printf("ID: %d\n", tdata->id);
                        printf("Name: %s\n", tdata->name);
                        printf("Author: %s\n", tdata->author);
                        printf("License: %s\n", tdata->license);
                        printf("Version: %s\n", tdata->version);
                        printf("Description: %s\n", tdata->description);
                        printf("Url: %s\n", tdata->url);
                        printf("Thumbnail: %s\n", tdata->thumbnail);
                        printf("Screenshot: %s\n", tdata->screenshot);
                        printf("Rating: %f\n", tdata->rating);
                        printf("User ID: %d\n", tdata->user_id);
                        printf("Created at: %s\n", tdata->created_at);
                        printf("Updated at: %s\n", tdata->updated_at);
                  }
                  if (!strcmp(request, "list_by_user_id"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_theme_list_filter_by_user_id(atoi(argv[7]), 0, 0);

                     printf("Themes from user ID #%d (%d):\n", atoi(argv[7]), evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Theme_List_Data *tld;
                           tld = (Theme_List_Data *)l1->data;
                           printf("Name: %s\n", (char *)tld->name);
                           printf("Description: %s\n", (char *)tld->description);
                           printf("Version: %s\n", (char *)tld->version);
                           printf("URL: %s\n", (char *)tld->url);
                           printf("Screenshot: %s\n", (char *)tld->screenshot);
                        }
                     }
                  }
                  if (!strcmp(request, "list_by_user_name"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_theme_list_filter_by_user_name(argv[7], 0, 0);

                     printf("Themes from user name \"%s\" (%d):\n", argv[7], evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Theme_List_Data *tld;
                           tld = (Theme_List_Data *)l1->data;
                           printf("Name: %s\n", (char *)tld->name);
                           printf("Description: %s\n", (char *)tld->description);
                           printf("Version: %s\n", (char *)tld->version);
                           printf("URL: %s\n", (char *)tld->url);
                           printf("Screenshot: %s\n", (char *)tld->screenshot);
                        }
                     }
                  }
                  if (!strcmp(request, "list_by_theme_group_title"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_theme_list_filter_by_group_title(argv[7], 0, 0);

                     printf("Themes that provides \"%s\" as theme_group title (%d):\n", argv[7], evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Theme_List_Data *tld;
                           tld = (Theme_List_Data *)l1->data;
                           printf("Name: %s\n", (char *)tld->name);
                           printf("Description: %s\n", (char *)tld->description);
                           printf("Version: %s\n", (char *)tld->version);
                           printf("URL: %s\n", (char *)tld->url);
                           printf("Screenshot: %s\n", (char *)tld->screenshot);
                        }
                     }
                  }
                  if (!strcmp(request, "list_by_theme_group_name"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_theme_list_filter_by_group_name(argv[7], 0, 0);

                     printf("Themes that provides \"%s\" as theme_group name (%d):\n", argv[7], evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Theme_List_Data *tld;
                           tld = (Theme_List_Data *)l1->data;
                           printf("Name: %s\n", (char *)tld->name);
                           printf("Description: %s\n", (char *)tld->description);
                           printf("Version: %s\n", (char *)tld->version);
                           printf("URL: %s\n", (char *)tld->url);
                           printf("Screenshot: %s\n", (char *)tld->screenshot);
                        }
                     }
                  }
                  if (!strcmp(request, "list_by_application_id"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_theme_list_filter_by_application_id(atoi(argv[7]), 0, 0);

                     printf("Themes that matches the provided application id #%d (%d):\n", atoi(argv[7]), evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Theme_List_Data *tld;
                           tld = (Theme_List_Data *)l1->data;
                           printf("Name: %s\n", (char *)tld->name);
                           printf("Description: %s\n", (char *)tld->description);
                           printf("Version: %s\n", (char *)tld->version);
                           printf("URL: %s\n", (char *)tld->url);
                           printf("Screenshot: %s\n", (char *)tld->screenshot);
                        }
                     }
                  }
                  if (!strcmp(request, "list_by_module_id"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_theme_list_filter_by_module_id(atoi(argv[7]), 0, 0);

                     printf("Themes that matches the provided module id #%d (%d):\n", atoi(argv[7]), evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Theme_List_Data *tld;
                           tld = (Theme_List_Data *)l1->data;
                           printf("Name: %s\n", (char *)tld->name);
                           printf("Description: %s\n", (char *)tld->description);
                           printf("Version: %s\n", (char *)tld->version);
                           printf("URL: %s\n", (char *)tld->url);
                           printf("Screenshot: %s\n", (char *)tld->screenshot);
                        }
                     }
                  }
               }
            }
            if (!found)
               printf("The request argument isn't valid for remote target and theme request.\n");
         }
         else if ((!strcmp(target, "remote"))&&(!strcmp(object, "login")))
         {
            int i = 0;
            int found = 0;
            for (i = 0; i < sizeof(remote_login_req)/sizeof(remote_login_req[0]); i++)
            {
               if (!strcmp(remote_login_req[i], request))
               {
                  found = 1;

                  if (!strcmp(request, "api_key"))
                     printf("User: %s, API key: %s\n", argv[7], exchange_login_api_key_get(argv[7], argv[8]));
                  if (!strcmp(request, "all_data"))
                     {
                        Login_Data *ldata;
                        ldata = (Login_Data *)exchange_login(argv[7], argv[8]);
                        printf("ID: %d\n", ldata->id);
                        printf("Name: %s\n", ldata->name);
                        printf("Email: %s\n", ldata->email);
                        printf("Role: %s\n", ldata->role);
                        printf("API key: %s\n", ldata->api_key);
                        printf("Created at: %s\n", ldata->created_at);
                        printf("Updated at: %s\n", ldata->updated_at);
                     }
               }
            }
            if (!found)
               printf("The request argument isn't valid for remote target and login request.\n");
         }
         else if ((!strcmp(target, "remote"))&&(!strcmp(object, "theme_group")))
         {
            int i = 0;
            int found = 0;
            for (i = 0; i < sizeof(theme_group_req)/sizeof(theme_group_req[0]); i++)
            {
               if (!strcmp(theme_group_req[i], request))
               {
                  found = 1;

                  if (!strcmp(request, "list"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_theme_group_list_available();

                     printf("Available theme_groups (%d):\n", evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Theme_Group_Data *tgd;
                           tgd = (Theme_Group_Data *)l1->data;
                           printf("[Name] %s, [Title] %s\n", (char *)tgd->name, (char *)tgd->title);
                        }
                     }
                  }
               }
            }
            if (!found)
               printf("The request argument isn't valid for remote target and login request.\n");
         }
         else if ((!strcmp(target, "remote"))&&(!strcmp(object, "application")))
         {
            int i = 0;
            int found = 0;
            for (i = 0; i < sizeof(application_list_req)/sizeof(application_list_req[0]); i++)
            {
               if (!strcmp(application_list_req[i], request))
               {
                  found = 1;

                  if (!strcmp(request, "list_by_user_id"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_application_list_filter_by_user_id(atoi(argv[7]), 0, 0);

                     printf("Applications from user ID #%d (%d):\n", atoi(argv[7]), evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Application_List_Data *ald;
                           ald = (Application_List_Data *)l1->data;
                           printf("ID: %d\n", ald->id);
                           printf("Name: %s\n", (char *)ald->name);
                           printf("Description: %s\n", (char *)ald->description);
                           printf("URL: %s\n", (char *)ald->url);
                           printf("Screenshot: %s\n", (char *)ald->screenshot);
                           printf("User ID: %d\n", ald->user_id);
                        }
                     }
                  }
                  if (!strcmp(request, "list_by_user_name"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_application_list_filter_by_user_name(argv[7], 0, 0);

                     printf("Applications from user name \"%s\" (%d):\n", argv[7], evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Application_List_Data *ald;
                           ald = (Application_List_Data *)l1->data;
                           printf("ID: %d\n", ald->id);
                           printf("Name: %s\n", (char *)ald->name);
                           printf("Description: %s\n", (char *)ald->description);
                           printf("URL: %s\n", (char *)ald->url);
                           printf("Screenshot: %s\n", (char *)ald->screenshot);
                           printf("User ID: %d\n", ald->user_id);
                        }
                     }
                  }
               }
            }
         }
         //**//
         else if ((!strcmp(target, "remote"))&&(!strcmp(object, "module")))
         {
            int i = 0;
            int found = 0;
            for (i = 0; i < sizeof(module_list_req)/sizeof(module_list_req[0]); i++)
            {
               if (!strcmp(module_list_req[i], request))
               {
                  found = 1;

                  if (!strcmp(request, "list_by_user_id"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_module_list_filter_by_user_id(atoi(argv[7]), 0, 0);

                     printf("Modules from user ID #%d (%d):\n", atoi(argv[7]), evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Module_List_Data *mld;
                           mld = (Module_List_Data *)l1->data;
                           printf("ID: %d\n", mld->id);
                           printf("Name: %s\n", (char *)mld->name);
                           printf("Description: %s\n", (char *)mld->description);
                           printf("URL: %s\n", (char *)mld->url);
                           printf("Screenshot: %s\n", (char *)mld->screenshot);
                           printf("User ID: %d\n", mld->user_id);
                           printf("Application ID: %d\n", mld->application_id);
                        }
                     }
                  }
                  if (!strcmp(request, "list_by_user_name"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_module_list_filter_by_user_name(argv[7], 0, 0);

                     printf("Modules from user name \"%s\" (%d):\n", argv[7], evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Module_List_Data *mld;
                           mld = (Module_List_Data *)l1->data;
                           printf("ID: %d\n", mld->id);
                           printf("Name: %s\n", (char *)mld->name);
                           printf("Description: %s\n", (char *)mld->description);
                           printf("URL: %s\n", (char *)mld->url);
                           printf("Screenshot: %s\n", (char *)mld->screenshot);
                           printf("User ID: %d\n", mld->user_id);
                           printf("Application ID: %d\n", mld->application_id);
                        }
                     }
                  }
                  if (!strcmp(request, "list_by_application_id"))
                  {
                     Evas_List *l, *l1;
                     l = exchange_module_list_filter_by_application_id(atoi(argv[7]), 0, 0);

                     printf("Modules for application ID #%d (%d):\n", atoi(argv[7]), evas_list_count(l));
                     for (l1 = l; l1; l1 = evas_list_next(l1))
                     {
                        if (l1->data)
                        {
                           Module_List_Data *mld;
                           mld = (Module_List_Data *)l1->data;
                           printf("ID: %d\n", mld->id);
                           printf("Name: %s\n", (char *)mld->name);
                           printf("Description: %s\n", (char *)mld->description);
                           printf("URL: %s\n", (char *)mld->url);
                           printf("Screenshot: %s\n", (char *)mld->screenshot);
                           printf("User ID: %d\n", mld->user_id);
                           printf("Application ID: %d\n", mld->application_id);
                        }
                     }
                  }
               }
            }
         }
         //**//
      }
   }

   if (target)
      free(target);
   if (object)
      free(object);
   if (request)
      free(request);

   exchange_shutdown();
   return 0;
}

static void
help(void)
{
   printf("Usage: exchange_cli [options] <resource>\n\n"
          "Options:\n"
          "-t --target=local|remote\t\t\t The target of the object\n"
          "-o --object=theme|login|theme_group|application\t The type of object\n"
          "-r --request\t\t\t\t\t The requested data (see below)\n"
          "<resource>\t\t\t\t\t See below\n\n"
          "Requests for theme object and local target:\n"
          "name:\t\t The name of the theme\n"
          "author:\t\t The author of the theme\n"
          "version:\t The versionof the theme\n"
          "license:\t The license of the theme\n"
          "updates:\t Check for updates of local theme\n\n"
          "Requests for theme object and remote target:\n"
          "id:\t\t The id of the theme\n"
          "author:\t\t The author of the theme\n"
          "license:\t The license of the theme\n"
          "version:\t The version of the theme\n"
          "description:\t The description of the theme\n"
          "url:\t\t The URL of the theme\n"
          "thumbnail_url:\t The thumbnail URL of the theme\n"
          "screenshot_url:\t The screenshot URL of the theme\n"
          "user_id:\t The id of the author\n"
          "created_at:\t The creation timestamp\n"
          "updated_at:\t The last update timestamp\n"
          "all_data:\t A All available data\n"
          "list_by_user_id:\t\t A list of themes that matches the supplied user ID\n"
          "list_by_user_name:\t\t A list of themes that matches the supplied user name\n"
          "list_by_theme_group_title:\t A list of themes that matches the supplied theme_group title\n"
          "list_by_theme_group_name:\t A list of themes that matches the supplied theme_group name\n"
          "list_by_application_id:\t\t A list of themes that matches the supplied application id\n"
          "list_by_module_id:\t\t A list of themes that matches the supplied module id\n\n"
          "Requests for login object and remote target:\n"
          "api_key:\t The API key for the user\n"
          "all_data:\t A Login_Data pointer that contains all available data\n\n"
          "Requests for theme_group object and remote target:\n"
          "list:\t A list of available theme_groups\n\n"
          "Requests for application object and remote target:\n"
          "list_by_user_id:\t A list of applications that matches the supplied user ID\n"
          "list_by_user_name:\t A list of applications that matches the supplied user name\n\n"
          "Requests for module object and remote target:\n"
          "list_by_user_id:\t A list of modules that matches the supplied user ID\n"
          "list_by_user_name:\t A list of modules that matches the supplied user name\n"
          "list_by_application_id:\t A list of modules that matches the supplied application ID\n\n"
          "Resources:\n"
          "theme object, local target\t\t\t\t The path of theme file\n"
          "theme object, remote target\t\t\t\t The name of the theme\n"
          "theme object, remote target, list_by_xxxx request\t One of available filter (see above)\n"
          "login object, remote target\t\t\t\t The username and the password of the user\n"
          "application object, remote target, list_by_xxxx request\t One of available filter (see above)\n"
          "module object, remote target, list_by_xxxx request\t One of available filter (see above)\n"
         );
}
