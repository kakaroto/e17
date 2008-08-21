#include <ewl/Ewl.h>

#define BUTTONS 18

/*
typedef struct _test_set test_set;

struct _test_set
{
	char *name;
	Ewl_Callback_Function func;
};
*/

void            __close_main_widow(Ewl_Widget * w, void *ev_data,
				   void *user_data);
void            __create_box_test_window(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __create_button_test_window(Ewl_Widget * w, void *ev_data,
					    void *user_data);
void            __create_fileselector_test_window(Ewl_Widget * w, void *ev_data,
						  void *user_data);
void            __create_filedialog_test_window(Ewl_Widget * w, void *ev_data,
						  void *user_data);
void            __create_floater_test_window(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            __create_entry_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __create_image_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __create_imenu_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __create_menu_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __create_notebook_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_password_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_progressbar_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_scrollpane_test_window(Ewl_Widget * w, void *ev_data,
						void *user_data);
void            __create_seeker_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_spinner_test_window(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            __create_table_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __create_textarea_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_tooltip_test_window(Ewl_Widget * w, void *ev_data, 
		            void *user_data);
void            __create_tree_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_selectionbook_test_window(Ewl_Widget * w,
						   void *ev_data,
						   void *user_data);
void            __create_selectionbar_test_window(Ewl_Widget * w,
						   void *ev_data,
						   void *user_data);

