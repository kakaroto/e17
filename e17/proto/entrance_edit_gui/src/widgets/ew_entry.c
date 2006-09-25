#include <Etk.h>
#include <Entrance_Widgets.h>


Entrance_Entry
_ew_entry_new(void)
{
	Entrance_Entry ee = calloc(1, sizeof(*ee));
	if(ee)
	{
		ee->owner = NULL;
		ee->box = NULL;
		ee->label = NULL;
		ee->control = NULL;
	}

	return ee;
}

Entrance_Entry
ew_entry_new(const char *label, const char *text, int ispassword)
{
	Entrance_Entry ew = _ew_entry_new();
	if(!ew)
	{
		return NULL;
	}

	ew->owner = etk_hbox_new(0, 10);
	ew->control = etk_entry_new();


	if(ispassword)
		ew_entry_password_set(ew);

	if(label)
	{
		ew->label = etk_label_new(label);
		etk_box_append(ETK_BOX(ew->owner), ew->label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	}

	etk_box_append(ETK_BOX(ew->owner), ew->control, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	if(text)
	{
		ew_entry_set(ew, text);
	} 
	return ew;
}

const char*
ew_entry_get(Entrance_Entry ew)
{
	return etk_entry_text_get(ETK_ENTRY(ew->control));
}

void
ew_entry_set(Entrance_Entry ew, const char *text)
{
	if(ew)
		etk_entry_text_set(ETK_ENTRY(ew->control), text);
}

void
ew_entry_password_set(Entrance_Entry ew)
{
	if(ew)
		etk_entry_password_mode_set(ETK_ENTRY(ew->control), ETK_TRUE);
}

void 
ew_entry_password_clear(Entrance_Entry ew)
{
	if(ew)
		etk_entry_password_mode_set(ETK_ENTRY(ew->control),  ETK_FALSE);
}
