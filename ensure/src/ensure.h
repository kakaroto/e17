
#include <Elementary.h>

#ifndef ensure_unused
#define ensure_unused   __attribute__((unused))
#define ensure_restrict __restrict__
#endif

enum ensure_severity;
struct enobj;
struct enasn;
struct assurance;
struct fileselector; /* in file.h */

enum
{ ENSURE_MAGIC = 0xE45073EE };

enum enview
{
   ENVIEW_ERROR,
   ENVIEW_CONFIG,
   ENVIEW_OBJECT_TREE,
   ENVIEW_HIDDEN,
};

enum ensure_severity
{
   ENSURE_CRITICAL,
   ENSURE_BUG,
   ENSURE_BADFORM,
   ENSURE_PEDANTIC,
   ENSURE_POLICY,
   ENSURE_N_SEVERITIES,
};

struct result
{
   struct ensure *ensure;

   const char    *title;
   time_t         tm;

   /* A list of enwins */
   Eina_List     *windows;
   Eina_Hash     *objdb;
};

struct ensure
{
   int                  magic;

   char               **args;

   enum enview          current_view;
   Evas_Object         *view;
   Evas_Object         *viewselect;
   Evas_Object         *reportselect;

   Evas_Object         *mainwindow;

   /* The list of hidden objects */
   Eina_List           *hidden;

   struct result       *cur;
   Eina_List           *results;

   int                  commandfd;

   struct fileselector *fileselector;
};

struct severityinfo
{
   const char       *name;
   const char       *icon;
   Elm_Genlist_Item *item;
   Eina_List        *asninfo;
};

extern struct severityinfo severity[ENSURE_N_SEVERITIES];
extern Evas_Object *savebutton;

int  ensure_assurance_add(struct assurance *);

/* report a bug */
int  ensure_bug(struct enobj *enobj, enum ensure_severity sev, const char *, const char *fmt, ...) __attribute__ ((format(printf, 4, 5)));

int  ensure_enobj_err_list_add(struct enobj *);
void view_dirty_set(struct ensure *);

#ifndef streq
#define streq(a, b) (strcmp((a), (b)) == 0)
#endif
