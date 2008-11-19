#include "main.h"

#include <windows.h>
#include <aygshell.h>
#include <Evas_Engine_Software_16_WinCE.h>


static HINSTANCE instance = NULL;
static HWND window = NULL;
static int  backend = 0;

typedef int (*suspend) (int backend);
typedef int (*resume) (int backend);

static suspend _suspend = NULL;
static resume _resume = NULL;


typedef BOOL (__stdcall *UnregisterFunc1Proc)(UINT, UINT);

static int
_wince_hardware_keys_register(HWND window)
{
   HINSTANCE           core_dll;
   UnregisterFunc1Proc unregister_fct;
   int                 i;

   core_dll = LoadLibrary(L"coredll.dll");
   if (!core_dll)
     return 0;

   unregister_fct = (UnregisterFunc1Proc)GetProcAddress(core_dll, L"UnregisterFunc1");
   if (!unregister_fct)
     {
        FreeLibrary(core_dll);
        return 0;
     }

   for (i = 0xc1; i <= 0xcf; i++)
     {
        unregister_fct(MOD_WIN, i);
        RegisterHotKey(window, i, MOD_WIN, i);
     }

   FreeLibrary(core_dll);

   return 1;
}

static void
_wince_key_down(WPARAM wParam)
{
   int key;

   key = LOWORD(wParam);

   if ((key == VK_SHIFT) ||
       (key == VK_LSHIFT) ||
       (key == VK_RSHIFT))
     evas_key_modifier_on(evas, "Shift");

   if (key == VK_CAPITAL)
     {
        if (evas_key_lock_is_set(evas_key_lock_get(evas), "Caps_Lock"))
          evas_key_lock_off(evas, "Caps_Lock");
        else
          evas_key_lock_on(evas, "Caps_Lock");
     }
   if ((key == VK_RETURN) || (key == VK_APP1))
     evas_event_feed_key_down(evas, "Return", "Return", NULL, NULL, 0, NULL);
   if (key == VK_LEFT)
     evas_event_feed_key_down(evas, "Left", "Left", NULL, NULL, 0, NULL);
   if (key == VK_RIGHT)
     evas_event_feed_key_down(evas, "Right", "Right", NULL, NULL, 0, NULL);
   if ((key == 81) || (key == VK_APP2))
     evas_event_feed_key_down(evas, "Q", "Q", NULL, NULL, 0, NULL);
   if ((key == 113) || (key == VK_APP3))
     evas_event_feed_key_down(evas, "q", "q", NULL, NULL, 0, NULL);
}

static void
_wince_key_up(WPARAM wParam)
{
   int key;

   key = LOWORD(wParam);

   if ((key == VK_SHIFT) ||
       (key == VK_LSHIFT) ||
       (key == VK_RSHIFT))
     evas_key_modifier_off(evas, "Shift");
   if ((key == VK_RETURN) || (key == VK_APP1))
     evas_event_feed_key_up(evas, "Return", "Return", NULL, NULL, 0, NULL);
   if (key == VK_LEFT)
     evas_event_feed_key_up(evas, "Left", "Left", NULL, NULL, 0, NULL);
   if (key == VK_RIGHT)
     evas_event_feed_key_up(evas, "Right", "Right", NULL, NULL, 0, NULL);
   if ((key == 81) || (key == VK_APP2))
     evas_event_feed_key_up(evas, "Q", "Q", NULL, NULL, 0, NULL);
   if ((key == 113) || (key == VK_APP3))
     evas_event_feed_key_up(evas, "q", "q", NULL, NULL, 0, NULL);
}

static LRESULT CALLBACK
MainWndProc(HWND   hwnd,
            UINT   uMsg,
            WPARAM wParam,
            LPARAM lParam)
{
   switch (uMsg)
     {
     case WM_DESTROY:
       PostQuitMessage(0);
       return 0;
     case WM_CLOSE:
       PostQuitMessage(0);
       return 0;
     case WM_PAINT:
       ValidateRect(hwnd, NULL);
       return 0;
     case WM_HOTKEY:
       _wince_key_down(wParam);
       return 0;
     case WM_KEYDOWN:
       _wince_key_down(wParam);
       return 0;
     case WM_KEYUP:
       _wince_key_up(wParam);
       return 0;
     case WM_KILLFOCUS:
       if (_suspend)
         _suspend (backend);
       return 0;
     case WM_SETFOCUS:
       if (_resume)
         _resume (backend);
       return 0;
     default:
       return DefWindowProc(hwnd, uMsg, wParam, lParam);
     }
}

int
engine_software_16_wince_args(int argc, char **argv)
{
   WNDCLASS                            wc;
   RECT                                rect;
   Evas_Engine_Info_Software_16_WinCE *einfo;
   int                                 width;
   int                                 height;
   int                                 stride_x;
   int                                 stride_y;
   int                                 bpp;
   int                                 format;
   void                               *buffer;
   int                                 ok = 0;
   int                                 i;

   for (i = 1; i < argc; i++)
     {
        if ((!strcmp(argv[i], "-e")) && (i < (argc - 1)))
          {
             i++;
             if (!strcmp(argv[i], "wince")) ok = 1;
             if (!strcmp(argv[i], "wince-fb")) { ok = 1; backend = 1; }
             if (!strcmp(argv[i], "wince-gapi")) { ok = 1; backend = 2; }
             if (!strcmp(argv[i], "wince-ddraw")) { ok = 1; backend = 3; }
             if (!strcmp(argv[i], "wince-gdi")) { ok = 1; backend = 4; }
          }
     }
   if (!ok) return 0;

   instance = GetModuleHandle(NULL);
   if (!instance) return 0;

   memset (&wc, 0, sizeof (wc));
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = MainWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = instance;
   wc.hIcon = NULL;
   wc.hCursor = LoadCursor (NULL, IDC_ARROW);
   wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
   wc.lpszMenuName =  NULL;
   wc.lpszClassName = L"Evas_Software_16_WinCE_Test";

   if(!RegisterClass(&wc))
     {
        FreeLibrary(instance);
        return 0;
     }

   SetRect(&rect, 0, 0,
           GetSystemMetrics(SM_CXSCREEN),
           GetSystemMetrics(SM_CYSCREEN));

   window = CreateWindowEx(WS_EX_TOPMOST,
                           L"Evas_Software_16_WinCE_Test",
                           L"Evas_Software_16_WinCE_Test",
                           WS_VISIBLE | WS_POPUP,
                           rect.left, rect.top,
                           rect.right - rect.left,
                           rect.bottom - rect.top,
                           NULL, NULL, instance, NULL);
   if (!window)
     {
        UnregisterClass(L"Evas_Software_16_WinCE_Test", instance);
        FreeLibrary(instance);
        return 0;
     }

   SHFullScreen(window,
                SHFS_HIDETASKBAR | SHFS_HIDESTARTICON | SHFS_HIDESIPBUTTON);

   if (!_wince_hardware_keys_register(window))
     {
        DestroyWindow(window);
        UnregisterClass(L"Evas_Software_16_WinCE_Test", instance);
        FreeLibrary(instance);
        return 0;
     }

   evas_output_method_set(evas, evas_render_method_lookup("software_16_wince"));

   einfo = (Evas_Engine_Info_Software_16_WinCE *)evas_engine_info_get(evas);
   if (!einfo)
     {
        printf("Evas does not support the 16bit Software WinCE Engine\n");
        DestroyWindow(window);
        UnregisterClass(L"Evas_Software_16_WinCE_Test", instance);
        FreeLibrary(instance);
        return 0;
     }

   einfo->info.window = window;
   einfo->info.width = win_w;
   einfo->info.height = win_h;
   einfo->info.backend = backend;
   einfo->info.rotation = 0;
   evas_engine_info_set(evas, (Evas_Engine_Info *)einfo);

   _suspend = einfo->func.suspend;
   _resume = einfo->func.resume;

   /* the second parameter is ignored, as it's the first call of ShowWindow */
   ShowWindow(window, SW_SHOWDEFAULT);
   UpdateWindow(window);

   return 1;
}

void
engine_software_16_wince_loop(void)
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
