#include <Etk.h>
#include <Entrance_Widgets.h>


Entrance_Widget
ew_label_new(const char *text)
{
	Entrance_Widget ew = ew_new();
	if(!ew) {
		return NULL;
	}

	if(text)
		ew->owner = etk_label_new(text);

	return ew;
}

void
ew_label_set(Entrance_Widget ew, const char *text)
{
	if(!ew || !text)
		return;

	etk_label_set(ETK_LABEL(ew->owner), text);
}

const char*
ew_label_get(Entrance_Widget ew)
{
	if(!ew)
		return;

	etk_label_get(ETK_LABEL(ew->owner));
}




