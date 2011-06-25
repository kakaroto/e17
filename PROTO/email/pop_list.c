#include "email_private.h"

Eina_Bool
email_stat(Email *e, Email_Stat_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->stat_cbs = eina_list_append(e->stat_cbs, cb);
   e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_STAT);
   email_write(e->svr, "STAT\r\n", 6);
   return EINA_TRUE;
}
