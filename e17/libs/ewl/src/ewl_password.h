#ifndef __EWL_PASSWORD_H__
#define __EWL_PASSWORD_H__

/**
 * @defgroup Ewl_Password Password: A Single Line Text Password Widget
 * @brief Defines the Ewl_Password class to allow for single line obscured text.
 *
 * @{
 */

/**
 * @themekey /password/file
 * @themekey /password/group
 */

/**
 * Inherits from the Ewl_Widget and provides single line editable text.
 */
typedef struct Ewl_Password Ewl_Password;

/**
 * @def EWL_PASSWORD(password)
 * Typecasts a pointer to an Ewl_Password pointer.
 */
#define EWL_PASSWORD(password) ((Ewl_Password *) password)

/**
 * @struct Ewl_Password
 * Inherits from the Ewl_Entry and extends it to obscure the text.
 */
struct Ewl_Password
{
	Ewl_Entry  entry; /**< Inherit from Ewl_Entry */
	char      *real_text; /**< The password contents */
	char       obscure; /**< Character displayed for password characters */
};

Ewl_Widget     *ewl_password_new(char *text);
void            ewl_password_init(Ewl_Password * e, char *text);
void            ewl_password_set_text(Ewl_Password * e, char *t);
char           *ewl_password_get_text(Ewl_Password * e);

void ewl_password_insert_text(Ewl_Password * e, char *s);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_password_key_down_cb(Ewl_Widget * w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif				/* __EWL_PASSWORD_H__ */
