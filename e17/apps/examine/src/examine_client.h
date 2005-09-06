#include "ecore_config_client.h"
#include <Ewl.h>

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
  Ecore_Config_Ipc_Call id;
  char           *name;
  para            signature;
} call;

typedef enum bound_flags {
  BOUND_NONE = 0,
  BOUND_BOUND = 1,
  BOUND_STEPPED = 2
} bound_flags;


typedef struct examine_prop {
  char           *key;
  int             type;
  bound_flags     bound;
  int             min, max, step;
  float           fmin, fmax, fstep;
  union {
    char           *ptr;
    long            val;
    float           fval;
  } value;
  union {
    char           *ptr;
    long            val;
    float           fval;
  } oldvalue;
  Ewl_Widget     *w;
  void           *data;
  struct examine_prop *next;
} examine_prop;

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

char           *__examine_client_theme_search_path;

int             examine_client_send(call * c, char *key, char *val);
void            examine_client_list_props(void);
void            examine_client_theme_search_path_get(void);
void            examine_client_revert_list(void);
void            examine_client_revert(examine_prop * target);
void            examine_client_save_list(void);
void            examine_client_save(examine_prop * target);
void            examine_client_get_val(char *key);
void            examine_client_set_val(examine_prop * target);
int             examine_client_init(char *pipe_name, connstate * cs);
int             examine_client_exit(void);

/* from examine.c */
void draw_tree(examine_prop * prop_item);
