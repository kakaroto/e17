#include <windows.h>
#include <windowsx.h>

#include "evas_test_main.h"

#include "Evas.h"
#include "Evas_Engine_Software_DDraw.h"


static int
_directdraw_init (HWND                 window,
		  LPDIRECTDRAW        *object,
		  LPDIRECTDRAWSURFACE *surface_primary,
		  LPDIRECTDRAWSURFACE *surface_back,
		  int                 *depth,
		  int width,
		  int height)
{
   DDSURFACEDESC2      surface_desc;
   DDPIXELFORMAT       pixel_format;
   LPDIRECTDRAWCLIPPER clipper;
   LPDIRECTDRAW        o;
   DDSURFACEDESC2     *sd;
   HRESULT             res;

   res = DirectDrawCreateEx (NULL, (void **)&o, &IID_IDirectDraw7, NULL);
   if (FAILED(res))
     return 0;

   res = IDirectDraw7_SetCooperativeLevel (o, window, DDSCL_NORMAL);
   if (FAILED(res))
     {
	IDirectDraw7_Release (o);
	return 0;
     }

   res = IDirectDraw7_CreateClipper (o, 0, &clipper, NULL);
   if (FAILED(res))
     {
	IDirectDraw7_Release (o);
	return 0;
     }

   res = IDirectDrawClipper_SetHWnd (clipper, 0, window);
   if (FAILED(res))
     {
	IDirectDrawClipper_Release (clipper);
	IDirectDraw7_Release (o);
	return 0;
     }

   memset(&surface_desc, 0, sizeof(surface_desc));
   surface_desc.dwSize = sizeof(surface_desc);
   surface_desc.dwFlags = DDSD_CAPS;
   surface_desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

   sd=&surface_desc;
   res = IDirectDraw7_CreateSurface (o, &surface_desc,
				     surface_primary, NULL);
   if (FAILED(res))
     {
	IDirectDrawClipper_Release (clipper);
	IDirectDraw7_Release (o);
	return 0;
     }

   res = IDirectDrawSurface7_SetClipper (*surface_primary, clipper);
   if (FAILED(res))
     {
	IDirectDrawClipper_Release (clipper);
	IDirectDrawSurface7_Release (*surface_primary);
	IDirectDraw7_Release (o);
	return 0;
     }

   memset (&surface_desc, 0, sizeof(surface_desc));
   surface_desc.dwSize = sizeof(surface_desc);
   surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
   surface_desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
   surface_desc.dwWidth = width;
   surface_desc.dwHeight = height;

   sd=&surface_desc;
   res = IDirectDraw7_CreateSurface (o, (DDSURFACEDESC *)sd,
				     surface_back, NULL);
   if (FAILED(res))
     {
	IDirectDrawClipper_Release (clipper);
	IDirectDrawSurface7_Release (*surface_primary);
	IDirectDraw7_Release (o);
	return 0;
     }

   ZeroMemory(&pixel_format, sizeof(pixel_format));
   pixel_format.dwSize = sizeof(pixel_format);
   IDirectDrawSurface7_GetPixelFormat(*surface_primary, &pixel_format);

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
     case WM_RBUTTONDOWN:
       PostQuitMessage(0);
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
       evas_event_feed_mouse_move(evas, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, NULL);
       return 0;
     default:
       return DefWindowProc(hwnd, uMsg, wParam, lParam);
     }
}

int
main(int argc, char **argv)
{
   WNDCLASS            wc;
   RECT                rect;
   HINSTANCE           hinstance;
   MSG                 msg;
   HWND                window;
   LPDIRECTDRAW        object;
   LPDIRECTDRAWSURFACE surface_primary;
   LPDIRECTDRAWSURFACE surface_back;
   int                 depth;
   int                 running;
   int                 pause_me = 0;

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
   wc.lpszClassName = "Evas_Software_DDraw_Test";

   if(!RegisterClass(&wc)) return EXIT_FAILURE;

   rect.left = 0;
   rect.top = 0;
   rect.right = win_w;
   rect.bottom = win_h;
   AdjustWindowRect (&rect, WS_OVERLAPPEDWINDOW | WS_SIZEBOX, FALSE);

   window = CreateWindowEx(0,
			   "Evas_Software_DDraw_Test",
			   "Evas_Software_DDraw_Test",
			   WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
			   CW_USEDEFAULT, CW_USEDEFAULT,
                           rect.right - rect.left, rect.bottom - rect.top,
			   NULL, NULL, hinstance, NULL);
   if (!window) return EXIT_FAILURE;

   if (!_directdraw_init(window,
			 &object,
			 &surface_primary,
			 &surface_back,
			 &depth, win_w, win_h))
     return EXIT_FAILURE;

   ShowWindow(window, SW_SHOWDEFAULT);
   UpdateWindow(window);

   /* test evas_free....  :) */
   evas_init();
   evas = evas_new();
   evas_output_method_set(evas, evas_render_method_lookup("software_ddraw"));
   evas_output_size_set(evas, win_w, win_h);
   evas_output_viewport_set(evas, 0, 0, win_w, win_h);
   {
      Evas_Engine_Info_Software_DDraw *einfo;

      einfo = (Evas_Engine_Info_Software_DDraw *)evas_engine_info_get(evas);

      /* the following is specific to the engine */
      einfo->info.window = window;
      einfo->info.object = object;
      einfo->info.surface_primary = surface_primary;
      einfo->info.surface_back = surface_back;
      einfo->info.depth = depth;
      einfo->info.rotation = 0;
      evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);
   }
   setup();
   printf("################ evas free\n");
   setdown();
   evas_free(evas);
   printf("evas freed. DONE\n");

   evas = evas_new();
   evas_output_method_set(evas, evas_render_method_lookup("software_ddraw"));
   evas_output_size_set(evas, win_w, win_h);
   evas_output_viewport_set(evas, 0, 0, win_w, win_h);
   {
      Evas_Engine_Info_Software_DDraw *einfo;

      einfo = (Evas_Engine_Info_Software_DDraw *) evas_engine_info_get(evas);

      /* the following is specific to the engine */
      einfo->info.window = window;
      einfo->info.object = object;
      einfo->info.surface_primary = surface_primary;
      einfo->info.surface_back = surface_back;
      einfo->info.depth = depth;
      einfo->info.rotation = 0;
      evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);
   }
   setup();
   printf("################ evas free\n");
   setdown();
   evas_free(evas);
   printf("evas freed. DONE\n");

   evas = evas_new();
   evas_output_method_set(evas, evas_render_method_lookup("software_ddraw"));
   evas_output_size_set(evas, win_w, win_h);
   evas_output_viewport_set(evas, 0, 0, win_w, win_h);
   {
      Evas_Engine_Info_Software_DDraw *einfo;

      einfo = (Evas_Engine_Info_Software_DDraw *) evas_engine_info_get(evas);

      /* the following is specific to the engine */
      einfo->info.window = window;
      einfo->info.object = object;
      einfo->info.surface_primary = surface_primary;
      einfo->info.surface_back = surface_back;
      einfo->info.depth = depth;
      einfo->info.rotation = 0;
      evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);
   }
   setup();
   printf("################ evas free\n");
   setdown();
   evas_free(evas);
   printf("evas freed. DONE\n");

   evas = evas_new();
   evas_output_method_set(evas, evas_render_method_lookup("software_ddraw"));
   evas_output_size_set(evas, win_w, win_h);
   evas_output_viewport_set(evas, 0, 0, win_w, win_h);
   {
      Evas_Engine_Info_Software_DDraw *einfo;

      einfo = (Evas_Engine_Info_Software_DDraw *) evas_engine_info_get(evas);

      /* the following is specific to the engine */
      einfo->info.window = window;
      einfo->info.object = object;
      einfo->info.surface_primary = surface_primary;
      einfo->info.surface_back = surface_back;
      einfo->info.depth = depth;
      einfo->info.rotation = 0;
      evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);
   }
   setup();
   printf("################ evas free\n");
   setdown();
   evas_free(evas);
   printf("evas freed. DONE\n");

   evas = evas_new();
   evas_output_method_set(evas, evas_render_method_lookup("software_ddraw"));
   evas_output_size_set(evas, win_w, win_h);
   evas_output_viewport_set(evas, 0, 0, win_w, win_h);
   {
      Evas_Engine_Info_Software_DDraw *einfo;

      einfo = (Evas_Engine_Info_Software_DDraw *) evas_engine_info_get(evas);

      /* the following is specific to the engine */
      einfo->info.window = window;
      einfo->info.object = object;
      einfo->info.surface_primary = surface_primary;
      einfo->info.surface_back = surface_back;
      einfo->info.depth = depth;
      einfo->info.rotation = 0;
      evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);
   }
   setup();
   orig_start_time = start_time = get_time();

   running = 1;
   while (running)
     {
        while (PeekMessage (&msg, window, 0, 0, PM_NOREMOVE))
	  {
	     int res;

	     res = GetMessage (&msg, NULL, 0, 0);
	     TranslateMessage (&msg);
	     DispatchMessage (&msg);
	     if (res == 0)
	       running = 0;
	  }
	if (!(pause_me == 1))
	  {
	     loop();
	     evas_render(evas);
	  }
     }

 exit:
   setdown();
   evas_free(evas);
   if (surface_primary)
     IDirectDrawSurface7_Release (surface_primary);
   if (object)
      IDirectDraw7_Release (object);
   evas_shutdown();

   return EXIT_SUCCESS;
}
