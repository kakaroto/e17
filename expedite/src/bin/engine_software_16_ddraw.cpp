#include "main.h"

#include <windowsx.h>
#include <Evas_Engine_Software_16_DDraw.h>


static HWND window;


static int
_directdraw_init (HWND                 window,
                  int                  width,
                  int                  height,
                  LPDIRECTDRAW        *object,
                  LPDIRECTDRAWSURFACE *surface_primary,
                  LPDIRECTDRAWSURFACE *surface_back,
                  LPDIRECTDRAWSURFACE *surface_source,
                  int                 *depth)
{
   DDSURFACEDESC surface_desc;
   DDPIXELFORMAT pixel_format;
   LPDIRECTDRAW  o;
   void         *source;
   HRESULT       res;

   res = DirectDrawCreate (NULL, &o, NULL);
   if (FAILED(res))
     return 0;

   res = o->SetCooperativeLevel (window, DDSCL_NORMAL);
   if (FAILED(res))
     {
        o->Release ();
        return 0;
     }

   memset(&surface_desc, 0, sizeof(surface_desc));
   surface_desc.dwSize = sizeof(surface_desc);
   surface_desc.dwFlags = DDSD_CAPS;
   surface_desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

   res = o->CreateSurface (&surface_desc, surface_primary, NULL);
   if (FAILED(res))
     {
        o->Release ();
        return 0;
     }

   memset (&surface_desc, 0, sizeof(surface_desc));
   surface_desc.dwSize = sizeof(surface_desc);
   surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
   surface_desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
   surface_desc.dwWidth = width;
   surface_desc.dwHeight = height;

   res = o->CreateSurface (&surface_desc, surface_back, NULL);
   if (FAILED(res))
     {
        (*surface_primary)->Release ();
        o->Release ();
        return 0;
     }

   ZeroMemory(&pixel_format, sizeof(pixel_format));
   pixel_format.dwSize = sizeof(pixel_format);
   (*surface_primary)->GetPixelFormat(&pixel_format);

   source = malloc(width * height * 2);
   if (!source)
     {
        (*surface_primary)->Release ();
        o->Release ();
        return 0;
     }

   memset (&surface_desc, 0, sizeof (surface_desc));
   surface_desc.dwSize = sizeof (surface_desc);
   surface_desc.dwFlags =
     DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH |
     DDSD_LPSURFACE | DDSD_PITCH | DDSD_PIXELFORMAT;
   surface_desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
   surface_desc.dwWidth = width;
   surface_desc.dwHeight = height;
   surface_desc.lPitch = 2 * surface_desc.dwWidth;
   surface_desc.lpSurface = source;
   surface_desc.ddpfPixelFormat = pixel_format;

   if (FAILED(o->CreateSurface(&surface_desc, surface_source, NULL)))
     {
        free(source);
        (*surface_primary)->Release ();
        o->Release ();
        return 0;
     }

   *object = o;
   *depth = pixel_format.dwRGBBitCount;

   return 1;
}

static LRESULT CALLBACK
MainWndProc(HWND   hwnd,
            UINT   uMsg,
            WPARAM wParam,
            LPARAM lParam)
{
   switch (uMsg)
     {
     case WM_CREATE:
       return 0;
     case WM_DESTROY:
       PostQuitMessage(0);
       return 0;
     case WM_CLOSE:
       PostQuitMessage(0);
       return 0;
     case WM_WINDOWPOSCHANGED: {
       PAINTSTRUCT ps;
       HDC hdc;

       hdc = BeginPaint (window, &ps);
       evas_damage_rectangle_add(evas,
                                 ps.rcPaint.left, ps.rcPaint.top,
                                 ps.rcPaint.right - ps.rcPaint.left,
                                 ps.rcPaint.bottom - ps.rcPaint.top);
       EndPaint(window, &ps);
       return 0;
     }
     case WM_SIZING:
       {
          PRECT rect = (PRECT)lParam;

          evas_output_viewport_set(evas, 0, 0,
                                   rect->right - rect->left,
                                   rect->bottom - rect->top);
          evas_output_size_set(evas,
                               rect->right - rect->left,
                               rect->bottom - rect->top);
          win_w = rect->right - rect->left;
          win_h = rect->bottom - rect->top;
          return 0;
       }
     case WM_RBUTTONDOWN:
       evas_event_feed_mouse_move(evas, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, NULL);
       evas_event_feed_mouse_down(evas, 3, EVAS_BUTTON_NONE, 0, NULL);
       return 0;
     case WM_LBUTTONDOWN:
       evas_event_feed_mouse_move(evas, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, NULL);
       evas_event_feed_mouse_down(evas, 1, EVAS_BUTTON_NONE, 0, NULL);
       return 0;
     case WM_LBUTTONUP:
       evas_event_feed_mouse_move(evas, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, NULL);
       evas_event_feed_mouse_up(evas, 1, EVAS_BUTTON_NONE, 0, NULL);
       return 0;
     case WM_RBUTTONUP:
       evas_event_feed_mouse_move(evas, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, NULL);
       evas_event_feed_mouse_up(evas, 3, EVAS_BUTTON_NONE, 0, NULL);
       return 0;
     case WM_MOUSEMOVE:
       if (!evas_pointer_inside_get(evas)) evas_event_feed_mouse_in(evas, 0, NULL);
       evas_event_feed_mouse_move(evas, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, NULL);
       return 0;
     case WM_MOUSELEAVE:
       evas_event_feed_mouse_out(evas, 0, NULL);
       return 0;
       /* FIXME : can't find an event when the mouse is entering */
     case WM_KEYDOWN:
     case WM_SYSKEYDOWN: {
        int key;

        key = LOWORD(wParam);

        if ((key == VK_SHIFT) ||
            (key == VK_LSHIFT) ||
            (key == VK_RSHIFT))
          evas_key_modifier_on(evas, "Shift");
        if ((key == VK_CONTROL) ||
            (key == VK_LCONTROL) ||
            (key == VK_RCONTROL))
          evas_key_modifier_on(evas, "Control");
        if ((key == VK_MENU) ||
            (key == VK_LMENU) ||
            (key == VK_RMENU))
          evas_key_modifier_on(evas, "Alt");
        if ((key == VK_LWIN) ||
            (key == VK_RWIN))
          evas_key_modifier_on(evas, "Super");

        if (key == VK_CAPITAL)
          {
             if (evas_key_lock_is_set(evas_key_lock_get(evas), "Caps_Lock"))
               evas_key_lock_off(evas, "Caps_Lock");
             else
               evas_key_lock_on(evas, "Caps_Lock");
          }
        if (key == VK_NUMLOCK)
          {
             if (evas_key_lock_is_set(evas_key_lock_get(evas), "Num_Lock"))
               evas_key_lock_off(evas, "Num_Lock");
             else
               evas_key_lock_on(evas, "Num_Lock");
          }
        if (key == VK_SCROLL)
          {
             if (evas_key_lock_is_set(evas_key_lock_get(evas), "Scroll_Lock"))
               evas_key_lock_off(evas, "Scroll_Lock");
             else
               evas_key_lock_on(evas, "Scroll_Lock");
          }
        if (key == VK_ESCAPE)
          evas_event_feed_key_down(evas, "Escape", "Escape", NULL, NULL, 0, NULL);
        if (key == VK_RETURN)
          evas_event_feed_key_down(evas, "Return", "Return", NULL, NULL, 0, NULL);
        if (key == VK_LEFT)
          evas_event_feed_key_down(evas, "Left", "Left", NULL, NULL, 0, NULL);
        if (key == VK_RIGHT)
          evas_event_feed_key_down(evas, "Right", "Right", NULL, NULL, 0, NULL);
        if (key == 81)
          evas_event_feed_key_down(evas, "Q", "Q", NULL, NULL, 0, NULL);
        if (key == 113)
          evas_event_feed_key_down(evas, "q", "q", NULL, NULL, 0, NULL);
        return 0;
     }
     case WM_KEYUP:
     case WM_SYSKEYUP: {
        int key;

        key = LOWORD(wParam);

        if ((key == VK_SHIFT) ||
            (key == VK_LSHIFT) ||
            (key == VK_RSHIFT))
          evas_key_modifier_off(evas, "Shift");
        if ((key == VK_CONTROL) ||
            (key == VK_LCONTROL) ||
            (key == VK_RCONTROL))
          evas_key_modifier_off(evas, "Control");
        if ((key == VK_MENU) ||
            (key == VK_LMENU) ||
            (key == VK_RMENU))
          evas_key_modifier_off(evas, "Alt");
        if ((key == VK_LWIN) ||
            (key == VK_RWIN))
          evas_key_modifier_off(evas, "Super");
        if (key == VK_ESCAPE)
          evas_event_feed_key_up(evas, "Escape", "Escape", NULL, NULL, 0, NULL);
        if (key == VK_RETURN)
          evas_event_feed_key_up(evas, "Return", "Return", NULL, NULL, 0, NULL);
        if (key == VK_LEFT)
          evas_event_feed_key_up(evas, "Left", "Left", NULL, NULL, 0, NULL);
        if (key == VK_RIGHT)
          evas_event_feed_key_up(evas, "Right", "Right", NULL, NULL, 0, NULL);
        if (key == 81)
          evas_event_feed_key_up(evas, "Q", "Q", NULL, NULL, 0, NULL);
        if (key == 113)
          evas_event_feed_key_up(evas, "q", "q", NULL, NULL, 0, NULL);
        return 0;
     }
     default:
       return DefWindowProc(hwnd, uMsg, wParam, lParam);
     }
}

int
engine_software_16_ddraw_args(int argc, char **argv)
{
   WNDCLASS                            wc;
   RECT                                rect;
   HINSTANCE                           instance;
   LPDIRECTDRAW                        object;
   LPDIRECTDRAWSURFACE                 surface_primary;
   LPDIRECTDRAWSURFACE                 surface_back;
   LPDIRECTDRAWSURFACE                 surface_source;
   Evas_Engine_Info_Software_16_DDraw *einfo;
   DWORD                               style;
   DWORD                               exstyle;
   int                                 depth;
   int                                 i;
   int                                 ok = 0;

   for (i = 1; i < argc; i++)
     {
        if ((!strcmp(argv[i], "-e")) && (i < (argc - 1)))
          {
             i++;
             if (!strcmp(argv[i], "ddraw-16")) ok = 1;
          }
     }
   if (!ok) return 0;

   instance = GetModuleHandle(NULL);
   if (!instance) return 0;

   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = MainWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = instance;
   wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
   wc.hCursor = LoadCursor (NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
   wc.lpszMenuName =  NULL;
   wc.lpszClassName = "Evas_Software_16_DDraw_Test";

   if(!RegisterClass(&wc))
     goto free_library;

   style = WS_OVERLAPPEDWINDOW | WS_SIZEBOX;
   exstyle = 0;

   rect.left = 0;
   rect.top = 0;
   rect.right = win_w;
   rect.bottom = win_h;
   AdjustWindowRectEx(&rect, style, FALSE, exstyle);

   window = CreateWindowEx(exstyle,
                           "Evas_Software_16_DDraw_Test",
                           "Evas_Software_16_DDraw_Test",
                           style,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           rect.right - rect.left, rect.bottom - rect.top,
                           NULL, NULL, instance, NULL);
   if (!window)
     goto unregister_class;

   /* make the window non resizable */
   style = GetWindowLong(window, GWL_STYLE);
   style &= ~WS_THICKFRAME;
   if (!SetWindowLong(window, GWL_STYLE, style))
     goto unregister_class;

   if (!_directdraw_init(window, win_w, win_h,
                         &object,
                         &surface_primary,
                         &surface_back,
                         &surface_source,
                         &depth))
        goto destroy_window;

   evas_output_method_set(evas, evas_render_method_lookup("software_16_ddraw"));
   einfo = (Evas_Engine_Info_Software_16_DDraw *)evas_engine_info_get(evas);
   if (!einfo)
     {
        fprintf(stderr, "Evas does not support the 16 bits Software DirectDraw Engine\n");
        /* should shutdown ddraw  */
        goto destroy_window;
     }

   einfo->info.window = window;
   einfo->info.object = object;
   einfo->info.surface_primary = surface_primary;
   einfo->info.surface_back = surface_back;
   einfo->info.surface_source = surface_source;
   einfo->info.depth = depth;
   einfo->info.rotation = 0;
   if (!evas_engine_info_set(evas, (Evas_Engine_Info *) einfo))
     {
	printf("Evas can not setup the informations of the 16 bits Software DirectDraw Engine\n");
        goto destroy_window;
     }

   /* the second parameter is ignored, as it's the first call of ShowWindow */
   ShowWindow(window, SW_SHOWDEFAULT);
   UpdateWindow(window);

   return 1;

 destroy_window:
   DestroyWindow(window);
 unregister_class:
   UnregisterClass("Evas_Software_16_DDraw_Test", instance);
 free_library:
   FreeLibrary(instance);

   return 0;
}

void
engine_software_16_ddraw_loop(void)
{
   MSG msg;
   int res;

 again:
   if (!PeekMessage (&msg, window, 0, 0, PM_NOREMOVE))
     return;

   res = GetMessage (&msg, NULL, 0, 0);
   TranslateMessage (&msg);
   DispatchMessage (&msg);

   goto again;
}
