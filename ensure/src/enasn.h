struct ensure;
struct enobj;

enum ensure_severity {
	ENSURE_CRITICAL,
	ENSURE_BUG,
	ENSURE_BADFORM,
	ENSURE_PEDANTIC,
	ENSURE_POLICY,
	ENSURE_N_SEVERITIES,
};


struct assurance {
	const char *summary;
	const char *description;
	enum ensure_severity severity;

	void *(*init)(struct ensure *);
	int (*object)(struct ensure *, struct enobj *, void *data);
	int (*fini)(struct ensure *, void *data);
};

struct bug {
	enum ensure_severity severity;
	char *desc;
};

int enasn_load(const char *);
int enasn_check(struct ensure *);
void enasn_display_bugs(void *data, Evas_Object *obj, void *event);
