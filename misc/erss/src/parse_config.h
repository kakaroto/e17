typedef struct _config {
	char *header;
	char *hostname;
	char *url;

	char *item_root;
	char *item_start;
	char *item_title;
	char *item_url;
	char *item_description;

	int update_rate;
	int clock;
	int num_stories;

	int x;
	int y;

	int borderless;

	char *prefix;

	char *theme;
	char *config;
} Config;

typedef struct _rc_config {
	char *config;
	char *theme;
	char *browser;
	char *proxy;
	int proxy_port;
	char *enc_from;
	char *enc_to;
} Rc_Config;

extern Config *cfg;
extern Rc_Config *rc;

int erss_parse_rc_file ();
void erss_parse_config_file (char *file);
