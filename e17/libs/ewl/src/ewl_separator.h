
#ifndef __EWL_SEPARATOR_H__
#define __EWL_SEPARATOR_H__

typedef struct _ewl_separator Ewl_Separator;

#define EWL_SEPARATOR(separator) ((Ewl_Separator *) separator)

struct _ewl_separator
{
	Ewl_Widget widget;
	Ewl_Orientation orientation;

	struct
	{
		int l, r, t, b;
	}
	padd;
};

#define ewl_hseparator_new() ewl_separator_new(EWL_ORIENTATION_HORIZONTAL)
#define ewl_vseparator_new() ewl_separator_new(EWL_ORIENTATION_VERTICAL)

Ewl_Widget *ewl_separator_new(Ewl_Orientation o);
void ewl_separator_set_padding(Ewl_Widget * w, int l, int r, int t, int b);


#endif /* __EWL_SEPARATOR_H__ */
