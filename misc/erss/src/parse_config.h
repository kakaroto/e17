typedef struct _erss_rc_config {
	char   *config;
	char   *theme;
	char   *browser;
	char   *proxy;
	int     proxy_port;
	int     clock;
	double  tooltip_delay;
} Erss_Rc_Config;



extern Erss_Rc_Config *rc;

int  erss_parse_rc_file ();
Erss_Config *erss_parse_config_file (char *file);
Erss_Config *erss_cfg_dst (Erss_Config **);
