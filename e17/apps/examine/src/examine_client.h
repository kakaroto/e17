#include "ecore_config_client.h"
#include "ipc.h"

typedef enum {
  P_NONE = 0,
  P_SERIAL = 1,
  P_DIR = 2,
  P_FILE = 4,
  P_KEY = 8,
  P_VALUE = 16,
  P_SILENT = 32,
  P_HELPONLY = 64,
  P_LAST = P_HELPONLY
} para;


typedef struct _call {
  ex_ipc_call     id;
  char           *name;
  para            signature;
} call;



static call     calls[] = {
  {IPC_NONE, "bundle", P_HELPONLY},
  {IPC_BUNDLE_LIST, "bundle-list", P_NONE},
  {IPC_BUNDLE_NEW, "bundle-new", P_DIR},
  {IPC_BUNDLE_LABEL_GET, "bundle-label-get", P_SILENT | P_SERIAL},
  {IPC_BUNDLE_LABEL_GET, "bundle-label", P_SERIAL},
  {IPC_BUNDLE_LABEL_SET, "bundle-label-set", P_SILENT | P_SERIAL | P_DIR},
  {IPC_BUNDLE_LABEL_SET, "bundle-label", P_SILENT | P_HELPONLY},
  {IPC_BUNDLE_LABEL_FIND, "bundle-label-find", P_SILENT | P_DIR},
  {IPC_BUNDLE_LABEL_FIND, "bundle-find", P_DIR},

  {IPC_NONE, "prop", P_HELPONLY},
  {IPC_PROP_LIST, "prop-list", P_SERIAL},
  {IPC_PROP_DESC, "prop-describe", P_SERIAL | P_KEY},
  {IPC_PROP_GET, "prop-get", P_SERIAL | P_KEY},
  {IPC_PROP_SET, "prop-set", P_SERIAL | P_KEY | P_VALUE},
  {IPC_PROP_LIST, "list", P_SILENT | P_SERIAL},
  {IPC_PROP_DESC, "describe", P_SILENT | P_SERIAL | P_KEY},
  {IPC_PROP_DESC, "explain", P_SILENT | P_SERIAL | P_KEY},
  {IPC_PROP_GET, "print", P_SILENT | P_SERIAL | P_KEY},
  {IPC_PROP_GET, "get", P_SILENT | P_SERIAL | P_KEY},
  {IPC_PROP_SET, "set", P_SILENT | P_SERIAL | P_KEY | P_VALUE},
  {IPC_PROP_SET, "let", P_SILENT | P_SERIAL | P_KEY | P_VALUE},

};



int             examine_client_send(call * c, char *key);
char           *examine_client_list_props(void);
char           *examine_client_get_val(char *key);
int             examine_client_init(char *pipe_name, connstate * cs);
int             examine_client_exit(void);
