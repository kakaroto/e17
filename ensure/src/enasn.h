struct ensure;
struct enobj;

struct assurance
{
   const char          *summary;
   const char          *description;
   enum ensure_severity severity;

   void                *(*init)(struct ensure *);
   int                  (*object)(struct ensure *, struct enobj *, void *data);
   int                  (*fini)(struct ensure *, void *data);
};

struct bug
{
   const char          *type;
   enum ensure_severity severity;
   char                *desc;
};

int  enasn_load(const char *);
int  enasn_check(struct ensure *);
void enasn_display_bugs(void *data, Evas_Object *obj, void *event);
