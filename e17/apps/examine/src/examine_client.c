
/* Modified from Azundreis' evidence */

#include "Ecore_Config.h"

#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "examine_client.h"


char           *examine_client_buf;
ex_ipc_server_list *examine_client_server;

/*****************************************************************************/


int
ex_ipc_server_con(void *data, int type, void *event)
{
  connstate      *cs = (connstate *) data;

  *cs = ONLINE;
  E(1, "examine: Connected .\n");       /*to %s.\n", pipe_name)); */
  return 1;
}



int
ex_ipc_server_dis(void *data, int type, void *event)
{
  connstate      *cs = (connstate *) data;

  *cs = OFFLINE;
  ecore_main_loop_quit();
  E(1, "examine: Disconnected.\n");
  return 1;
}



int
ex_ipc_server_sent(void *data, int type, void *event)
{
  Ecore_Ipc_Event_Server_Data *e;

  e = (Ecore_Ipc_Event_Server_Data *) event;
  examine_client_buf = strdup(e->data);

  E(2, "application sent: %2d.%02d  #%03d=>#%03d  \"%s\".%d\n",
    e->major, e->minor, 0, 0, (char *) e->data, e->size);
  if (e->data && (e->size > 0))
    E(3, "%s\n", (char *) e->data);
  else if (e->response < 0)
    fprintf(stderr, "error #%d\n", e->response);
  else if (!e->response)
    E(3, "OK\n");
  else
    E(2, "result: %d\n", e->response);
  ecore_main_loop_quit();
  return 1;
}

static int
abbrevcmp(char *a, char *t)
{
  size_t          l1;
  char           *p, *q;
  int             ret;

  while (1) {
    if (!(p = strchr(a, '-')))
      p = a + strlen(a);
    l1 = p - a;
    if ((ret = strncasecmp(a, t, l1)))
      return ret;

    a = p;
    if (*a)
      a++;

    if (!(q = strchr(t, '-')))
      t = t + strlen(t);
    else
      t = q + 1;

    if (!*a)
      return *t ? -1 : 0;
    else if (!*t)
      return 1;
  }
}



static call    *
find_call(char *b)
{
  int             nc = sizeof(calls) / sizeof(call);
  call           *cp = calls, *r = NULL;

  while (nc-- > 0) {
    if (!abbrevcmp(b, cp->name)) {
      if (r) {
        printf("\"%s\" is not unique.\n", b);
        return NULL;
      }
      r = cp;
    }
    cp++;
  }
  return r;
}


/*****************************************************************************/

int
examine_client_send(call * c, char *key)
{
  char           *m;
  int             l, ret;
  long            serial;

  m = NULL;
  l = 0;
  serial = 0;
  if (key)
    send_append(&m, &l, key);
  ret = ex_ipc_send(&examine_client_server, c->id, serial, m, l);
  if (m)
    free(m);
}

char           *
examine_client_list_props(void)
{
  call           *c;

  c = find_call("prop-list");
  examine_client_send(c, NULL);
  return examine_client_buf;
}

char           *
examine_client_get_val(char *key)
{
  char           *ret, *end;
  call           *c;

  c = find_call("prop-get");
  examine_client_send(c, key);
  ret = strstr(examine_client_buf, "=") + 1;
  if (*ret == '"') {
    ret++;
    if (end = strstr(ret, "\""))
      *end = '\0';
  }
  if (*(ret + strlen(ret) - 1) == '\n')
    *(ret + strlen(ret) - 1) = '\0';
  return ret;
}

int
examine_client_init(char *pipe_name, connstate * cs)
{
  return ex_ipc_init(&examine_client_server, pipe_name, cs);
}

int
examine_client_exit(void)
{
  return ex_ipc_exit(&examine_client_server);
}
