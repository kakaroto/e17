#include "entrance_theme.h"

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
      fprintf(stderr, "Error reading theme db file %s", buf);
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

   t->welcome.font.name = e_db_str_get(db, "/entrance/welcome/font/name");
   t->welcome.font.style = e_db_str_get(db, "/entrance/welcome/font/style");
   e_db_int_get(db, "/entrance/welcome/font/size", &(t->welcome.font.size));
   e_db_int_get(db, "/entrance/welcome/color/r", &(t->welcome.color.r));
   e_db_int_get(db, "/entrance/welcome/color/g", &(t->welcome.color.g));
   e_db_int_get(db, "/entrance/welcome/color/b", &(t->welcome.color.b));
   e_db_int_get(db, "/entrance/welcome/color/a", &(t->welcome.color.a));
   e_db_float_get(db, "/entrance/welcome/pos/x", &(t->welcome.pos.x));
   e_db_float_get(db, "/entrance/welcome/pos/y", &(t->welcome.pos.y));
   e_db_int_get(db, "/entrance/welcome/offset/x", &(t->welcome.offset.x));
   e_db_int_get(db, "/entrance/welcome/offset/y", &(t->welcome.offset.y));

   t->password.font.name = e_db_str_get(db, "/entrance/passwd/font/name");
   t->password.font.style = e_db_str_get(db, "/entrance/passwd/font/style");
   e_db_int_get(db, "/entrance/passwd/font/size", &(t->password.font.size));
   e_db_int_get(db, "/entrance/passwd/color/r", &(t->password.color.r));
   e_db_int_get(db, "/entrance/passwd/color/g", &(t->password.color.g));
   e_db_int_get(db, "/entrance/passwd/color/b", &(t->password.color.b));
   e_db_int_get(db, "/entrance/passwd/color/a", &(t->password.color.a));

   t->entry.font.name = e_db_str_get(db, "/entrance/entry/font/name");
   t->entry.font.style = e_db_str_get(db, "/entrance/entry/font/style");
   e_db_int_get(db, "/entrance/entry/font/size", &(t->entry.font.size));
   e_db_int_get(db, "/entrance/entry/color/r", &(t->entry.color.r));
   e_db_int_get(db, "/entrance/entry/color/g", &(t->entry.color.g));
   e_db_int_get(db, "/entrance/entry/color/b", &(t->entry.color.b));
   e_db_int_get(db, "/entrance/entry/color/a", &(t->entry.color.a));
   e_db_float_get(db, "/entrance/entry/pos/x", &(t->entry.pos.x));
   e_db_float_get(db, "/entrance/entry/pos/y", &(t->entry.pos.y));
   e_db_int_get(db, "/entrance/entry/offset/x", &(t->entry.offset.x));
   e_db_int_get(db, "/entrance/entry/offset/y", &(t->entry.offset.y));

   e_db_int_get(db, "/entrance/entry/box/color/r", &(t->entry.box.color.r));
   e_db_int_get(db, "/entrance/entry/box/color/g", &(t->entry.box.color.g));
   e_db_int_get(db, "/entrance/entry/box/color/b", &(t->entry.box.color.b));
   e_db_int_get(db, "/entrance/entry/box/color/a", &(t->entry.box.color.a));
   e_db_float_get(db, "/entrance/entry/box/pos/x", &(t->entry.box.pos.x));
   e_db_float_get(db, "/entrance/entry/box/pos/y", &(t->entry.box.pos.y));
   e_db_int_get(db, "/entrance/entry/box/offset/x", &(t->entry.box.offset.x));
   e_db_int_get(db, "/entrance/entry/box/offset/y", &(t->entry.box.offset.y));
   e_db_int_get(db, "/entrance/entry/box/width", &(t->entry.box.size.w));
   e_db_int_get(db, "/entrance/entry/box/height", &(t->entry.box.size.h));

   t->selected_session.text.font.name =
      e_db_str_get(db, "/entrance/sessions/selected/font/name");
   t->selected_session.text.font.style =
      e_db_str_get(db, "/entrance/sessions/selected/font/style");
   e_db_int_get(db, "/entrance/sessions/selected/font/size",
                &(t->selected_session.text.font.size));
   e_db_int_get(db, "/entrance/sessions/selected/text/color/r",
                &(t->selected_session.text.color.r));
   e_db_int_get(db, "/entrance/sessions/selected/text/color/g",
                &(t->selected_session.text.color.g));
   e_db_int_get(db, "/entrance/sessions/selected/text/color/b",
                &(t->selected_session.text.color.b));
   e_db_int_get(db, "/entrance/sessions/selected/text/color/a",
                &(t->selected_session.text.color.a));
   e_db_int_get(db, "/entrance/sessions/selected/text/hicolor/r",
                &(t->selected_session.text.hicolor.r));
   e_db_int_get(db, "/entrance/sessions/selected/text/hicolor/g",
                &(t->selected_session.text.hicolor.g));
   e_db_int_get(db, "/entrance/sessions/selected/text/hicolor/b",
                &(t->selected_session.text.hicolor.b));
   e_db_int_get(db, "/entrance/sessions/selected/text/hicolor/a",
                &(t->selected_session.text.hicolor.a));
   e_db_float_get(db, "/entrance/sessions/selected/text/pos/x",
                  &(t->selected_session.text.pos.x));
   e_db_float_get(db, "/entrance/sessions/selected/text/pos/y",
                  &(t->selected_session.text.pos.y));
   e_db_int_get(db, "/entrance/sessions/selected/text/offset/x",
                &(t->selected_session.text.offset.x));
   e_db_int_get(db, "/entrance/sessions/selected/text/offset/y",
                &(t->selected_session.text.offset.y));
   e_db_float_get(db, "/entrance/sessions/selected/icon/pos/x",
                  &(t->selected_session.icon.pos.x));
   e_db_float_get(db, "/entrance/sessions/selected/icon/pos/y",
                  &(t->selected_session.icon.pos.y));
   e_db_int_get(db, "/entrance/sessions/selected/icon/offset/x",
                &(t->selected_session.icon.offset.x));
   e_db_int_get(db, "/entrance/sessions/selected/icon/offset/y",
                &(t->selected_session.icon.offset.y));
   e_db_int_get(db, "/entrance/sessions/selected/icon/width",
                &(t->selected_session.icon.size.w));
   e_db_int_get(db, "/entrance/sessions/selected/icon/height",
                &(t->selected_session.icon.size.h));

   t->session_list.font.name =
      e_db_str_get(db, "/entrance/sessions/list/text/font/name");
   t->session_list.font.style =
      e_db_str_get(db, "/entrance/sessions/list/text/font/style");
   e_db_int_get(db, "/entrance/sessions/list/text/font/size",
                &(t->session_list.font.size));
   e_db_int_get(db, "/entrance/sessions/list/text/color/r",
                &(t->session_list.color.r));
   e_db_int_get(db, "/entrance/sessions/list/text/color/g",
                &(t->session_list.color.g));
   e_db_int_get(db, "/entrance/sessions/list/text/color/b",
                &(t->session_list.color.b));
   e_db_int_get(db, "/entrance/sessions/list/text/color/a",
                &(t->session_list.color.a));

   t->session_list.sel_font.name =
      e_db_str_get(db, "/entrance/sessions/list/seltext/font/name");
   t->session_list.sel_font.style =
      e_db_str_get(db, "/entrance/sessions/list/seltext/font/style");
   e_db_int_get(db, "/entrance/sessions/list/seltext/font/size",
                &(t->session_list.sel_font.size));
   e_db_int_get(db, "/entrance/sessions/list/seltext/color/r",
                &(t->session_list.sel_color.r));
   e_db_int_get(db, "/entrance/sessions/list/seltext/color/g",
                &(t->session_list.sel_color.g));
   e_db_int_get(db, "/entrance/sessions/list/seltext/color/b",
                &(t->session_list.sel_color.b));
   e_db_int_get(db, "/entrance/sessions/list/seltext/color/a",
                &(t->session_list.sel_color.a));

   e_db_float_get(db, "/entrance/sessions/list/box/pos/x",
                  &(t->session_list.pos.x));
   e_db_float_get(db, "/entrance/sessions/list/box/pos/y",
                  &(t->session_list.pos.y));
   e_db_int_get(db, "/entrance/sessions/list/box/offset/x",
                &(t->session_list.offset.x));
   e_db_int_get(db, "/entrance/sessions/list/box/offset/y",
                &(t->session_list.offset.y));
   e_db_int_get(db, "/entrance/sessions/list/box/width",
                &(t->session_list.size.w));
   e_db_int_get(db, "/entrance/sessions/list/box/height",
                &(t->session_list.size.h));
   e_db_int_get(db, "/entrance/sessions/list/box/color/r",
                &(t->session_list.box_color.r));
   e_db_int_get(db, "/entrance/sessions/list/box/color/g",
                &(t->session_list.box_color.g));
   e_db_int_get(db, "/entrance/sessions/list/box/color/b",
                &(t->session_list.box_color.b));
   e_db_int_get(db, "/entrance/sessions/list/box/color/a",
                &(t->session_list.box_color.a));

   e_db_float_get(db, "/entrance/face/pos/x", &(t->face.pos.x));
   e_db_float_get(db, "/entrance/face/pos/y", &(t->face.pos.y));
   e_db_int_get(db, "/entrance/face/size/w", &(t->face.size.w));
   e_db_int_get(db, "/entrance/face/size/h", &(t->face.size.h));
   e_db_int_get(db, "/entrance/face/border", &(t->face.border));
   e_db_int_get(db, "/entrance/face/color/r", &(t->face.color.r));
   e_db_int_get(db, "/entrance/face/color/g", &(t->face.color.g));
   e_db_int_get(db, "/entrance/face/color/b", &(t->face.color.b));
   e_db_int_get(db, "/entrance/face/color/a", &(t->face.color.a));

   t->hostname.font.name = e_db_str_get(db, "/entrance/hostname/font/name");
   t->hostname.font.style = e_db_str_get(db, "/entrance/hostname/font/style");
   e_db_int_get(db, "/entrance/hostname/font/size", &(t->hostname.font.size));
   e_db_float_get(db, "/entrance/hostname/pos/x", &(t->hostname.pos.x));
   e_db_float_get(db, "/entrance/hostname/pos/y", &(t->hostname.pos.y));
   e_db_int_get(db, "/entrance/hostname/offset/x", &(t->hostname.offset.x));
   e_db_int_get(db, "/entrance/hostname/offset/y", &(t->hostname.offset.y));
   e_db_int_get(db, "/entrance/hostname/color/r", &(t->hostname.color.r));
   e_db_int_get(db, "/entrance/hostname/color/g", &(t->hostname.color.g));
   e_db_int_get(db, "/entrance/hostname/color/b", &(t->hostname.color.b));
   e_db_int_get(db, "/entrance/hostname/color/a", &(t->hostname.color.a));

   t->date.font.name = e_db_str_get(db, "/entrance/date/font/name");
   t->date.font.style = e_db_str_get(db, "/entrance/date/font/style");
   e_db_int_get(db, "/entrance/date/font/size", &(t->date.font.size));
   e_db_float_get(db, "/entrance/date/pos/x", &(t->date.pos.x));
   e_db_float_get(db, "/entrance/date/pos/y", &(t->date.pos.y));
   e_db_int_get(db, "/entrance/date/offset/x", &(t->date.offset.x));
   e_db_int_get(db, "/entrance/date/offset/y", &(t->date.offset.y));
   e_db_int_get(db, "/entrance/date/color/r", &(t->date.color.r));
   e_db_int_get(db, "/entrance/date/color/g", &(t->date.color.g));
   e_db_int_get(db, "/entrance/date/color/b", &(t->date.color.b));
   e_db_int_get(db, "/entrance/date/color/a", &(t->date.color.a));

   t->time.font.name = e_db_str_get(db, "/entrance/time/font/name");
   t->time.font.style = e_db_str_get(db, "/entrance/time/font/style");
   e_db_int_get(db, "/entrance/time/font/size", &(t->time.font.size));
   e_db_float_get(db, "/entrance/time/pos/x", &(t->time.pos.x));
   e_db_float_get(db, "/entrance/time/pos/y", &(t->time.pos.y));
   e_db_int_get(db, "/entrance/time/offset/x", &(t->time.offset.x));
   e_db_int_get(db, "/entrance/time/offset/y", &(t->time.offset.y));
   e_db_int_get(db, "/entrance/time/color/r", &(t->time.color.r));
   e_db_int_get(db, "/entrance/time/color/g", &(t->time.color.g));
   e_db_int_get(db, "/entrance/time/color/b", &(t->time.color.b));
   e_db_int_get(db, "/entrance/time/color/a", &(t->time.color.a));

   e_db_float_get(db, "/entrance/face/pos/x", &(t->face.pos.x));
   e_db_float_get(db, "/entrance/face/pos/y", &(t->face.pos.y));
   e_db_int_get(db, "/entrance/face/offset/x", &(t->face.offset.x));
   e_db_int_get(db, "/entrance/face/offset/y", &(t->face.offset.y));
   e_db_int_get(db, "/entrance/face/width", &(t->face.size.w));
   e_db_int_get(db, "/entrance/face/height", &(t->face.size.h));
   e_db_int_get(db, "/entrance/face/border", &(t->face.border));
   e_db_int_get(db, "/entrance/face/color/r", &(t->face.color.r));
   e_db_int_get(db, "/entrance/face/color/g", &(t->face.color.g));
   e_db_int_get(db, "/entrance/face/color/b", &(t->face.color.b));
   e_db_int_get(db, "/entrance/face/color/a", &(t->face.color.a));

   /* Close db */
   e_db_close(db);

   return t;
}
