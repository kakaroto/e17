#ifndef EWL_PASSWORD_H
#define EWL_PASSWORD_H

/**
 * @addtogroup Ewl_Password Ewl_Password: A Single Line Text Password Widget
 * @brief Defines the Ewl_Password class to allow for single line obscured text.
 *
 * @{
 */

/**
 * @themekey /password/file
 * @themekey /password/group
 */

/**
 * @def EWL_PASSWORD_TYPE
 * The type name for the Ewl_Password widget
 */
#define EWL_PASSWORD_TYPE "password"

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
 * Inherits from the Ewl_Entry and extends it to obscure the text.
 */
struct Ewl_Password
{
	Ewl_Entry  entry; /**< Inherit from Ewl_Entry */
	char      *real_text; /**< The password contents */
	char       obscure; /**< Character displayed for password characters */
};

Ewl_Widget	*ewl_password_new(void);
int		 ewl_password_init(Ewl_Password *e);

void		 ewl_password_text_set(Ewl_Password *e, const char *t);
char		*ewl_password_text_get(Ewl_Password *e);

void		 ewl_password_obscure_set(Ewl_Password *e, char o);
char		 ewl_password_obscure_get(Ewl_Password *e);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_password_key_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_password_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif

