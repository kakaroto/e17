#include <Etk.h>
#include <Entrance_Widgets.h>


Entrance_Widget
ew_entry_new(const char *text, int ispassword)
{
	Entrance_Widget ew = ew_new();
	if(!ew)
	{
		return NULL;
	}

	ew->owner = etk_entry_new();

	if(text)
		ew_entry_set(ew, text);

	if(ispassword)
		ew_entry_password_set(ew);

	return ew;
}

const char*
ew_entry_get(Entrance_Widget ew)
{
	return etk_entry_text_get(ETK_ENTRY(ew->owner));
}

void
ew_entry_set(Entrance_Widget ew, const char *text)
{
	if(ew)
		etk_entry_text_set(ETK_ENTRY(ew->owner), text);
}

void
ew_entry_password_set(Entrance_Widget ew)
{
	if(ew)
		etk_entry_password_set(ETK_ENTRY(ew->owner), ETK_TRUE);
}

void 
ew_entry_password_clear(Entrance_Widget ew)
{
	if(ew)
		etk_entry_password_set(ETK_ENTRY(ew->owner),  ETK_FALSE);
}
