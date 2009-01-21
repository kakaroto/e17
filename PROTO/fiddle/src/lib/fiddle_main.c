#ifdef HAVE_CONFIG_H
# include "fiddle_config.h"
#endif

#include <Fiddle.h>
#include "fiddle_priv.h"

EAPI void
fiddle_init(void)
{
}

EAPI void
fiddle_shutdown(void)
{
}

EAPI Fiddle_Context *
fiddle_context_new(void)
{
}

EAPI void
fiddle_context_free(Fiddle_Context *c)
{
}

EAPI const Fiddle_Event *
fiddle_touch_feed(Fiddle_Context *c, const Fiddle_Touch *t)
{
}

EAPI const
Fiddle_Event *fiddle_event_get(Fiddle_Context *c)
{
}

EAPI void
fiddle_history_flush(Fiddle_Context *c)
{
}
