#include "ipc.h"

typedef enum { OFFLINE, ONLINE } connstate;

typedef struct Ecore_Config_Ipc_Server_List {
  Ecore_Ipc_Server *srv;
  connstate       state;
  struct Ecore_Config_Ipc_Server_List *next;
} Ecore_Config_Ipc_Server_List;

/* in client */
int             ecore_config_ipc_server_con(void *data, int type, void *event);
int             ecore_config_ipc_server_dis(void *data, int type, void *event);
int             ecore_config_ipc_server_sent(void *data, int type, void *event);

/* in client lib */
const char     *ecore_config_error(int no);
int             ecore_config_ipc_init(Ecore_Config_Ipc_Server_List ** srv_list,
                                      char *pipe_name, connstate * cs);
int             ecore_config_ipc_exit(Ecore_Config_Ipc_Server_List ** srv_list);
int             ecore_config_ipc_sigexit(void *data, int type, void *event);
int             ecore_config_ipc_send(Ecore_Config_Ipc_Server_List ** srv_list,
                                      int major, int minor, void *data,
                                      int size);
int             send_append(char **mp, int *lp, char *dp);

extern int      debug;

#ifndef E
#  define E(lvl,fmt,args...) do { if(debug>=(lvl)) fprintf(stderr,fmt,## args); } while(0)
#endif
