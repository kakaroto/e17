#include "ephoto.h"
#include <string.h>

char *
sgettext(const char *msgid)
{
	char *msgval;

	msgval = gettext(msgid);
	
	if (msgval == msgid)
	{
		msgval = strrchr(msgid, '|') + 1;
	}
	
	return msgval;
}
