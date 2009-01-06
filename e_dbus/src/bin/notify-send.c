#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <getopt.h>
#include <E_Notify.h>

#include "config.h"

void
version(void)
{
  printf("e-notify-send "VERSION"\n");
}

void
usage(void)
{
  printf("Usage:\n"
         "  e-notify-send [OPTION...] <SUMMARY> [BODY] - create a notification\n"
         "\n"
         "Help Options:\n"
         "  -?, --help                        Show help options\n"
         "\n"
         "Application Options:\n"
         "  -n, --name=NAME                   Specifies the application name to use (default is e-notify-send).\n"
         "  -u, --urgency=LEVEL               Specifies the urgency level (low, normal, critical).\n"
         "  -t, --expire-time=TIME            Specifies the timeout in milliseconds at which to expire the notification.\n"
         "  -r, --replace=ID                  Specifies the ID of notification to replace.\n"
         "  -p, --print-id                    Prints the ID of notification to STDOUT.\n"
         "  -i, --icon=ICON                   Specifies an icon filename or stock icon to display.\n"
         "  -c, --category=TYPE               Specifies the notification category.\n"
         "  -v, --version                     Version of the package.\n"
         "\n");
}

int
read_int_arg(long long *result, const char *name, intmax_t min, intmax_t max)
{
  char *endptr;

  errno = 0;  
  *result = strtoll(optarg, &endptr, 10);
  if ((errno != 0 && *result == 0) || endptr == optarg) 
    {
       fprintf(stderr, "Cannot parse integer value '%s' for %s\n", optarg, name);
       return 0;
    }
  else if (*result > max || *result < min)
    {
       fprintf(stderr, "Integer value '%s' for %s out of range\n", optarg, name);
       return 0;
    }

  return 1;
}

void 
send_cb(void *user_data, void *method_return, DBusError *error)
{
   E_Notification_Return_Notify *r = method_return;

   if(!r)
     return;

   printf("%u\n", r->notification_id );

   ecore_main_loop_quit();
}

int
main(int argc, char **argv)
{
  int ch;
  long long value;
  int print_id = 0;
  E_Notification *n;

  e_notification_init();
  n = e_notification_new();
  e_notification_app_name_set(n, "e-notify-send");
  e_notification_timeout_set(n, -1);

  /* options descriptor */
  static struct option longopts[] = {
      { "help",        no_argument,            NULL,           '?' },
      { "name",        required_argument,      NULL,           'n' },
      { "urgency",     required_argument,      NULL,           'u' },
      { "expire-time", required_argument,      NULL,           't' },
      { "replace",     required_argument,      NULL,           'r' },
      { "print-id",    no_argument,            NULL,           'p' },
      { "icon",        required_argument,      NULL,           'i' },
      { "category",    required_argument,      NULL,           'c' },
      { "version",     no_argument,            NULL,           'v' },
      { NULL,          0,                      NULL,             0 }
  };

  while ((ch = getopt_long(argc, argv, "p?vn:u:t:r:i:c:", longopts, NULL)) != -1)
    switch (ch) {
    case '?':
      usage();
      return EXIT_SUCCESS;
      break;
    case 'v':
      version();
      return EXIT_SUCCESS;
      break;
    case 'n':
      e_notification_app_name_set(n, optarg);
      break;
    case 'u':
      if (!strcasecmp(optarg, "low"))
        e_notification_hint_urgency_set(n, E_NOTIFICATION_URGENCY_LOW);
      else if (!strcasecmp(optarg, "normal"))
        e_notification_hint_urgency_set(n, E_NOTIFICATION_URGENCY_NORMAL);
      else if (!strcasecmp(optarg, "critical"))
        e_notification_hint_urgency_set(n, E_NOTIFICATION_URGENCY_CRITICAL);
      else
        printf("Urgency level must be: low, normal or critical\n");
      break;
    case 't':
      if (!read_int_arg(&value, "-t", INT_MIN, INT_MAX))
        return EXIT_FAILURE;
      else 
        e_notification_timeout_set(n, (int)value);
      break;
    case 'r':
      if (!read_int_arg(&value, "-r", 0, UINT_MAX))
        return EXIT_FAILURE;
      else
           e_notification_replaces_id_set(n, (unsigned int)value);
      break;
    case 'i':
      e_notification_app_icon_set(n, optarg);
      break;
    case 'c':
      e_notification_hint_category_set(n, optarg);
      break;
    case 'p':
      print_id = 1;
      break;
    default:
      usage();
      return EXIT_FAILURE;
    }
  argc -= optind;
  argv += optind;

  if (argc < 1)
    {
      usage();
      return EXIT_FAILURE;
    }

  e_notification_summary_set(n, argv[0]);
  if (argc > 1) e_notification_body_set(n, argv[1]);


  if (print_id)
    {
       e_notification_send(n, send_cb, NULL);
       ecore_main_loop_begin();
    }
  else
    e_notification_send(n, NULL, NULL);

  e_notification_unref(n);
  e_notification_shutdown();

  return EXIT_SUCCESS;
}
