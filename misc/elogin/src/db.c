#include "str.h"
#include "db.h"
#include "mem.h"

char               *
e_db_str_get(char *name, char *bit)
{
   char               *str;
   Elogin_Widget       widget;

   if (e_string_cmp(bit, "name"))
      str = name;
   else if (e_string_cmp(bit, "class"))
      str = "img";
   else if (e_string_cmp(bit, "rel1"))
      str = "";
   else if (e_string_cmp(bit, "rel2"))
      str = "";
   else
      str = "";

   if (e_string_cmp(name, "box"))
      widget = BOX;
   else if (e_string_cmp(name, "logo"))
      widget = LOGO;
   else if (e_string_cmp(name, "login_box"))
      widget = LOGIN_BOX;
   else if (e_string_cmp(name, "user_box"))
      widget = USER_BOX;
   else if (e_string_cmp(name, "pass_box"))
      widget = PASS_BOX;
   else
      widget = NONE;

   switch (widget)
     {
     case BOX:
	str = "images/box2.png";
	break;
     case LOGO:
	str = "images/elogin.png";
	break;
     case LOGIN_BOX:
	str = "images/pbox.png";
	break;
     case USER_BOX:
	str = "images/input.png";
	break;
     case PASS_BOX:
	str = "images/input.png";
	break;
     case NONE:
	str = "";
	break;
     }
   return str;
}

int
e_db_int_get(char *name, char *bit)
{
   int                 X = 0;

   if (e_string_cmp(name, "login_box"))
     {
	if (e_string_cmp(bit, "type"))
	   X = 0;
	else if (e_string_cmp(bit, "x1"))
	   X = 0;
	else if (e_string_cmp(bit, "y1"))
	   X = 0;
	else if (e_string_cmp(bit, "x2"))
	   X = 0;
	else if (e_string_cmp(bit, "y2"))
	   X = 0;
	else if (e_string_cmp(bit, "x"))
	   X = 0;
	else if (e_string_cmp(bit, "y"))
	   X = 0;
	else if (e_string_cmp(bit, "w"))
	   X = 200;
	else if (e_string_cmp(bit, "h"))
	   X = 100;
     }
   else if (e_string_cmp(name, "box"))
     {
	if (e_string_cmp(bit, "type"))
	   X = 0;
	else if (e_string_cmp(bit, "x1"))
	   X = 0;
	else if (e_string_cmp(bit, "y1"))
	   X = 0;
	else if (e_string_cmp(bit, "x2"))
	   X = 0;
	else if (e_string_cmp(bit, "y2"))
	   X = 0;
	else if (e_string_cmp(bit, "x"))
	   X = 0;
	else if (e_string_cmp(bit, "y"))
	   X = 0;
	else if (e_string_cmp(bit, "w"))
	   X = 500;
	else if (e_string_cmp(bit, "h"))
	   X = 350;
     }
   else if (e_string_cmp(name, "logo"))
     {
	if (e_string_cmp(bit, "type"))
	   X = 0;
	else if (e_string_cmp(bit, "x1"))
	   X = 0;
	else if (e_string_cmp(bit, "y1"))
	   X = 0;
	else if (e_string_cmp(bit, "x2"))
	   X = 0;
	else if (e_string_cmp(bit, "y2"))
	   X = 0;
	else if (e_string_cmp(bit, "x"))
	   X = 0;
	else if (e_string_cmp(bit, "y"))
	   X = 0;
	else if (e_string_cmp(bit, "w"))
	   X = 181;
	else if (e_string_cmp(bit, "h"))
	   X = 132;
     }
   else if (e_string_cmp(name, "user_box"))
     {
	if (e_string_cmp(bit, "type"))
	   X = 0;
	else if (e_string_cmp(bit, "x1"))
	   X = 0;
	else if (e_string_cmp(bit, "y1"))
	   X = 0;
	else if (e_string_cmp(bit, "x2"))
	   X = 0;
	else if (e_string_cmp(bit, "y2"))
	   X = 0;
	else if (e_string_cmp(bit, "x"))
	   X = 0;
	else if (e_string_cmp(bit, "y"))
	   X = 0;
	else if (e_string_cmp(bit, "w"))
	   X = 125;
	else if (e_string_cmp(bit, "h"))
	   X = 25;
     }
   else if (e_string_cmp(name, "pass_box"))
     {
	if (e_string_cmp(bit, "type"))
	   X = 0;
	else if (e_string_cmp(bit, "x1"))
	   X = 0;
	else if (e_string_cmp(bit, "y1"))
	   X = 0;
	else if (e_string_cmp(bit, "x2"))
	   X = 0;
	else if (e_string_cmp(bit, "y2"))
	   X = 0;
	else if (e_string_cmp(bit, "x"))
	   X = 0;
	else if (e_string_cmp(bit, "y"))
	   X = 0;
	else if (e_string_cmp(bit, "w"))
	   X = 125;
	else if (e_string_cmp(bit, "h"))
	   X = 25;
     }

   return X;
}
