typedef struct _erss_config {
	char *header;
	char *hostname;
	char *url;

	char *item_root;
	char *item_start;
	char *item_title;
	char *item_url;
	char *item_description;

	int   update_rate;
	int   clock;
	int   num_stories;

	int   x;
	int   y;

	int   borderless;

	char *prefix;

	char *theme;
	char *config;
} Erss_Config;



typedef struct _erss_rc_config {
	char   *config;
	char   *theme;
	char   *browser;
	char   *proxy;
	int     proxy_port;
	int     clock;
	double  tooltip_delay;
} Erss_Rc_Config;



extern Erss_Config    *cfg;
extern Erss_Rc_Config *rc;

int  erss_parse_rc_file ();
void erss_parse_config_file (char *file);
