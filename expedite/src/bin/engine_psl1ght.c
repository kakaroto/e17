#include "main.h"

#include <Evas_Engine_PSL1GHT.h>
#include <io/mouse.h>
#include <io/kb.h>

static int window_width = 0;
static int window_height = 0;

Eina_Bool
engine_psl1ght_args(const char *engine, int width, int height)
{
   Evas_Engine_Info_PSL1GHT *einfo;

   evas_output_method_set(evas, evas_render_method_lookup("psl1ght"));
   einfo = (Evas_Engine_Info_PSL1GHT *)evas_engine_info_get(evas);
   if (!einfo)
     {
        printf("Evas does not support the PSL1GHT Engine\n");
        return EINA_FALSE;
     }

   if (!evas_engine_info_set(evas, (Evas_Engine_Info *)einfo))
     {
        printf("Evas can not setup the informations of the PSL1GHT Engine\n");
        return EINA_FALSE;
     }

   window_width = width;
   window_height = height;
   ioMouseInit(1);
   ioKbInit(1);
   return EINA_TRUE;
}

void
engine_psl1ght_loop(void)
{
   mouseInfo mouseinfo;
   static int mouse_connected = FALSE;
   static u8 mouse_buttons = 0;
   static int mouse_x = 0;
   static int mouse_y = 0;
   KbInfo kbInfo;
   static int keyboard_connected = FALSE;
   static KbLed keyboard_leds = {{0}};
   static KbMkey keyboard_mods = {{0}};
   static u16 keyboard_old_key = 0;
   int i;

   /* Check mouse events */
   ioMouseGetInfo(&mouseinfo);

   if (mouseinfo.status[0] == 1 && !mouse_connected) // Connected
     {
        mouse_connected = TRUE;
        mouse_buttons = 0;

        // Old events in the queue are discarded
        ioMouseClearBuf(0);
     }
   else if (mouseinfo.status[0] != 1 && mouse_connected) // Disconnected
     {
        mouse_connected = FALSE;
        mouse_buttons = 0;
     }

   if (mouse_connected)
     {
        mouseDataList datalist;

        ioMouseGetDataList(0, &datalist);

        for (i = 0; i < datalist.count; i++) {
             u8 old_left = mouse_buttons & 1;
             u8 new_left = datalist.list[i].buttons & 1;
             u8 old_right = mouse_buttons & 2;
             u8 new_right = datalist.list[i].buttons & 2;
             u8 old_middle = mouse_buttons & 4;
             u8 new_middle = datalist.list[i].buttons & 4;

             mouse_x += datalist.list[i].x_axis;
             mouse_y += datalist.list[i].y_axis;
             if (mouse_x < 0) mouse_x = 0;
             if (mouse_y < 0) mouse_y = 0;
             if (mouse_x > window_width) mouse_x = window_width;
             if (mouse_y > window_height) mouse_x = window_height;

             evas_event_feed_mouse_move(evas, mouse_x, mouse_y, 0, NULL);

             if (old_left != new_left)
               {
                  if (new_left)
                    evas_event_feed_mouse_down(evas, 1, EVAS_BUTTON_NONE, 0, NULL);
                  else
                    evas_event_feed_mouse_up(evas, 1, EVAS_BUTTON_NONE, 0, NULL);
               }
             if (old_right != new_right)
               {
                  if (new_right)
                    evas_event_feed_mouse_down(evas, 3, EVAS_BUTTON_NONE, 0, NULL);
                  else
                    evas_event_feed_mouse_up(evas, 3, EVAS_BUTTON_NONE, 0, NULL);
               }
             if (old_middle != new_middle)
               {
                  if (new_middle)
                    evas_event_feed_mouse_down(evas, 2, EVAS_BUTTON_NONE, 0, NULL);
                  else
                    evas_event_feed_mouse_up(evas, 2, EVAS_BUTTON_NONE, 0, NULL);
               }
             mouse_buttons = datalist.list[i].buttons;
          }
     }

   /* Check keyboard events */
   ioKbGetInfo(&kbInfo);

   if (kbInfo.status[0] == 1 && !keyboard_connected)
     {
        /* Connected */
         keyboard_connected = true;

         // Old events in the queue are discarded
         ioKbClearBuf(0);
         keyboard_leds._KbLedU.leds = 0;
         keyboard_mods._KbMkeyU.mkeys = 0;
         keyboard_old_key = 0;

         // Set raw keyboard code types to get scan codes
         ioKbSetCodeType(0, KB_CODETYPE_ASCII);
         ioKbSetReadMode(0, KB_RMODE_INPUTCHAR);
     }
   else if (kbInfo.status[0] != 1 && keyboard_connected)
     {
        /* Disconnected keyboard */
         keyboard_connected = FALSE;
     }

   if (keyboard_connected)
     {
        KbData Keys;

        // Read data from the keyboard buffer
        if (ioKbRead(0, &Keys) == 0 && Keys.nb_keycode > 0)
          {
             if (Keys.mkey._KbMkeyU._KbMkeyS.l_shift !=
                 keyboard_mods._KbMkeyU._KbMkeyS.l_shift ||
                 Keys.mkey._KbMkeyU._KbMkeyS.r_shift !=
                 keyboard_mods._KbMkeyU._KbMkeyS.r_shift)
               {
                  if (Keys.mkey._KbMkeyU._KbMkeyS.r_shift ||
                      Keys.mkey._KbMkeyU._KbMkeyS.l_shift)
                    evas_key_modifier_on(evas, "Shift");
                  else
                    evas_key_modifier_off(evas, "Shift");
               }
             if (Keys.mkey._KbMkeyU._KbMkeyS.l_ctrl !=
                 keyboard_mods._KbMkeyU._KbMkeyS.l_ctrl ||
                 Keys.mkey._KbMkeyU._KbMkeyS.r_ctrl !=
                 keyboard_mods._KbMkeyU._KbMkeyS.r_ctrl)
               {
                  if (Keys.mkey._KbMkeyU._KbMkeyS.r_ctrl ||
                      Keys.mkey._KbMkeyU._KbMkeyS.l_ctrl)
                    evas_key_modifier_on(evas, "Control");
                  else
                    evas_key_modifier_off(evas, "Control");
               }
             if (Keys.mkey._KbMkeyU._KbMkeyS.l_alt !=
                 keyboard_mods._KbMkeyU._KbMkeyS.l_alt ||
                 Keys.mkey._KbMkeyU._KbMkeyS.r_alt !=
                 keyboard_mods._KbMkeyU._KbMkeyS.r_alt)
               {
                  if (Keys.mkey._KbMkeyU._KbMkeyS.r_alt ||
                      Keys.mkey._KbMkeyU._KbMkeyS.l_alt)
                    evas_key_modifier_on(evas, "Alt");
                  else
                    evas_key_modifier_off(evas, "Alt");
               }
             if (Keys.mkey._KbMkeyU._KbMkeyS.l_win !=
                 keyboard_mods._KbMkeyU._KbMkeyS.l_win ||
                 Keys.mkey._KbMkeyU._KbMkeyS.r_win !=
                 keyboard_mods._KbMkeyU._KbMkeyS.r_win)
               {
                  if (Keys.mkey._KbMkeyU._KbMkeyS.r_win ||
                      Keys.mkey._KbMkeyU._KbMkeyS.l_win)
                    evas_key_modifier_on(evas, "Super");
                  else
                    evas_key_modifier_off(evas, "Super");
               }
             keyboard_mods = Keys.mkey;

             if (Keys.led._KbLedU._KbLedS.num_lock !=
                 keyboard_leds._KbLedU._KbLedS.num_lock)
               {
                  if (Keys.led._KbLedU._KbLedS.num_lock)
                    evas_key_lock_on(evas, "Num_Lock");
                  else
                    evas_key_lock_on(evas, "Num_Lock");
               }
             if (Keys.led._KbLedU._KbLedS.caps_lock !=
                 keyboard_leds._KbLedU._KbLedS.caps_lock)
               {
                  if (Keys.led._KbLedU._KbLedS.caps_lock)
                    evas_key_lock_on(evas, "Caps_Lock");
                  else
                    evas_key_lock_on(evas, "Caps_Lock");
               }
             if (Keys.led._KbLedU._KbLedS.scroll_lock !=
                 keyboard_leds._KbLedU._KbLedS.scroll_lock)
               {
                  if (Keys.led._KbLedU._KbLedS.scroll_lock)
                    evas_key_lock_on(evas, "Scroll_Lock");
                  else
                    evas_key_lock_on(evas, "Scroll_Lock");
               }
             keyboard_leds = Keys.led;

             for (i = 0; i < Keys.nb_keycode; i++) {
                  if (Keys.keycode[i] != keyboard_old_key)
                    {
                       u16 key = Keys.keycode[i];
                       char *key_str = NULL;

                       if (key == 0)
                         key = keyboard_old_key;
                       key_str = (char *)&key;

                       printf ("Key %d is %X\n", i, key);
                       key &= ~KB_KEYPAD;
                       if (key & KB_RAWDAT)
                         {
                            key &= 0xFF;
                            if (key == KB_RAWKEY_ESCAPE)
                              key_str = "Escape";
                            else if (key == KB_RAWKEY_ENTER)
                              key_str = "Return";
                            else if (key == KB_RAWKEY_UP_ARROW)
                              key_str = "Up";
                            else if (key == KB_RAWKEY_DOWN_ARROW)
                              key_str = "Down";
                            else if (key == KB_RAWKEY_LEFT_ARROW)
                              key_str = "Left";
                            else if (key == KB_RAWKEY_RIGHT_ARROW)
                              key_str = "Right";
                            else
                              key_str = NULL;
                         }
                       else {
                            if (key == 0x0A)
                              key_str = "Return";
                            else {
                                 key_str[0] = key_str[1];
                                 key_str[1] = 0;
                              }
                         }
                       if (key_str)
                         {
                            if (Keys.keycode[i] != 0)
                              {
                                 printf ("Key %s pressed\n", key_str);
                                 evas_event_feed_key_up(evas, key_str, key_str, NULL, NULL, 0, NULL);
                              }
                            else {
                                 evas_event_feed_key_down(evas, key_str, key_str, NULL, NULL, 0, NULL);
                                 printf ("Key %s released\n", key_str);
                              }
                         }
                       keyboard_old_key = Keys.keycode[0];
                    }
               }
          }
     }
   return;
}

void
engine_psl1ght_shutdown(void)
{
   ioMouseEnd();
   ioKbEnd();
   return;
}

