GtkWidget *CreateBarSubMenu (GtkWidget * menu, char *szName);
GtkWidget *CreateRightAlignBarSubMenu (GtkWidget * menu, char *szName);
GtkWidget *CreateMenuItem (GtkWidget * menu, char *szName,
			   char *szAccel, char *szTip,
			   GtkSignalFunc func, gpointer data);
