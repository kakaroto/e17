#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @param text: the initial text to display in the widget
 * @return Returns a new password widget on success, NULL on failure.
 * @brief Allocate and initialize a new password widget
 */
Ewl_Widget     *ewl_password_new(char *text)
{
	Ewl_Password      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = NEW(Ewl_Password, 1);
	if (!e)
		return NULL;

	ewl_password_init(e, text);

	DRETURN_PTR(EWL_WIDGET(e), DLEVEL_STABLE);
}

/**
 * @param e: the password widget to initialize
 * @param text: the initial text to display in the widget
 * @return Returns no value.
 * @brief Initialize an password widget to default values
 *
 * Initializes the password widget @a e to it's default values and callbacks.
 */
void ewl_password_init(Ewl_Password * e, char *text)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	w = EWL_WIDGET(e);

	ewl_entry_init(EWL_ENTRY(w), NULL);
	ewl_widget_inherit(w, "password");
	e->obscure = '*';

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_del(w, EWL_CALLBACK_KEY_DOWN, ewl_entry_cb_key_down);
	ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN, ewl_entry_cb_mouse_down);
	ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, ewl_entry_cb_mouse_move);
	ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN, ewl_password_key_down_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, ewl_password_destroy,
			    NULL);

	ewl_password_text_set(e, text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the password widget to change the text
 * @param t: the text to set for the password widget
 * @return Returns no value.
 * @brief Set the text for an password widget
 *
 * Change the text of the password widget @a e to the string @a t.
 */
void ewl_password_text_set(Ewl_Password * e, char *t)
{
	char *vis = NULL;
	int len;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	/*
	 * Zero this out just in case a segv occurs (by the end
	 * programmer, not EWL of course ;) so passwords aren't
	 * revealed in the resulting core dump.
	 */
	if (e->real_text) {
		ZERO(e->real_text, char, strlen(e->real_text));
		FREE(e->real_text);
	}

	if (t) {
		len = strlen(t);
		e->real_text = strdup(t);
		vis = NEW(char, len + 1);
		memset(vis, e->obscure, len);
	}

	ewl_text_text_set(EWL_TEXT(e), vis);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the password widget to retrieve the text
 * @return Returns the password text on success, NULL on failure.
 * @brief Get the text from an password widget
 */
char           *ewl_password_text_get(Ewl_Password * e)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, NULL);

	w = EWL_WIDGET(e);

	DRETURN_PTR((e->real_text ? strdup(e->real_text) : NULL),
			DLEVEL_STABLE);
}

/**
 * @param e: the password to retrieve the obscuring character
 * @return Returns the character value of the obscuring character.
 * @brief Retrieves the character used to obscure the text for a password.
 */
char ewl_password_obscure_get(Ewl_Password * e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, 0);

	DRETURN_INT(e->obscure, DLEVEL_STABLE);
}

/**
 * @param e: set the obscuring character for a passwords text
 * @param o: the character to obscure the password characters
 * @return Returns no value.
 * @brief Sets the character used to obscure the text for a password.
 */
void ewl_password_obscure_set(Ewl_Password * e, char o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	e->obscure = o;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_password_text_insert(Ewl_Password * e, char *s)
{
	char           *s2, *s3;
	int             l = 0, l2 = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	s2 = ewl_password_text_get(e);
	if (s)
		l = strlen(s);
	if (s2)
		l2 = strlen(s2);

	s3 = NEW(char, l + l2 + 1);
	if (!s3) {
		IF_FREE(s2);
		DRETURN(DLEVEL_STABLE);
	}

	s3[0] = 0;
	if (s2)
		strcat(s3, s2);
	if (s)
		strcat(s3, s);

	ewl_password_text_set(e, s3);

	IF_FREE(s2);
	FREE(s3);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Handle key events to modify the text of the password widget.
 */
void ewl_password_key_down_cb(Ewl_Widget * w, void *ev_data,
					void *user_data __UNUSED__)
{
	int len;
	char *tmp;
	Ewl_Password *e;
	Ewl_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_PASSWORD(w);
	ev = ev_data;

	if (!strcmp(ev->keyname, "BackSpace")) {
		tmp = ewl_text_text_get(EWL_TEXT(e));
		if (tmp && (len = strlen(tmp))) {
			tmp[len - 1] = '\0';
			ewl_text_text_set(EWL_TEXT(e), tmp);
			e->real_text[len - 1] = '\0';
			FREE(tmp);
		}
	}
	else if (!strcmp(ev->keyname, "Return") || !strcmp(ev->keyname,
				"KP_Return"))
		ewl_callback_call_with_event_data(w, EWL_CALLBACK_VALUE_CHANGED,
				ewl_text_text_get(EWL_TEXT(w)));
	else if (!strcmp(ev->keyname, "Enter") || !strcmp(ev->keyname,
				"KP_Enter"))
		ewl_callback_call_with_event_data(w, EWL_CALLBACK_VALUE_CHANGED,
				ewl_text_text_get(EWL_TEXT(w)));
	else if (ev->keyname) {
		ewl_password_text_insert(e, ev->keyname);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_password_destroy(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Password *p = EWL_PASSWORD(w);

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (p->real_text) {
		ZERO(p->real_text, char, strlen(p->real_text));
		FREE(p->real_text);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
