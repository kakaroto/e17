
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

typedef enum Examine_Callback_Type {
  EX_DATA_SET = 0,
  EX_DATA_GET = 1,
  EX_DATA_LIST = 2
} Examine_Callback_Type;

Examine_Callback_Type expected_type;

char           *examine_client_buf;
Ecore_Config_Ipc_Server_List *examine_client_server;
examine_prop   *prop_list;

/*****************************************************************************/


int
ecore_config_ipc_server_con(void *data, int type, void *event)
{
  connstate      *cs = (connstate *) data;

  *cs = ONLINE;
  E(1, "examine: Connected .\n");       /*to %s.\n", pipe_name)); */
  return 1;
}



int
ecore_config_ipc_server_dis(void *data, int type, void *event)
{
  connstate      *cs = (connstate *) data;

  *cs = OFFLINE;
  E(1, "examine: Disconnected.\n");
  return 1;
}



int
ecore_config_ipc_server_sent(void *data, int type, void *event)
{
  Ecore_Ipc_Event_Server_Data *e;

  e = (Ecore_Ipc_Event_Server_Data *) event;
  if (e->data && *((char *) e->data))
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

  switch (expected_type) {
  case EX_DATA_GET:
    examine_client_get_val_cb();
    break;
  case EX_DATA_LIST:
    examine_client_list_props_cb();
    break;
  default:
    break;
  }
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
examine_client_send(call * c, char *key, char *val)
{
  char           *m;
  int             l, ret;
  long            serial;

  m = NULL;
  l = 0;
  serial = 0;
  if (key)
    send_append(&m, &l, key);
  if (val)
    send_append(&m, &l, val);
  ret = ecore_config_ipc_send(&examine_client_server, c->id, serial, m, l);
  if (m)
    free(m);
}

void
examine_client_list_props(void)
{
  call           *c;

  if (prop_list) {
    draw_tree(prop_list);
    return;
  }

  c = find_call("prop-list");
  examine_client_send(c, NULL, NULL);

  expected_type = EX_DATA_LIST;
}

void
examine_client_list_props_cb(void)
{
  examine_prop   *prop_tmp;
  char           *label, *typename, *start, *end;
  int             tmpi;
  float           tmpd;
  char            type[20], range[10], step[5];
  int             mini, maxi;
  float           mind, maxd;

  if (examine_client_buf && (strlen(examine_client_buf) > 0)) {

    start = examine_client_buf;
    end = examine_client_buf + strlen(examine_client_buf);

    while (start < end) {
      label = start;
      while (*start) {
        if (*start == ':') {
          *start = '\0';
          break;
        }
        start++;
      }

      start++;
      typename = ++start;
      while (*start) {
        if (*start == '\n') {
          *start = '\0';
          break;
        }
        start++;
      }

      if (*label && *typename) {
        prop_tmp = malloc(sizeof(examine_prop));
        prop_tmp->key = strdup(label);
        prop_tmp->bound = BOUND_NONE;

        type[0] = '\0';
        range[0] = '\0';
        step[0] = '\0';
        sscanf(typename, "%s%*s%s%*s%s", &type, &range, &step);

        if (type[strlen(type) - 1] == ',')
          type[strlen(type) - 1] = '\0';
        if (*range)
          if (range[strlen(range) - 1] == ',')
            range[strlen(range) - 1] = '\0';

        if (!strcmp(type, "string")) {
          prop_tmp->type = PT_STR;
        } else if (!strcmp(type, "integer")) {
          prop_tmp->type = PT_INT;
          if (*range) {
            prop_tmp->bound |= BOUND_BOUND;
            sscanf(range, "%d..%d", &mini, &maxi);
            prop_tmp->min = mini;
            prop_tmp->max = maxi;
          }
          if (*step) {
            prop_tmp->bound |= BOUND_STEPPED;
            sscanf(step, "%d", &tmpi);
            prop_tmp->step = tmpi;
          }
        } else if (!strcmp(type, "float")) {
          prop_tmp->type = PT_FLT;
          if (*range) {
            prop_tmp->bound |= BOUND_BOUND;
            sscanf(range, "%lf..%lf", &mind, &maxd);
            prop_tmp->fmin = mind;
            prop_tmp->fmax = maxd;
          }
          if (*step) {
            prop_tmp->bound |= BOUND_STEPPED;
            sscanf(step, "%lf", &tmpd);
            prop_tmp->fstep = tmpd;
          }
        } else if (!strcmp(type, "colour")) {
          prop_tmp->type = PT_RGB;
        } else
          prop_tmp->value.ptr = NULL;

        prop_tmp->next = prop_list;
        prop_list = prop_tmp;
      }

      start++;
    }
  }
  free(examine_client_buf);

  draw_tree(prop_list);
}

void
examine_client_revert_list(void)
{
  examine_prop   *prop_item;

  prop_item = prop_list;
  while (prop_item) {
    examine_client_revert(prop_item);
    prop_item = prop_item->next;
  }
  draw_tree(prop_list);
}

void
examine_client_revert(examine_prop * target)
{
  switch (target->type) {
  case PT_INT:
    target->value.val = target->oldvalue.val;
    break;
  case PT_FLT:
    target->value.fval = target->oldvalue.fval;
    break;
  default:                     /* PT_STR, PT_RGB */
    free(target->value.ptr);
    target->value.ptr = strdup(target->oldvalue.ptr);
  }
}

void
examine_client_save_list(void)
{
  examine_prop   *prop_item;

  prop_item = prop_list;
  while (prop_item) {
    examine_client_save(prop_item);
    prop_item = prop_item->next;
  }
}

void
examine_client_save(examine_prop * target)
{
  switch (target->type) {
  case PT_INT:
    if (target->value.val != target->oldvalue.val) {
      target->oldvalue.val = target->value.val;
      examine_client_set_val(target);
    }
    break;
  case PT_FLT:
    if (target->value.fval != target->oldvalue.fval) {
      target->oldvalue.fval = target->value.fval;
      examine_client_set_val(target);
    }
    break;
  default:                     /* PT_STR, PT_RGB */
    if (strcmp(target->value.ptr, target->oldvalue.ptr) != 0) {
      free(target->oldvalue.ptr);
      target->oldvalue.ptr = strdup(target->value.ptr);
      examine_client_set_val(target);
    }
  }
}

void
examine_client_get_val(char *key)
{
  call           *c;

  c = find_call("prop-get");
  expected_type = EX_DATA_GET;
  examine_client_send(c, key, NULL);
}

void
examine_client_get_val_cb(void)
{
  char           *ret, *key, *end, *tmp;
  int             tmpi;
  float           tmpd;
  examine_prop   *prop;

  ret = strstr(examine_client_buf, "=") + 1;
  if (*ret == '"') {
    ret++;
    if (end = strstr(ret, "\""))
      *end = '\0';
  }

  if (*(ret + strlen(ret) - 1) == '\n')
    *(ret + strlen(ret) - 1) = '\0';
  key = examine_client_buf;
  tmp = strstr(examine_client_buf, ":");
  *tmp = '\0';

  prop = prop_list;
  while (prop) {
    if (!strcmp(key, prop->key))
      break;
    prop = prop->next;
  }
  if (!prop)
    return;

  switch (prop->type) {
  case PT_INT:
    sscanf(ret, "%d", &tmpi);
    prop->value.val = tmpi;
    prop->oldvalue.val = tmpi;
    ewl_spinner_set_value(EWL_SPINNER(prop->w), (double) tmpi);
    break;
  case PT_FLT:
    sscanf(ret, "%lf", &tmpd);
    prop->value.fval = tmpd;
    prop->oldvalue.fval = tmpd;
    ewl_spinner_set_value(EWL_SPINNER(prop->w), tmpd);
    break;
  default:                     /* PT_STR, PT_RGB */
    prop->value.ptr = strdup(ret);
    prop->oldvalue.ptr = strdup(ret);
    ewl_entry_set_text(EWL_ENTRY(prop->w), ret);
  }
}

void
examine_client_set_val(examine_prop * target)
{
  char           *valstr;
  call           *c;

  c = find_call("prop-set");

  switch (target->type) {
  case PT_INT:
    valstr = malloc(1000);      /* ### FIXME */
    sprintf(valstr, "%d", target->value.val);
    break;
  case PT_FLT:
    valstr = malloc(1000);      /* ### FIXME */
    sprintf(valstr, "%f", target->value.fval);
    break;
  default:                     /* PT_STR, PT_RGB */
    valstr = target->value.ptr;
  }

  examine_client_send(c, target->key, valstr);

  expected_type = EX_DATA_SET;
}

int
examine_client_init(char *pipe_name, connstate * cs)
{
  return ecore_config_ipc_init(&examine_client_server, pipe_name, cs);
}

int
examine_client_exit(void)
{
  return ecore_config_ipc_exit(&examine_client_server);
}
