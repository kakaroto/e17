
#ifndef __EWL_BUTTON_H__
#define __EWL_BUTTON_H__

typedef enum _ewl_button_state Ewl_Button_State;

enum _ewl_button_type {
	EWL_BUTTON_TYPE_NORMAL,
	EWL_BUTTON_TYPE_CHECK,
	EWL_BUTTON_TYPE_RADIO
};

typedef enum _ewl_button_type Ewl_Button_Type;

struct _ewl_button {
	Ewl_Widget widget;
	Ebits_Object ebits_object;
	Ewl_State state;
	Ewl_Button_Type type;
};

typedef struct _ewl_button Ewl_Button;

#define EWL_BUTTON(button) ((Ewl_Button *) button)

Ewl_Widget * ewl_button_new();
Ewl_Widget * ewl_button_new_with_label(const char * label);

#endif
