#include "main.h"

#include <windows.h>
#include <aygshell.h>
#include <Evas_Engine_Software_16_WinCE.h>


typedef struct
{
  short vkUp;        // key for up
  POINT ptUp;        // x,y position of key/button.  Not on screen but in screen coordinates.
  short vkDown;
  POINT ptDown;
  short vkLeft;
  POINT ptLeft;
  short vkRight;
  POINT ptRight;
  short vkA;
  POINT ptA;
  short vkB;
  POINT ptB;
  short vkC;
  POINT ptC;
  short vkStart;
  POINT ptStart;
} _GAPI_Key_List;


static HWND window;
static int  backend = 0;
static _GAPI_Key_List *key_list = NULL;

typedef int (*suspend) (int backend);
typedef int (*resume) (int backend);

static suspend _suspend = NULL;
static resume _resume = NULL;

void
_wince_fb_key_down(WPARAM wParam)
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
}

void
_wince_fb_key_up(WPARAM wParam)
{
   int key;

   key = LOWORD(wParam);

   if ((key == VK_SHIFT) ||
       (key == VK_LSHIFT) ||
       (key == VK_RSHIFT))
     evas_key_modifier_off(evas, "Shift");
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
}

void
_wince_gapi_key(WPARAM wParam)
{
  if (wParam == (unsigned int)key_list->vkLeft)
    evas_event_feed_key_down(evas, "Left", "Left", NULL, NULL, 0, NULL);
  if (wParam == (unsigned int)key_list->vkRight)
     evas_event_feed_key_down(evas, "Right", "Right", NULL, NULL, 0, NULL);
  if (wParam == (unsigned int)key_list->vkA)
     evas_event_feed_key_down(evas, "Return", "Return", NULL, NULL, 0, NULL);
  if (wParam == (unsigned int)key_list->vkB)
     evas_event_feed_key_down(evas, "Q", "Q", NULL, NULL, 0, NULL);
  if (wParam == (unsigned int)key_list->vkC)
     evas_event_feed_key_down(evas, "q", "q", NULL, NULL, 0, NULL);
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
     case WM_KEYDOWN:
     case WM_SYSKEYDOWN: {
        if (backend == 1)
          _wince_fb_key_down(wParam);
        if (backend == 2)
          _wince_gapi_key(wParam);

        return 0;
     }
     case WM_KEYUP:
     case WM_SYSKEYUP: {
        if (backend == 1)
          _wince_fb_key_up(wParam);
        if (backend == 2)
          _wince_gapi_key(wParam);

        return 0;
     }
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
   HINSTANCE                           hinstance;
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
          }
     }
   if (!ok) return 0;

   hinstance = GetModuleHandle(NULL);

   memset (&wc, 0, sizeof (wc));
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = MainWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hinstance;
   wc.hIcon = NULL;
   wc.hCursor = LoadCursor (NULL, IDC_ARROW);
   wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
   wc.lpszMenuName =  NULL;
   wc.lpszClassName = L"Evas_Software_16_WinCE_Test";

   if(!RegisterClass(&wc)) return EXIT_FAILURE;

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
                           NULL, NULL, hinstance, NULL);
   if (!window) return EXIT_FAILURE;

   SHFullScreen(window,
                SHFS_HIDETASKBAR | SHFS_HIDESTARTICON | SHFS_HIDESIPBUTTON);

   evas_output_method_set(evas, evas_render_method_lookup("software_16_wince"));
   einfo = (Evas_Engine_Info_Software_16_WinCE *)evas_engine_info_get(evas);
   if (!einfo)
     {
        printf("Evas does not support the 16bit Software WinCE Engine\n");
        return 0;
     }

   einfo->info.window = window;
   einfo->info.backend = backend;
   einfo->info.rotation = 0;
   evas_engine_info_set(evas, (Evas_Engine_Info *)einfo);

   _suspend = einfo->func.suspend;
   _resume = einfo->func.resume;
   key_list = einfo->func.default_keys(backend);

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
