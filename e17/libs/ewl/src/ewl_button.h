
#ifndef __EWL_BUTTON_H__
#define __EWL_BUTTON_H__

enum _ewl_button_state {
	Ewl_Button_State_Normal,
	Ewl_Button_State_Hilited,
	Ewl_Button_State_Clicked
};

typedef enum _ewl_button_state Ewl_Button_State;

enum _ewl_button_type {
	Ewl_Button_Type_Normal,
	Ewl_Button_Type_Check,
	Ewl_Button_Type_Radio
};

typedef enum _ewl_button_type Ewl_Button_Type;

struct _ewl_button {
	Ewl_Widget widget;
	Ewl_Button_State state;
	Ewl_Button_Type type;
};

typedef struct _ewl_button Ewl_Button;

#define EWL_BUTTON(button) ((Ewl_Button *) button)

Ewl_Widget * ewl_button_new(Ewl_Button_Type type);

#endif
