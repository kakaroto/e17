GtkWidget          *CreateBarSubMenu(GtkWidget * menu, const char *szName);
GtkWidget          *CreateRightAlignBarSubMenu(GtkWidget * menu,
					       const char *szName);
GtkWidget          *CreateMenuItem(GtkWidget * menu, const char *szName,
				   const char *szAccel, const char *szTip,
				   GtkSignalFunc func, const void *data);
