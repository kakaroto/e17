#include <Ecore.h>
#include "Pulse.h"

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

static void
result_cb(Pulse *conn, Pulse_Tag_Id id, void *ev)
{
   if (!ev) fprintf(stderr, "Command %u failed!\n", id);
   else fprintf(stderr, "Command %u succeeded!\n", id);
   pulse_free(conn);
   ecore_main_loop_quit();
}

void
sink_cb(Pulse *conn, Pulse_Tag_Id id __UNUSED__, Eina_List *sinks)
{
   Pulse_Sink *sink;
   uint32_t tid;

   pulse_sinks_watch(conn);
   EINA_LIST_FREE(sinks, sink)
     {
        printf("Sink:\n");
        printf("\tname: %s\n", pulse_sink_name_get(sink));
        printf("\tidx: %"PRIu32"\n", pulse_sink_idx_get(sink));
        printf("\tdesc: %s\n", pulse_sink_desc_get(sink));
        printf("\tchannels: %u\n", pulse_sink_channels_count(sink));
        printf("\tmuted: %s\n", pulse_sink_muted_get(sink) ? "yes" : "no");
        printf("\tavg: %g\n", pulse_sink_avg_get_pct(sink));
        printf("\tbalance: %f\n", pulse_sink_balance_get(sink));
/*
        tid = pulse_sink_channel_volume_set(conn, sink, 0, 25);
        tid = pulse_sink_mute_set(conn, pulse_sink_idx_get(sink), EINA_FALSE);
        if (!tid)
          {
             pulse_free(conn);
             ecore_main_loop_quit();
          }
        pulse_cb_set(conn, tid, (Pulse_Cb)result_cb);
        pulse_sink_free(sink);
*/
     }
}

static Eina_Bool
con(void *d __UNUSED__, int type __UNUSED__, Pulse *conn)
{
   uint32_t id;
   printf("connect!\n");
   id = pulse_sinks_get(conn);
   pulse_cb_set(conn, id, (Pulse_Cb)sink_cb);
   return ECORE_CALLBACK_RENEW;
}


int
main(int argc, char *argv[])
{
   Pulse *conn;
   pulse_init();
   eina_log_domain_level_set("pulse", EINA_LOG_LEVEL_DBG);
   ecore_app_args_set(argc, (const char **)argv);
   ecore_event_handler_add(PULSE_EVENT_CONNECTED, (Ecore_Event_Handler_Cb)con, NULL);

   conn = pulse_new();
   if (!conn) goto error;
   if (!pulse_connect(conn)) goto error;
        
   ecore_main_loop_begin();
   return 0;
error:
   fprintf(stderr, "Could not connect!\n");
   return 1;
}

