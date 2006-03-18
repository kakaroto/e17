gint                CommsInit(void (*msg_receive_func) (gchar * msg));
void                CommsSend(gchar * s);

gchar               in_init;
gint                gdk_error_warnings;
