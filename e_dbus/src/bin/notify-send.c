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
         "  -i, --icon=ICON                   Specifies an icon filename or stock icon to display.\n"
         "  -c, --category=TYPE               Specifies the notification category.\n"
         "  -v, --version                     Version of the package.\n"
         "\n");
}

int
main(int argc, char **argv)
{
  int ch;
  char *endptr;
  int timeout;
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
      { "icon",        required_argument,      NULL,           'i' },
      { "category",    required_argument,      NULL,           'c' },
      { "version",     no_argument,            NULL,           'v' },
      { NULL,          0,                      NULL,             0 }
  };

  while ((ch = getopt_long(argc, argv, "?vn:u:t:i:c:", longopts, NULL)) != -1)
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
      errno = 0;
      timeout = strtol(optarg, &endptr, 10);
      if ((errno != 0 && timeout == 0) || endptr == optarg) 
        {
          fprintf(stderr, "Cannot parse integer value '%s' for -t\n", optarg);
          return EXIT_FAILURE;
        }
      else if (timeout > INT_MAX || timeout < INT_MIN)
        {
          fprintf(stderr, "Integer value '%s' for -t out of range\n", optarg);
          return EXIT_FAILURE;
        }
      else 
        e_notification_timeout_set(n, timeout);
      break;
    case 'i':
      e_notification_app_icon_set(n, optarg);
      break;
    case 'c':
      e_notification_hint_category_set(n, optarg);
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

  e_notification_send(n, NULL, NULL);
  e_notification_unref(n);
  e_notification_shutdown();

  return EXIT_SUCCESS;
}
