#include <E_Notification_Daemon.h>

typedef struct Daemon_Data Daemon_Data;
typedef struct Timer_Data Timer_Data;

struct Timer_Data
{
  Daemon_Data *d;
  E_Notification *n;
};

struct Daemon_Data
{
  E_Notification_Daemon *daemon;
  Eina_List *open_notes;

  Eina_List *history;
  int history_start;
  int max_history_length;
  int default_timeout;

  int next_id;
};

void
daemon_note_close(Daemon_Data *dd, E_Notification *n, int reason)
{
  printf("Close notification #%d\n", e_notification_id_get(n));

  if (eina_list_data_find(dd->open_notes, n))
  {
    dd->open_notes = eina_list_remove(dd->open_notes, n);
    e_notification_closed_set(n, 1);
    e_notification_daemon_signal_notification_closed(dd->daemon, e_notification_id_get(n), reason);
    e_notification_unref(n);
  }
}

int
cb_note_close_timer(void *data)
{
  Timer_Data *td = data;

  if (!e_notification_closed_get(td->n))
    daemon_note_close(td->d, td->n, E_NOTIFICATION_CLOSED_EXPIRED);

  e_notification_unref(td->n);
  free(td);

  return 0;
}

void
daemon_note_show(Daemon_Data *d, E_Notification *n)
{
  e_notification_ref(n);
  d->open_notes = eina_list_append(d->open_notes, n); 
  e_notification_ref(n);
  d->history = eina_list_append(d->history, n); 

  // adjust history
  if (eina_list_count(d->history) > d->max_history_length)
  {
    E_Notification *old;
    old = eina_list_data_get(d->history);
    d->history = eina_list_remove_list(d->history, d->history);
    d->history_start = e_notification_id_get(old) + 1;
    e_notification_unref(old);
  }

  {
    int timeout;

    timeout = e_notification_timeout_get(n);
    Timer_Data *td = calloc(1, sizeof(Timer_Data));
    td->d = d;
    e_notification_ref(n);
    td->n = n;
    ecore_timer_add(timeout == -1 ? d->default_timeout : (float)timeout / 1000, cb_note_close_timer, td);
  }

  printf("Received notification from %s:\n%s\n%s\n\n", 
    e_notification_app_name_get(n),
    e_notification_summary_get(n), e_notification_body_get(n)
  );
}

E_Notification *
daemon_note_open_find(Daemon_Data *d, int id)
{
  E_Notification *n;
  Eina_List *l;

  EINA_LIST_FOREACH(d->open_notes, l, n)
    if (e_notification_id_get(n) == id) return n;

  return NULL;
}

E_Notification *
daemon_note_history_find(Daemon_Data *d, int id)
{
  if (id < d->history_start) return NULL;

  // TODO

  return NULL;
}



int
cb_notify(E_Notification_Daemon *daemon, E_Notification *n)
{
  Daemon_Data *dd;
  unsigned int replaces_id;
  unsigned int new_id;

  dd = e_notification_daemon_data_get(daemon);
  replaces_id = e_notification_replaces_id_get(n);
  if (replaces_id) 
  {
    // close old one flagged as replaced
  }

  new_id = dd->next_id++;
  e_notification_id_set(n, new_id);

  daemon_note_show(dd, n);

  return new_id;
}

void
cb_close_notification(E_Notification_Daemon *daemon, unsigned int notification_id)
{
  Daemon_Data *dd;
  E_Notification *n;
  dd = e_notification_daemon_data_get(daemon);
  n = daemon_note_open_find(dd, notification_id);
  if (n)
    daemon_note_close(dd, n, E_NOTIFICATION_CLOSED_REQUESTED);
  // else send error?
}


int
main(int argc, char **argv)
{
  E_Notification_Daemon *d;
  E_Notification *n;
  Daemon_Data *dd;


  ecore_init();

  dd = calloc(1, sizeof(Daemon_Data));
  dd->open_notes = NULL;
  dd->history = NULL;
  dd->next_id = dd->history_start = 1;
  dd->max_history_length = 5;
  dd->default_timeout = 5;

  /* set up the daemon */
  d = e_notification_daemon_add("e_notification_module", "Enlightenment");
  e_notification_daemon_data_set(d, dd);
  dd->daemon = d;
  e_notification_daemon_callback_notify_set(d, cb_notify);
  e_notification_daemon_callback_close_notification_set(d, cb_close_notification);

  ecore_main_loop_begin();
  while (dd->open_notes)
    {
       n = eina_list_data_get(dd->open_notes);
       e_notification_unref(n);
       dd->open_notes = eina_list_remove_list(dd->open_notes, dd->open_notes);
    }
  while (dd->history)
    {
       n = eina_list_data_get(dd->history);
       e_notification_unref(n);
       dd->history = eina_list_remove_list(dd->history, dd->history);
    }
  free(dd);
  e_notification_daemon_free(d);
  ecore_shutdown();

  return 0;
}
