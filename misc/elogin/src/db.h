#ifndef DB_H
#define DB_H 1

enum _elogin_widget
{
	NONE,
	BOX,
	LOGO,
	LOGIN_BOX,
	USER_BOX,
	PASS_BOX
};

typedef enum	_elogin_widget	Elogin_Widget;

char *	e_db_str_get(char *name, char *bit);
int		e_db_int_get(char *name, char *bit);

#endif
