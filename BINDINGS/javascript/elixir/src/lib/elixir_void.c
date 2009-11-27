#include <stdlib.h>

#include "Elixir.h"

#define ELIXIR_VOID_MAGIC       0xDEAD007
#define ELIXIR_VOID_CHECK(Void) (!Void || Void->magic != ELIXIR_VOID_MAGIC)

struct elixir_void_data_s
{
   int		magic;
   jsval	val;
   void*	private;
   JSContext*	cx;
   JSObject*    parent;
};

void*
elixir_void_new(JSContext* cx, JSObject *parent, jsval any, void* private)
{
   struct elixir_void_data_s*	dt;

   dt = malloc(sizeof (struct elixir_void_data_s));
   if (!dt)
     return NULL;

   dt->magic = ELIXIR_VOID_MAGIC;
   dt->val = any;
   dt->private = private;
   dt->cx = cx;
   dt->parent = parent;

   /* Prevent GC */
   elixir_rval_register(cx, &dt->val);
   elixir_object_register(cx, &dt->parent, NULL);
   elixir_void_register(cx, dt);

   return dt;
}

jsval
elixir_void_free(void* data)
{
   struct elixir_void_data_s *dt = data;
   JSContext *cx;
   Eina_Bool suspended;
   jsval val;

   if (ELIXIR_VOID_CHECK(dt))
     return JSVAL_NULL;

   val = dt->val;
   cx = dt->cx;

   suspended = elixir_function_suspended(cx);

   if (suspended) elixir_function_start(cx);

   elixir_rval_delete(cx, &dt->val);
   elixir_object_unregister(cx, &dt->parent);

   if (suspended) elixir_function_stop(cx);

   elixir_void_unregister(cx, dt);

   dt->magic = 0;
   free(dt);

   return val;
}

void
elixir_void_move(JSContext *replace, void *data)
{
   struct elixir_void_data_s *dt = data;
   JSContext *cx;

   if (ELIXIR_VOID_CHECK(dt))
     return ;

   cx = dt->cx;

   elixir_rval_register(replace, &dt->val);
   elixir_rval_delete(cx, &dt->val);

   elixir_object_register(replace, &dt->parent, NULL);
   elixir_object_unregister(cx, &dt->parent);

   elixir_void_register(replace, data);
   elixir_void_unregister(cx, data);

   dt->cx = replace;
}

JSObject*
elixir_void_get_parent(void* data)
{
   struct elixir_void_data_s*	dt = data;

   if (ELIXIR_VOID_CHECK(dt))
     return NULL;

   if (dt->parent)
     return dt->parent;
   return NULL;
}

JSObject *
elixir_void_set_parent(const void *data, JSObject *parent)
{
   struct elixir_void_data_s *dt = (struct elixir_void_data_s *) data;
   JSObject *old;

   if (ELIXIR_VOID_CHECK(dt))
     return NULL;

   old = dt->parent;
   elixir_object_register(dt->cx, &old, NULL);

   elixir_object_unregister(dt->cx, &dt->parent);
   dt->parent = parent;
   elixir_object_register(dt->cx, &dt->parent, NULL);

   elixir_object_unregister(dt->cx, &old);

   return old;
}

jsval
elixir_void_get_jsval(void* data)
{
   struct elixir_void_data_s* dt = data;

   if (ELIXIR_VOID_CHECK(dt))
     return JSVAL_NULL;

   return dt->val;
}

void*
elixir_void_get_private(void* data)
{
   struct elixir_void_data_s* dt = data;

   if (ELIXIR_VOID_CHECK(dt))
     return NULL;

   return dt->private;
}

void
elixir_void_set_private(void *data, void *private)
{
   struct elixir_void_data_s* dt = data;

   if (ELIXIR_VOID_CHECK(dt))
     return ;

   dt->private = private;
}

JSContext*
elixir_void_get_cx(void* data)
{
   struct elixir_void_data_s* dt = data;

   if (ELIXIR_VOID_CHECK(dt))
     return NULL;

   return dt->cx;
}
