/*
 * Copyright (C) 2000, Tuncer M. Ayaz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <epplet.h>

Epplet_gadget btn_help, btn_reload, btn_close, p; 

static void On_Btn_Help		( void *data );
static void On_Btn_Close	( void *data );
static void On_List_Reload	( void *data );
static void On_List_Select	( void *data );
static void On_Focus_In		( void *data, Window  w );
static void On_Focus_Out	( void *data, Window w );

static void
On_Btn_Help ( void *data )
{
	Epplet_show_about ("E-GtkRc");

	return;
	data = NULL;
}

static void
On_Btn_Close (void *data)
{
	Epplet_unremember();
	Esync();
	Epplet_cleanup();
	data = NULL;
	exit(0);
}

static void
On_List_Reload ( void *data )
{
	int i, num_popup_entries;
	char *s;
	FILE *f;

	s = (char *) calloc (1024, sizeof(char));

	system ("cd ~/.gtk; ls .gtkrc.* | sed s/.gtkrc.// | cat > ~/gtkRCs.txt");

	sprintf (s, "%s/gtkRCs.txt", getenv("HOME"));
	if ( !(f = fopen(s, "r")) )
		return;

	/* scan ~/.gtk/ and reload the current themes into the list */
	if ( p ) {
		num_popup_entries = Epplet_popup_entry_num ( p );
		for ( i = num_popup_entries; i > 0; i-- )
			Epplet_remove_popup_entry ( p, i - 1 );
		p = Epplet_create_popup();
		Epplet_gadget_show( Epplet_create_popupbutton ("Themes", NULL, 2, 20, 44, 13, NULL, p) );
	}

	Epplet_add_popup_entry (p, "-Empty-", NULL, NULL, NULL);

	while ( (fscanf(f, "%s", s)) != EOF )
		Epplet_add_popup_entry (p, s, NULL, On_List_Select, strdup(s));

	fclose(f);
	free(s);
	
	system("rm ~/gtkRCs.txt");
	
	return;
	data = NULL;
}

static void
On_List_Select ( void *data )
{
	char *s;
	
	s = (char *) calloc (1024, sizeof(char));
	sprintf (s, "ln -sf ~/.gtk/.gtkrc.%s ~/.gtkrc", (char *) data);
	system(s);

	if ( getenv("EBIN") ) {
		sprintf ( s, "%s/E-GtkRc-Tester", getenv("EBIN") );
		system(s);
	}
	else
		system ("/usr/local/enlightenment/bin/E-GtkRc-Tester");

	free(s);
	
	return;
	data = NULL;
}

  
static void
On_Focus_In (void *data, Window w)
{
	if ( w == Epplet_get_main_window() ) {
		Epplet_gadget_show(btn_help);
		Epplet_gadget_show(btn_close);
	}

	return;
	data = NULL;
	w = (Window) 0;
}

static void
On_Focus_Out (void *data, Window w)
{
	if ( w == Epplet_get_main_window() ) {
		Epplet_gadget_hide(btn_help);
		Epplet_gadget_hide(btn_close);
	}

	return;
	data = NULL;
	w = (Window) 0;
}

int
main(int argc, char **argv)
{
	Epplet_Init( "E-GtkRc", "0.3", "Gtk-Themes-Switcher", 3, 3, argc, argv, 0 );
   
	btn_help = Epplet_create_button( NULL, NULL, 3, 2, 0, 0, "HELP", 0, NULL,  On_Btn_Help, NULL );
	btn_reload = Epplet_create_button( NULL, NULL, 18, 2, 0, 0, "REPEAT", 0, NULL, On_List_Reload, NULL );
	btn_close = Epplet_create_button( NULL, NULL, 33, 2, 0, 0, "CLOSE", 0, NULL, On_Btn_Close, NULL );
	
	Epplet_gadget_show( btn_help );
	Epplet_gadget_show( btn_reload );
	Epplet_gadget_show( btn_close );
	p = Epplet_create_popup();
	Epplet_gadget_show( Epplet_create_popupbutton ("Themes", NULL, 2, 20, 44, 13, NULL, p) );
	
	Epplet_register_focus_in_handler  (On_Focus_In, NULL);
	Epplet_register_focus_out_handler (On_Focus_Out, NULL);

	On_List_Reload (NULL);	/* initially load themes-list */

	Epplet_show();
	Epplet_Loop();
	return 0;
}
