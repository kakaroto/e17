#ifndef _EW_MESSAGEBOX_H
#define _EW_MESSAGEBOX_H

#define ICON_SIZE 48
#define EW_MESSAGEBOX_ICON_QUESTION "question" 
#define EW_MESSAGEBOX_ICON_MESSAGE	"information"
#define EW_MESSAGEBOX_ICON_WARNING	"warning"
#define EW_MESSAGEBOX_ICON_ERROR		"error"

Entrance_Dialog ew_messagebox_ok(const char *type, const char *title, const char *message);
Entrance_Dialog ew_messagebox_ok_cancel(const char *, const char *, const char *, void (*)(void *, int, void *));
Entrance_Dialog ew_messagebox_yes_no(const char *, const char *, const char *, void (*)(void *, int, void *));
Entrance_Dialog ew_messagebox_yes_no_cancel(const char *, const char *, const char *, void (*)(void *, int, void *));


#endif
