#include "bits.h"
#include "Edb.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static Evas_List __bit_descriptions = NULL;

static Ebits_Object_Description _ebits_find_description(char *file);

static Ebits_Object_Description
_ebits_find_description(char *file)
{
   Ebits_Object_Description d = NULL;
   Evas_List l;
   E_DB_File *db;
   int version;
   
   /* find bit description in cache */
   for (l = __bit_descriptions; l; l = l->next)
     {
	d = l->data;
	if (!strcmp(d->file, file)) 
	  {
	     if (l != __bit_descriptions)
	       {
		  __bit_descriptions = evas_list_remove(__bit_descriptions, d);
		  __bit_descriptions = evas_list_prepend(__bit_descriptions, d);
	       }
	      return d;
	   }
     }
   /* open db */
   db = e_db_open_read(file);
   /* no db - return NULL */
   if (!db) return NULL;
   
   /* check version of file (and magic key/number) */
   version = 0;
   e_db_int_get(db, "/type/bits", &(version));
   /* got either no key or version != 1 */
   if (version != 1)
     {
	e_db_close(db);
	return NULL;
     }
   /* new description */
   d = malloc(sizeof(struct _Ebits_Object_Description));
   memset(d, 0, sizeof(struct _Ebits_Object_Description));
   d->file = strdup(file);
   d->references = 1;
   
   e_db_int_get(db, "/base/min/w", &(d->min.w));
   e_db_int_get(db, "/base/min/h", &(d->min.h));
   
   e_db_int_get(db, "/base/max/w", &(d->max.w));
   e_db_int_get(db, "/base/max/h", &(d->max.h));

   e_db_int_get(db, "/base/padding/l", &(d->padding.l));
   e_db_int_get(db, "/base/padding/r", &(d->padding.r));
   e_db_int_get(db, "/base/padding/t", &(d->padding.t));
   e_db_int_get(db, "/base/padding/b", &(d->padding.b));

   e_db_int_get(db, "/base/inset/l", &(d->padding.l));
   e_db_int_get(db, "/base/inset/r", &(d->padding.r));
   e_db_int_get(db, "/base/inset/t", &(d->padding.t));
   e_db_int_get(db, "/base/inset/b", &(d->padding.b));
   
   e_db_close(db);
   __bit_descriptions = evas_list_prepend(__bit_descriptions, d);
   return d;
}

Ebits_Object ebits_load(char *file)
{
   Ebits_Object o;
   Ebits_Object_Description d;   
   
   d = _ebits_find_description(file);
   if (!d) return NULL;
   
   o = ebits_new();
   o->description = d;
   return o;
}

void ebits_free(Ebits_Object o){}
void ebits_add_to_evas(Ebits_Object o, Evas e){}
void ebits_show(Ebits_Object o){}
void ebits_hide(Ebits_Object o){}
void ebits_set_layer(Ebits_Object o, int l){}
void ebits_raise(Ebits_Object o){}
void ebits_lower(Ebits_Object o){}
void ebits_move(Ebits_Object o, double x, double y){}
void ebits_resize(Ebits_Object o, double w, double h){}
void ebits_get_padding(Ebits_Object o, int *l, int *r, int *t, int *b){}
void ebits_get_insets(Ebits_Object o, int *l, int *r, int *t, int *b){}
void ebits_get_min_size(Ebits_Object o, int *w, int *h){}
void ebits_get_max_size(Ebits_Object o, int *w, int *h){}
void ebits_get_size_step(Ebits_Object o, int *x, int *y){}
void ebits_get_bit_geometry(Ebits_Object o, char *c, double *x, double *y, double *w, double *h){}

Ebits_Object ebits_new(void)
{
   Ebits_Object o;
   
   o = malloc(sizeof(struct _Ebits_Object));
   memset(o, 0, sizeof(struct _Ebits_Object));
   o->description = NULL;
   o->state.x = 0;
   o->state.y = 0;
   o->state.w = 1;
   o->state.h = 1;
   return o;
}

void ebits_save(Ebits_Object o, char *file) {}

