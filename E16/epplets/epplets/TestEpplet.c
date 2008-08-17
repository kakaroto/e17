#include "epplet.h"

int                 val = 0;
int                 val2 = 0;
int                 val3 = 33;
Epplet_gadget       prog1, prog2, prog3, prog4, tog, sl1, sl2, da, im;

static void         cb_timer(void *data);
static void         cb_close(void *data);

static void
cb_timer(void *data)
{

   val3++;
   if (val3 > 100)
      val3 = 0;
   val = rand() & 0x1;
   val2++;
   if (val2 > 50)
      val2 = 0;
   Epplet_gadget_data_changed(prog1);
   Epplet_gadget_data_changed(prog2);
   Epplet_gadget_data_changed(prog3);
   Epplet_gadget_data_changed(prog4);
   Epplet_gadget_data_changed(tog);
   Epplet_gadget_data_changed(sl1);
   Epplet_gadget_data_changed(sl2);
   if (val)
      Epplet_change_image(im, 24, 24, "/usr/share/pixmaps/gnome-term.png");
   else
      Epplet_change_image(im, 24, 24, "/usr/share/pixmaps/gnome-globe.png");
   Epplet_timer(cb_timer, NULL, 0.05, "TIMER");
   data = NULL;
}

static void
cb_close(void *data)
{
   Epplet_unremember();
   Esync();
   Epplet_cleanup();
   data = NULL;
   exit(0);
}

int
main(int argc, char **argv)
{
   Epplet_gadget       p;

   Epplet_Init("E-TestEpplet", "0.1", "Enlightenment Test Epplet",
	       12, 4, argc, argv, 0);
   Epplet_timer(cb_timer, NULL, 0.05, "TIMER");
   Epplet_gadget_show(Epplet_create_button("Test", NULL,
					   2, 2, 40, 12, NULL, 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(tog = Epplet_create_togglebutton("Toggle", NULL,
						       2, 14, 40, 12,
						       &val, NULL, NULL));
   Epplet_gadget_show(da = Epplet_create_drawingarea(2, 26, 40, 24));
   Epplet_gadget_show(sl1 = Epplet_create_hslider(2, 50, 40,
						  0, 50,
						  5, 10, &val2, NULL, NULL));
   Epplet_gadget_show(sl2 = Epplet_create_vslider(42, 2, 60,
						  0, 50,
						  5, 10, &val2, NULL, NULL));
   Epplet_gadget_show(prog1 = Epplet_create_vbar(50, 2, 8, 60, 0, &val3));
   Epplet_gadget_show(prog2 = Epplet_create_hbar(58, 2, 40, 8, 0, &val3));
   Epplet_gadget_show(prog3 = Epplet_create_vbar(90, 12, 8, 40, 1, &val3));
   Epplet_gadget_show(prog4 = Epplet_create_hbar(58, 54, 40, 8, 1, &val3));
   Epplet_gadget_show(Epplet_create_label(58, 10, "Label", 0));
   Epplet_gadget_show(im = Epplet_create_image(58, 20, 24, 24,
					       "/usr/share/pixmaps/gnome-globe.png"));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   98, 2, 0, 0, "ARROW_UP", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   98, 14, 0, 0, "ARROW_DOWN", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   98, 26, 0, 0, "ARROW_LEFT", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   98, 38, 0, 0, "ARROW_RIGHT", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   98, 50, 0, 0, "PLAY", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   110, 2, 0, 0, "STOP", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   110, 14, 0, 0, "PAUSE", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   110, 26, 0, 0, "PREVIOUS", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   110, 38, 0, 0, "NEXT", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   110, 50, 0, 0, "EJECT", 0, NULL,
					   NULL, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   122, 2, 0, 0, "CLOSE", 0, NULL,
					   cb_close, NULL));
   Epplet_gadget_show(Epplet_create_button(NULL, NULL,
					   122, 14, 0, 0, "FAST_FORWARD", 0,
					   NULL, NULL, NULL));
   Epplet_gadget_show(Epplet_create_button
		      (NULL, NULL, 122, 26, 0, 0, "REWIND", 0, NULL, NULL,
		       NULL));
   Epplet_gadget_show(Epplet_create_button
		      (NULL, NULL, 122, 38, 0, 0, "REPEAT", 0, NULL, NULL,
		       NULL));
   Epplet_gadget_show(Epplet_create_button
		      (NULL, NULL, 122, 50, 0, 0, "SKIP", 0, NULL, NULL, NULL));
   Epplet_gadget_show(Epplet_create_button
		      (NULL, NULL, 134, 2, 0, 0, "HELP", 0, NULL, NULL, NULL));
   Epplet_gadget_show(Epplet_create_button
		      (NULL, NULL, 134, 14, 0, 0, "CONFIGURE", 0, NULL, NULL,
		       NULL));
   p = Epplet_create_popup();
   Epplet_add_popup_entry(p, "Entry 1", NULL, NULL, NULL);
   Epplet_add_popup_entry(p, "Entry 2", NULL, NULL, NULL);
   Epplet_add_popup_entry(p, "Entry 3", NULL, NULL, NULL);
   Epplet_add_popup_entry(p, "Entry 4", NULL, NULL, NULL);
   Epplet_gadget_show(Epplet_create_popupbutton("Popup", NULL,
						146, 2, 40, 12, NULL, p));
   Epplet_show();
   Epplet_Loop();
   return 0;
}
