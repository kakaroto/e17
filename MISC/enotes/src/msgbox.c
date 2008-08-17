
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#include "msgbox.h"

/* Making a Message Box */

/**
 * @param title: The titlebar content of the window.
 * @param content: The content string to display in the window.
 * @param x: X co-ordinate to place the window at.
 * @param y: Y co-ordinate to place the window at.
 * @param width: Width of window.
 * @param height: Height of the window.
 * @brief: Opens up a message box.
 */
void
msgbox(char *title, char *content)
{
	MsgBox         *mb = malloc(sizeof(MsgBox));

	/* Setup the Window */
	mb->win = ewl_window_new();
	ewl_window_title_set((Ewl_Window *) mb->win, title);
	ewl_widget_show(mb->win);

	mb->vbox = ewl_vbox_new();
	ewl_container_child_append((Ewl_Container *) mb->win, mb->vbox);
	ewl_object_padding_set((Ewl_Object *) mb->vbox, 5, 10, 5, 10);
	ewl_widget_show(mb->vbox);

	mb->msg = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(mb->msg), content);
	ewl_container_child_append((Ewl_Container *) mb->vbox, mb->msg);
	ewl_widget_show(mb->msg);

	mb->hbox = ewl_hbox_new();
	ewl_container_child_append((Ewl_Container *) mb->vbox, mb->hbox);
	ewl_widget_show(mb->hbox);

	mb->okbtn = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(mb->okbtn), "Ok.");
	ewl_container_child_append((Ewl_Container *) mb->hbox, mb->okbtn);
	ewl_widget_show(mb->okbtn);

	/* EWL Callbacks */
	ewl_callback_append(mb->okbtn, EWL_CALLBACK_CLICKED,
			    (void *) msgbox_okbtn_clicked, mb);
	ewl_callback_append(mb->win, EWL_CALLBACK_DELETE_WINDOW,
			    (void *) msgbox_okbtn_clicked, mb);

	return;
}


/* Callbacks */

/**
 * @param p: The Msgbox to be destroyed.
 * @brief: Closes the msgbox window ee.
 */
void
msgbox_close(MsgBox * p)
{
	ewl_widget_destroy(p->win);
	free(p);
	return;
}

/**
 * @param widget: The Ewl_Widget of the ok button which was clicked.
 * @param ev_data: Event data, not used.
 * @param data: The msgbox supplied when the callback was set.
 * @brief: Ewl callback on the ok button being clicked.
 *         Closes the Ecore_Evas window (data) via msgbox_close.
 */
void
msgbox_okbtn_clicked(Ewl_Widget * widget, void *ev_data, void *data)
{
	msgbox_close((MsgBox *) data);
	return;
}
