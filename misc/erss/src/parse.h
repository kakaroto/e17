typedef struct _article {
	Evas_Object *obj;

	char *url;
	char *description;
} Article;

typedef struct _config {
	char *header;
	char *hostname;
	char *url;

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

	char *proxy;
	int proxy_port;
	char *browser;
	char *prefix;
} Config;

void parse_data (char *buf);
void parse_config_file (char *file);

extern int objects_placed;
extern Config *cfg;
extern Article *item;
