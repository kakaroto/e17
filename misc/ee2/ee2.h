/* header file for Electric Eyes 2 */
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <Imlib2.h>
#include <gdk/gdkx.h>
#include <gdk/gdkprivate.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

/* functions */
void LoadImage(char *imagetoload);

void DrawChecks(void);
void Checks(int image_h, int image_w);

void ShowAbout(GtkWidget *widget, gpointer data);
void HideAbout(GtkWidget *widget, gpointer data);
void CloseWindow(GtkWidget *widget, gpointer data);
void CloseFileSel(GtkWidget *widget, gpointer data);
void FileOpen(GtkWidget *widget, GtkFileSelection *fs);
void OpenImageFromMenu(GtkWidget *widget, gpointer data);
void SaveImage(GtkWidget *widget, gpointer data);
void ShowSaveSel(GtkWidget *widget, gpointer data);
void CloseSaveSel(GtkWidget *widget, gpointer data);
void SaveImageAs(GtkWidget *widget, GtkFileSelection *fs);
void RefreshImage(GtkWidget *widget, gpointer data);
void BlurImage(GtkWidget *widget, gpointer data);
void BlurImageMore(GtkWidget *widget, gpointer data);
void SharpenImage(GtkWidget *widget, gpointer data);
void SharpenImageMore(GtkWidget *widget, gpointer data);
void Flip1(GtkWidget *widget, gpointer data);
void Flip2(GtkWidget *widget, gpointer data);
void Flip3(GtkWidget *widget, gpointer data);

gint ButtonPressed(GtkWidget *widget, GdkEvent *event, gpointer data);

gboolean a_config(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data);
gboolean a_expose(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data);
