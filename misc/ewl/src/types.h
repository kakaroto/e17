#ifndef _TYPES_H_
#define _TYPES_H_ 1

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */
#ifndef TRUE
#define TRUE  1
#endif /* TRUE */

typedef unsigned int    EwlType;
typedef EwlType         EwlWidgetType;
typedef void            EwlData;
typedef char            EwlBool;
typedef unsigned int    EwlFlag;
typedef unsigned int    EwlWidgetState;

/* delta for doing double comparisons */
#define _EWL_IMLIB_DELTA 0.01 

enum _EwlWidgetTypeEnum	{
	EWL_WIDGET,

	EWL_CONTAINER,
	EWL_VBOX,
	EWL_HBOX,
	EWL_LBOX,
	EWL_WINDOW,

	EWL_LABEL,
	EWL_BUTTON,

	EWL_TYPE_LAST
};

/* referenced in widget.c, ewl_widget_get_tyupe_string() */
static char *_EwlWidgetTypeStringEnum[]	= {
	"EWL_WIDGET",

	"EWL_CONTAINER",
	"EWL_VBOX",
	"EWL_HBOX",
	"EWL_LBOX",
	"EWL_WINDOW",

	"EWL_LABEL",
	"EWL_BUTTON",

	"EWL_TYPE_LAST"
};


enum _EwlWidgetStateEnum	{
	EWL_STATE_INACTIVE,
	EWL_STATE_NORMAL,
	EWL_STATE_HILITED,
	EWL_STATE_CLICKED,

	EWL_STATE_LAST
};

static char  *_EwlWidgetStateStringEnum[] = {
	"EWL_STATE_INACTIVE",
	"EWL_STATE_NORMAL",
	"EWL_STATE_HILITED",
	"EWL_STATE_CLICKED",

	"EWL_STATE_LAST"
};

static char  __depricated_types_shut_the_fuck_up_pedantic()	{
	if (!_EwlWidgetTypeStringEnum||!_EwlWidgetStateStringEnum)	{
		return __depricated_types_shut_the_fuck_up_pedantic();
	}
	return 0;
}

#endif /* _EWL_TYPES_H_ */

