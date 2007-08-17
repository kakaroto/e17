#include "main.h"


#ifndef HAVE_DIRECT3D


extern "C" {

int
engine_direct3d_args(int argc, char **argv)
{
  fprintf(stderr, "ERROR: No Direct3D Engine built\n");
  return 0;
}

void
engine_direct3d_loop(void)
{
}

}

#else
# include <windowsx.h>
# include <Evas_Engine_Direct3D.h>


static HWND window;


extern "C" {

static int
_direct3d_init (HWND                window,
                int                 width,
                int                 height,
                LPDIRECT3D9        *object,
                LPDIRECT3DDEVICE9  *device,
                LPD3DXSPRITE       *sprite,
                LPDIRECT3DTEXTURE9 *texture,
                int                *depth)
{
  D3DPRESENT_PARAMETERS pp;
  D3DDISPLAYMODE        dm;
  D3DSURFACE_DESC       sd;
  D3DCAPS9              caps;
  DWORD                 flag;

  *object = Direct3DCreate9 (D3D_SDK_VERSION);
  if (!*object)
    goto no_object;

  if (FAILED ((*object)->GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &dm)))
    goto no_device;

  if (FAILED ((*object)->GetDeviceCaps (D3DADAPTER_DEFAULT,
                                        D3DDEVTYPE_HAL,
                                        &caps)))
    goto no_device;

  flag = (caps.VertexProcessingCaps != 0)
    ? D3DCREATE_HARDWARE_VERTEXPROCESSING
    : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

  ZeroMemory(&pp, sizeof(pp));
  pp.BackBufferWidth = width;
  pp.BackBufferHeight = height;
  pp.BackBufferFormat = dm.Format;
  pp.BackBufferCount = 1;
  pp.MultiSampleType = D3DMULTISAMPLE_NONE;
  pp.MultiSampleQuality = 0;
  pp.SwapEffect = D3DSWAPEFFECT_FLIP;
  pp.hDeviceWindow = window;
  pp.Windowed  = TRUE;
  pp.EnableAutoDepthStencil = FALSE;
  pp.FullScreen_RefreshRateInHz = 0;
  pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

  if (FAILED((*object)->CreateDevice (D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      window,
                                      flag,
                                      &pp,
                                      device)))
    goto no_device;

  if (FAILED (D3DXCreateSprite (*device, sprite)))
    goto no_sprite;

  if (FAILED ((*device)->CreateTexture (width, height, 1,
                                        D3DUSAGE_DYNAMIC,
                                        dm.Format,
                                        D3DPOOL_DEFAULT,
                                        texture, NULL)))
    goto no_texture;

  if (FAILED ((*texture)->GetLevelDesc (0, &sd)))
    goto no_level_desc;

  switch (sd.Format) {
  case D3DFMT_A8R8G8B8:
  case D3DFMT_X8R8G8B8:
    *depth = 32;
    break;
  case D3DFMT_R5G6B5:
    *depth = 16;
    break;
  default:
    goto no_level_desc;
  }

  return 1;

 no_level_desc:
  (*texture)->Release ();
 no_texture:
  (*sprite)->Release ();
 no_sprite:
  (*device)->Release ();
 no_device:
  (*object)->Release ();
 no_object:
  return 0;
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
     case WM_PAINT: {
       PAINTSTRUCT ps;
       HDC hdc;

       hdc = BeginPaint (window, &ps);
       EndPaint(window, &ps);
       evas_damage_rectangle_add(evas,
                                 ps.rcPaint.left, ps.rcPaint.top,
                                 ps.rcPaint.right - ps.rcPaint.left,
                                 ps.rcPaint.bottom - ps.rcPaint.top);
       return 0;
     }
     case WM_SIZING:
       {
          PRECT rect = (PRECT)lParam;

          evas_output_size_set(evas,
                               rect->right - rect->left,
                               rect->bottom - rect->top);
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
engine_direct3d_args(int argc, char **argv)
{
   WNDCLASS                   wc;
   RECT                       rect;
   HINSTANCE                  hinstance;
   MSG                        msg;
   LPDIRECT3D9                object;
   LPDIRECT3DDEVICE9          device;
   LPD3DXSPRITE               sprite;
   LPDIRECT3DTEXTURE9         texture;
   Evas_Engine_Info_Direct3D *einfo;
   int                        depth;
   int                        i;
   int                        ok = 0;

   for (i = 1; i < argc; i++)
     {
        if ((!strcmp(argv[i], "-e")) && (i < (argc - 1)))
          {
             i++;
             if (!strcmp(argv[i], "direct3d")) ok = 1;
          }
     }
   if (!ok) return 0;

   hinstance = GetModuleHandle(0);

   wc.style = 0;
   wc.lpfnWndProc = MainWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hinstance;
   wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
   wc.hCursor = LoadCursor (NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
   wc.lpszMenuName =  NULL;
   wc.lpszClassName = "Evas_Direct3D_Test";

   if(!RegisterClass(&wc)) return EXIT_FAILURE;

   rect.left = 0;
   rect.top = 0;
   rect.right = win_w;
   rect.bottom = win_h;
   AdjustWindowRect (&rect, WS_OVERLAPPEDWINDOW | WS_SIZEBOX, FALSE);

   window = CreateWindowEx(0,
                           "Evas_Direct3D_Test",
                           "Evas_Direct3D_Test",
                           WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           rect.right - rect.left, rect.bottom - rect.top,
                           NULL, NULL, hinstance, NULL);
   if (!window) return EXIT_FAILURE;

   if (!_direct3d_init(window, win_w, win_h,
                       &object,
                       &device,
                       &sprite,
                       &texture,
                       &depth))
     return 0;

   evas_output_method_set(evas, evas_render_method_lookup("direct3d"));
   einfo = (Evas_Engine_Info_Direct3D *)evas_engine_info_get(evas);
   if (!einfo)
     {
        printf("Evas does not support the Direct3D Engine\n");
        return 0;
     }

   einfo->info.window = window;
   einfo->info.object = object;
   einfo->info.device = device;
   einfo->info.sprite = sprite;
   einfo->info.texture = texture;
   einfo->info.depth = depth;
   einfo->info.rotation = 0;
   evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);

   /* the second parameter is ignored, as it's the first call of ShowWindow */
   ShowWindow(window, SW_SHOWDEFAULT);
   UpdateWindow(window);

   return 1;
}

void
engine_direct3d_loop(void)
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


}

#endif /* HAVE_DIRECT3D */
