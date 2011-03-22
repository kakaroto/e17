
#include <Elementary.h>

#ifndef ensure_unused
#define ensure_unused	__attribute__((unused))
#define ensure_restrict	__restrict__
#endif

enum ensure_severity;
struct enobj;
struct enasn;
struct assurance;

enum { ENSURE_MAGIC = 0xE45073EE };

enum enview {
	ENVIEW_NONE,
	ENVIEW_ERROR,
	ENVIEW_CONFIG,
	ENVIEW_OBJECT_TREE,
	ENVIEW_HIDDEN,
};

enum ensure_severity {
	ENSURE_CRITICAL,
	ENSURE_BUG,
	ENSURE_BADFORM,
	ENSURE_PEDANTIC,
	ENSURE_POLICY,
	ENSURE_N_SEVERITIES,
};

struct ensure {
	int magic;

	enum enview current_view;
	Evas_Object *view;
	Evas_Object *viewselect;

	/* The list of hidden objects */
	Eina_List *hidden;
};



struct severityinfo {
	const char *name;
	const char *icon;
	Elm_Genlist_Item *item;
	Eina_List *asninfo;
};

extern struct severityinfo severity[ENSURE_N_SEVERITIES];

int ensure_assurance_add(struct assurance *);

/* report a bug */
int ensure_bug(struct enobj *enobj, enum ensure_severity sev,
		const char *fmt, ...);// __attribute__((printf(3,4)));


int ensure_enobj_err_list_add(struct enobj *);


#ifndef streq
#define streq(a, b) (strcmp((a), (b)) == 0)
#endif

