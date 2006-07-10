#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_password_text_insert(Ewl_Password *e, const char *s);

/**
 * @return Returns a new password widget on success, NULL on failure.
 * @brief Allocate and initialize a new password widget
 */
Ewl_Widget *
ewl_password_new(void)
{
	Ewl_Password *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = NEW(Ewl_Password, 1);
	if (!e)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_password_init(e)) {
		ewl_widget_destroy(EWL_WIDGET(e));
		e = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(e), DLEVEL_STABLE);
}

/**
 * @param e: the password widget to initialize
 * @return Returns no value.
 * @brief Initialize an password widget to default values
 *
 * Initializes the password widget @a e to it's default values and callbacks.
 */
int
ewl_password_init(Ewl_Password *e)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	w = EWL_WIDGET(e);

	if (!ewl_entry_init(EWL_ENTRY(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(w, EWL_PASSWORD_TYPE);
	e->obscure = '*';

	/* text is not selectable */
	ewl_text_selectable_set(EWL_TEXT(w), FALSE);

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_del(w, EWL_CALLBACK_KEY_DOWN, ewl_entry_cb_key_down);
	ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN, ewl_entry_cb_mouse_down);
	ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN, ewl_password_key_down_cb,
			    NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_password_destroy_cb,
			    NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param e: the password widget to change the text
 * @param t: the text to set for the password widget
 * @return Returns no value.
 * @brief Set the text for an password widget
 *
 * Change the text of the password widget @a e to the string @a t.
 */
void
ewl_password_text_set(Ewl_Password *e, const char *t)
{
	char *vis = NULL;
	int len;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_PASSWORD_TYPE);

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
char *
ewl_password_text_get(Ewl_Password *e)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, NULL);
	DCHECK_TYPE_RET("e", e, EWL_PASSWORD_TYPE, NULL);

	w = EWL_WIDGET(e);

	DRETURN_PTR((e->real_text ? strdup(e->real_text) : NULL),
			DLEVEL_STABLE);
}

/**
 * @param e: the password to retrieve the obscuring character
 * @return Returns the character value of the obscuring character.
 * @brief Retrieves the character used to obscure the text for a password.
 */
char
ewl_password_obscure_get(Ewl_Password *e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, 0);
	DCHECK_TYPE_RET("e", e, EWL_PASSWORD_TYPE, 0);

	DRETURN_INT(e->obscure, DLEVEL_STABLE);
}

/**
 * @param e: set the obscuring character for a passwords text
 * @param o: the character to obscure the password characters
 * @return Returns no value.
 * @brief Sets the character used to obscure the text for a password.
 */
void
ewl_password_obscure_set(Ewl_Password *e, char o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_PASSWORD_TYPE);

	e->obscure = o;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_password_text_insert(Ewl_Password *e, const char *s)
{
	char *s2, *s3;
	int l = 0, l2 = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_PASSWORD_TYPE);

	s2 = ewl_password_text_get(e);
	if (s) l = strlen(s);
	if (s2) l2 = strlen(s2);

	s3 = NEW(char, l + l2 + 1);
	if (!s3) {
		if (s2) 
		{
			ZERO(s2, char, strlen(s2));
			FREE(s2);
		}
		DRETURN(DLEVEL_STABLE);
	}

	s3[0] = 0;
	if (s2) strcat(s3, s2);
	if (s) strcat(s3, s);

	ewl_password_text_set(e, s3);

	if (s2)
	{
		ZERO(s2, char, strlen(s2));
		FREE(s2);
	}

	ZERO(s3, char, strlen(s3));
	FREE(s3);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Key_Down data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Handle key events to modify the text of the password widget.
 */
void
ewl_password_key_down_cb(Ewl_Widget *w, void *ev_data,
				void *user_data __UNUSED__)
{
	int len;
	Ewl_Password *e;
	Ewl_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	e = EWL_PASSWORD(w);
	ev = ev_data;

	if (!strcmp(ev->keyname, "BackSpace")) {
		if ((len = ewl_text_length_get(EWL_TEXT(e))) > 0)
			e->real_text[len - 1] = '\0';

		ewl_entry_delete_left(EWL_ENTRY(e));
	}

	else if ((!strcmp(ev->keyname, "Return"))
			|| (!strcmp(ev->keyname, "KP_Return"))
			|| (!strcmp(ev->keyname, "Enter")) 
			|| (!strcmp(ev->keyname, "KP_Enter"))
			|| (!strcmp(ev->keyname, "\n")))
		ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

	else if ((!strcmp(ev->keyname, "Left")) 
			|| (!strcmp(ev->keyname, "Right"))
			|| (!strcmp(ev->keyname, "Up"))
			|| (!strcmp(ev->keyname, "Down")) 
			|| (!strcmp(ev->keyname, "Delete"))) {
		/* ignore these */
	}
	else if (ev->keyname)
		ewl_password_text_insert(e, ev->keyname);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_password_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	Ewl_Password *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	p = EWL_PASSWORD(w);
	if (p->real_text) {
		ZERO(p->real_text, char, strlen(p->real_text));
		FREE(p->real_text);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

