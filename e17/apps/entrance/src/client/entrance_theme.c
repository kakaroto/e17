#include "entrance_theme.h"

#define COLOR_GET(src, col) \
   { \
      str = e_db_str_get(db, (src)); \
      if (str) \
      { \
         color_parse(str, (col)); \
         free(str); \
      } \
   }

/**
 * color_parse: Parses a color value @str in the form "#rrggbbaa"
 *              into the E_Color struct pointed to by @c
 * Returns 1 if an error occured, else 0.
 */
static int
color_parse(char *str, E_Color *c)
{
   if (!c)
      return 1;

   if (!str || !str[0] || str[0] != '#' || strlen(str) < 9)
      memset(c, 0, sizeof(E_Color));
   else
   {
      sscanf(&str[1], "%02x", &(*c).r);
      sscanf(&str[3], "%02x", &(*c).g);
      sscanf(&str[5], "%02x", &(*c).b);
      sscanf(&str[7], "%02x", &(*c).a);
   }

   return 0;
}

#define OFFSET_GET(src, offs) \
   { \
      str = e_db_str_get(db, (src)); \
      if (str) \
      { \
         offset_parse(str, (offs)); \
		 free(str); \
      } \
   }
/**
 * offset_parse: Parses an offset value @str in the form "x:y"
 *               into the E_Offset struct pointed to by @o
 *  Returns 1 if an error occured, else 0.
 */
static int
offset_parse(char *str, E_Offset *o)
{
   if (!o)
      return 1;

   if (!str || !str[0])
      memset(o, 0, sizeof(E_Offset));
   else
      sscanf(str, "%i:%i", &(*o).x, &(*o).y);

   return 0;
}

#define SIZE_GET(src, size) \
   { \
      str = e_db_str_get(db, (src)); \
      if (str) \
      { \
         size_parse(str, (size)); \
         free(str); \
      } \
   }

/**
 * size_parse: Parses a size value @str in the form "w:h"
 *             into the E_Size struct pointed to by @s
 *  Returns 1 if an error occured, else 0.
 */
static int
size_parse(char *str, E_Size *s)
{
   if (!s)
      return 1;

   if (!str || !str[0])
      memset(s, 0, sizeof(E_Size));
   else
      sscanf(str, "%i:%i", &(*s).w, &(*s).h);

   return 0;
}

#define POS_GET(src, pos) \
   { \
      str = e_db_str_get(db, (src)); \
      if (str) \
      { \
         pos_parse(str, (pos)); \
         free(str); \
      } \
   }
/**
 * pos_parse: Parses a position value @str in the form "x:y"
 *            into the E_Pos struct pointed to by @p
 * Returns 1 if an error occured, else 0.
 */
static int
pos_parse(char *str, E_Pos *p)
{
   if (!p)
      return 1;

   if (!str || !str[0])
      memset(p, 0, sizeof(E_Pos));
   else
      sscanf(str, "%f:%f", &(*p).x, &(*p).y);
   
   return 0;
}

Entrance_Theme
entrance_theme_parse(char *name, char *path)
{
   char buf[PATH_MAX];
   Entrance_Theme t;
   E_DB_File *db;
   char *str;

   if (!path)
      return NULL;

   snprintf(buf, PATH_MAX, "%s/%s.db", path, name);
   if (!(db = e_db_open_read(buf)))
   {
      syslog(LOG_CRIT, "Error reading theme db file %s.", buf);
      return NULL;
   }

   t = (Entrance_Theme) malloc(sizeof(struct _Entrance_Theme));
   memset(t, 0, sizeof(struct _Entrance_Theme));

   str = e_db_str_get(db, "/entrance/background");
   if (str)
   {
      snprintf(buf, PATH_MAX, "%s/%s", path, str);
      t->bg = strdup(buf);
      free(str);
   }

   str = e_db_str_get(db, "/entrance/pointer");
   if (str)
   {
      snprintf(buf, PATH_MAX, "%s/%s", path, str);
      t->pointer = strdup(buf);
      free(str);
   }

   /* welcome */
   t->welcome.font.name = e_db_str_get(db, "/entrance/welcome/font/name");
   t->welcome.font.style = e_db_str_get(db, "/entrance/welcome/font/style");
   e_db_int_get(db, "/entrance/welcome/font/size", &(t->welcome.font.size));
   
   COLOR_GET("/entrance/welcome/color", &t->welcome.color);
   POS_GET("/entrance/welcome/pos", &t->welcome.pos);
   OFFSET_GET("/entrance/welcome/offset", &t->welcome.offset);
   
   /* password */
   t->password.font.name = e_db_str_get(db, "/entrance/passwd/font/name");
   t->password.font.style = e_db_str_get(db, "/entrance/passwd/font/style");
   e_db_int_get(db, "/entrance/passwd/font/size", &(t->password.font.size));

   COLOR_GET("/entrance/passwd/color", &t->password.color);
   
   t->entry.font.name = e_db_str_get(db, "/entrance/entry/font/name");
   t->entry.font.style = e_db_str_get(db, "/entrance/entry/font/style");
   e_db_int_get(db, "/entrance/entry/font/size", &(t->entry.font.size));
  
   /* entry */
   COLOR_GET("/entrance/entry/color", &t->entry.color);
   POS_GET("/entrance/entry/pos", &t->entry.pos);
   OFFSET_GET("/entrance/entry/offset", &t->entry.offset);

   COLOR_GET("/entrance/entry/box/color", &t->entry.box.color);
   POS_GET("/entrance/entry/box/pos", &t->entry.box.pos);
   OFFSET_GET("/entrance/entry/box/offset", &t->entry.box.offset);
   SIZE_GET("/entrance/entry/box/size", &t->entry.box.size);
   
   /* selected session */
   t->selected_session.text.font.name =
      e_db_str_get(db, "/entrance/sessions/selected/font/name");
   t->selected_session.text.font.style =
      e_db_str_get(db, "/entrance/sessions/selected/font/style");
   e_db_int_get(db, "/entrance/sessions/selected/font/size",
                &(t->selected_session.text.font.size));

   COLOR_GET("/entrance/sessions/selected/text/color", &t->selected_session.text.color);
   COLOR_GET("/entrance/sessions/selected/text/hicolor", &t->selected_session.text.hicolor);
   POS_GET("/entrance/sessions/selected/text/pos", &t->selected_session.text.pos);
   OFFSET_GET("/entrance/sessions/selected/text/offset", &t->selected_session.text.offset);
   
   POS_GET("/entrance/sessions/selected/icon/pos", &t->selected_session.icon.pos);
   OFFSET_GET("/entrance/sessions/selected/icon/offset", &t->selected_session.icon.offset);
   SIZE_GET("/entrance/sessions/selected/icon/size", &t->selected_session.icon.size);
  
   /* session list */
   t->session_list.font.name =
      e_db_str_get(db, "/entrance/sessions/list/text/font/name");
   t->session_list.font.style =
      e_db_str_get(db, "/entrance/sessions/list/text/font/style");
   e_db_int_get(db, "/entrance/sessions/list/text/font/size",
                &(t->session_list.font.size));
  
   COLOR_GET("/entrance/sessions/list/text/color", &t->session_list.color);
   
   t->session_list.sel_font.name =
      e_db_str_get(db, "/entrance/sessions/list/seltext/font/name");
   t->session_list.sel_font.style =
      e_db_str_get(db, "/entrance/sessions/list/seltext/font/style");
   e_db_int_get(db, "/entrance/sessions/list/seltext/font/size",
                &(t->session_list.sel_font.size));

   COLOR_GET("/entrance/sessions/list/seltext/color", &t->session_list.sel_color);
   POS_GET("/entrance/sessions/list/box/pos", &t->session_list.pos);
   OFFSET_GET("/entrance/sessions/list/box/offset", &t->session_list.offset);
   SIZE_GET("/entrance/sessions/list/box/size", &t->session_list.size);
   
   COLOR_GET("/entrance/sessions/list/box/color", &t->session_list.box_color);
   
   /* face */
   COLOR_GET("/entrance/face/color", &t->face.color);
   POS_GET("/entrance/face/pos", &t->face.pos);
   OFFSET_GET("/entrance/face/offset", &t->face.offset);
   SIZE_GET("/entrance/face/size", &t->face.size);
   e_db_int_get(db, "/entrance/face/border", &(t->face.border));

   /* hostname */
   t->hostname.font.name = e_db_str_get(db, "/entrance/hostname/font/name");
   t->hostname.font.style = e_db_str_get(db, "/entrance/hostname/font/style");
   e_db_int_get(db, "/entrance/hostname/font/size", &(t->hostname.font.size));

   POS_GET("/entrance/hostname/pos", &t->hostname.pos);
   OFFSET_GET("/entrance/hostname/offset", &t->hostname.offset);
   COLOR_GET("/entrance/hostname/color", &t->hostname.color);
  
   /* date */
   t->date.font.name = e_db_str_get(db, "/entrance/date/font/name");
   t->date.font.style = e_db_str_get(db, "/entrance/date/font/style");
   e_db_int_get(db, "/entrance/date/font/size", &(t->date.font.size));
   
   COLOR_GET("/entrance/date/color", &t->date.color);
   POS_GET("/entrance/date/pos", &t->date.pos);
   OFFSET_GET("/entrance/date/offset", &t->date.offset);

   /* time */
   t->time.font.name = e_db_str_get(db, "/entrance/time/font/name");
   t->time.font.style = e_db_str_get(db, "/entrance/time/font/style");
   e_db_int_get(db, "/entrance/time/font/size", &(t->time.font.size));

   COLOR_GET("/entrance/time/color", &t->time.color);
   POS_GET("/entrance/time/pos", &t->time.pos);
   OFFSET_GET("/entrance/time/offset", &t->time.offset);

   /* Close db */
   e_db_close(db);

   return t;
}
