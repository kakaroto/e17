#ifndef E_MOD_GADCON_H
#define E_MOD_GADCON_H

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);

EAPI void _gc_register(void);
EAPI void _gc_unregister(void);

#endif
