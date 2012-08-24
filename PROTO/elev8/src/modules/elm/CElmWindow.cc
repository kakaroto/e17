#include "elm.h"
#include "CElmWindow.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmWindow, title);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, conformant);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, autodel);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, borderless);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, shaped);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, alpha);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, override);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, fullscreen);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, maximized);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, iconified);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, withdrawn);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, urgent);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, demand_attention);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, modal);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, sticky);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, quickpanel);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, screen_constrain);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, focus_highlight_enabled);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, keyboard_win);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, rotation);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, priority_major);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, priority_minor);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, quickpanel_zone);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, size_step);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, size_base);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, icon_name);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, role);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, focus_highlight_style);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, aspect);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, indicator_mode);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, indicator_opacity);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, keyboard_mode);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, prop_focus_skip);
GENERATE_RO_PROPERTY_CALLBACKS(CElmWindow, focus);
GENERATE_RO_PROPERTY_CALLBACKS(CElmWindow, screen_position);
GENERATE_RO_PROPERTY_CALLBACKS(CElmWindow, screen_size);
GENERATE_METHOD_CALLBACKS(CElmWindow, socket_listen);
GENERATE_METHOD_CALLBACKS(CElmWindow, activate);
GENERATE_METHOD_CALLBACKS(CElmWindow, lower);
GENERATE_METHOD_CALLBACKS(CElmWindow, raise);
GENERATE_METHOD_CALLBACKS(CElmWindow, center);

GENERATE_TEMPLATE_FULL(CElmObject, CElmWindow,
                  PROPERTY(title),
                  PROPERTY(conformant),
                  PROPERTY(autodel),
                  PROPERTY(borderless),
                  PROPERTY(shaped),
                  PROPERTY(alpha),
                  PROPERTY(override),
                  PROPERTY(fullscreen),
                  PROPERTY(maximized),
                  PROPERTY(iconified),
                  PROPERTY(withdrawn),
                  PROPERTY(urgent),
                  PROPERTY(demand_attention),
                  PROPERTY(modal),
                  PROPERTY(sticky),
                  PROPERTY(quickpanel),
                  PROPERTY(screen_constrain),
                  PROPERTY(focus_highlight_enabled),
                  PROPERTY(keyboard_win),
                  PROPERTY(rotation),
                  PROPERTY(priority_major),
                  PROPERTY(priority_minor),
                  PROPERTY(quickpanel_zone),
                  PROPERTY(size_step),
                  PROPERTY(size_base),
                  PROPERTY(icon_name),
                  PROPERTY(role),
                  PROPERTY(focus_highlight_style),
                  PROPERTY(aspect),
                  PROPERTY(indicator_mode),
                  PROPERTY(indicator_opacity),
                  PROPERTY(keyboard_mode),
                  PROPERTY(prop_focus_skip),
                  PROPERTY_RO(focus),
                  PROPERTY_RO(screen_position),
                  PROPERTY_RO(screen_size),
                  METHOD(socket_listen),
                  METHOD(activate),
                  METHOD(lower),
                  METHOD(raise),
                  METHOD(center));

Evas_Object *CElmWindow::main = NULL;
// Getters and Settters

Handle<Value> CElmWindow::title_get() const
{
   const char *s = elm_win_title_get(eo);
   return s ? String::New(s) : Undefined();
}

void CElmWindow::title_set(Handle<Value> val)
{
   if (val->IsString())
     elm_win_title_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmWindow::conformant_get() const
{
   return Boolean::New(elm_win_conformant_get(eo));
}

void CElmWindow::conformant_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_conformant_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::autodel_get() const
{
   return Boolean::New(elm_win_autodel_get(eo));
}

void CElmWindow::autodel_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_autodel_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::borderless_get() const
{
   return Boolean::New(elm_win_borderless_get(eo));
}

void CElmWindow::borderless_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_borderless_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::shaped_get() const
{
   return Boolean::New(elm_win_shaped_get(eo));
}

void CElmWindow::shaped_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_shaped_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::alpha_get() const
{
   return Boolean::New(elm_win_alpha_get(eo));
}

void CElmWindow::alpha_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_alpha_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::override_get() const
{
   return Boolean::New(elm_win_override_get(eo));
}

void CElmWindow::override_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_override_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::fullscreen_get() const
{
   return Boolean::New(elm_win_fullscreen_get(eo));
}

void CElmWindow::fullscreen_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_fullscreen_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::maximized_get() const
{
   return Boolean::New(elm_win_maximized_get(eo));
}

void CElmWindow::maximized_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_maximized_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::iconified_get() const
{
   return Boolean::New(elm_win_iconified_get(eo));
}

void CElmWindow::iconified_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_iconified_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::withdrawn_get() const
{
   return Boolean::New(elm_win_withdrawn_get(eo));
}

void CElmWindow::withdrawn_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_withdrawn_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::urgent_get() const
{
   return Boolean::New(elm_win_urgent_get(eo));
}

void CElmWindow::urgent_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_urgent_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::demand_attention_get() const
{
   return Boolean::New(elm_win_demand_attention_get(eo));
}

void CElmWindow::demand_attention_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_demand_attention_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::modal_get() const
{
   return Boolean::New(elm_win_modal_get(eo));
}

void CElmWindow::modal_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_modal_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::sticky_get() const
{
   return Boolean::New(elm_win_sticky_get(eo));
}

void CElmWindow::sticky_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_sticky_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::quickpanel_get() const
{
   return Boolean::New(elm_win_quickpanel_get(eo));
}

void CElmWindow::quickpanel_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_quickpanel_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::screen_constrain_get() const
{
   return Boolean::New(elm_win_screen_constrain_get(eo));
}

void CElmWindow::screen_constrain_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_screen_constrain_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::focus_highlight_enabled_get() const
{
   return Boolean::New(elm_win_focus_highlight_enabled_get(eo));
}

void CElmWindow::focus_highlight_enabled_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_focus_highlight_enabled_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::keyboard_win_get() const
{
   return Boolean::New(elm_win_keyboard_win_get(eo));
}

void CElmWindow::keyboard_win_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_keyboard_win_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::rotation_get() const
{
   return Integer::New(elm_win_rotation_get(eo));
}

void CElmWindow::rotation_set(Handle <Value> val)
{
   if (val->IsInt32())
     elm_win_rotation_set(eo, val->ToInt32()->Value());
}

Handle<Value> CElmWindow::priority_major_get() const
{
   return Integer::New(elm_win_quickpanel_priority_major_get(eo));
}

void CElmWindow::priority_major_set(Handle <Value> val)
{
   if (val->IsInt32())
     elm_win_quickpanel_priority_major_set(eo, val->ToInt32()->Value());
}

Handle<Value> CElmWindow::priority_minor_get() const
{
   return Integer::New(elm_win_quickpanel_priority_minor_get(eo));
}

void CElmWindow::priority_minor_set(Handle <Value> val)
{
   if (val->IsInt32())
     elm_win_quickpanel_priority_minor_set(eo, val->ToInt32()->Value());
}

Handle<Value> CElmWindow::quickpanel_zone_get() const
{
   return Integer::New(elm_win_quickpanel_zone_get(eo));
}

void CElmWindow::quickpanel_zone_set(Handle <Value> val)
{
   if (val->IsInt32())
     elm_win_quickpanel_zone_set(eo, val->ToInt32()->Value());
}

Handle<Value> CElmWindow::size_step_get() const
{
   return size_step;
}

void CElmWindow::size_step_set(Handle <Value> val)
{
   if (!val->IsArray())
     return;

   Local<Object> size = val->ToObject();
   elm_win_size_step_set(eo,
        size->Get(0)->ToNumber()->Value(),
        size->Get(1)->ToNumber()->Value());

   size_step.Dispose();
   size_step = Persistent<Value>::New(val);
}

Handle<Value> CElmWindow::size_base_get() const
{
   return size_base;
}

void CElmWindow::size_base_set(Handle <Value> val)
{
   if (!val->IsArray())
     return;

   Local<Object> size = val->ToObject();
   elm_win_size_base_set(eo,
        size->Get(0)->ToNumber()->Value(),
        size->Get(1)->ToNumber()->Value());

   size_base.Dispose();
   size_base = Persistent<Value>::New(val);
}

Handle<Value> CElmWindow::icon_name_get() const
{
   return String::New(elm_win_icon_name_get(eo));
}

void CElmWindow::icon_name_set(Handle <Value> val)
{
   if (val->IsString())
     elm_win_icon_name_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmWindow::role_get() const
{
   return String::New(elm_win_role_get(eo));
}

void CElmWindow::role_set(Handle <Value> val)
{
   if (val->IsString())
     elm_win_role_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmWindow::focus_highlight_style_get() const
{
   const char *style = elm_win_focus_highlight_style_get(eo);

   return style ? String::New(style) : Undefined();
}

void CElmWindow::focus_highlight_style_set(Handle <Value> val)
{
   if (val->IsString())
     elm_win_focus_highlight_style_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmWindow::aspect_get() const
{
   return Integer::New(elm_win_aspect_get(eo));
}

void CElmWindow::aspect_set(Handle <Value> val)
{
   if (val->IsInt32())
     elm_win_aspect_set(eo, val->ToInt32()->Value());
}

Handle<Value> CElmWindow::indicator_mode_get() const
{
   switch (elm_win_indicator_mode_get(eo)) {
     case ELM_WIN_INDICATOR_HIDE:
       return String::NewSymbol("hide");
     case ELM_WIN_INDICATOR_SHOW:
       return String::NewSymbol("show");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmWindow::indicator_mode_set(Handle<Value> value)
{
   String::Utf8Value mode_string(value->ToString());

   if (!strcmp(*mode_string, "hide"))
     elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_HIDE);
   else if (!strcmp(*mode_string, "show"))
     elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_SHOW);
   else
     elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_UNKNOWN);
}

Handle<Value> CElmWindow::indicator_opacity_get() const
{
   switch (elm_win_indicator_opacity_get(eo)) {
     case ELM_WIN_INDICATOR_OPACITY_UNKNOWN:
       return String::NewSymbol("opacityunknown");
     case ELM_WIN_INDICATOR_OPAQUE:
       return String::NewSymbol("opacity");
     case ELM_WIN_INDICATOR_TRANSLUCENT:
       return String::NewSymbol("translucent");
     case ELM_WIN_INDICATOR_TRANSPARENT:
       return String::NewSymbol("transparent");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmWindow::indicator_opacity_set(Handle<Value> value)
{
   String::Utf8Value mode_string(value->ToString());

   if (!strcmp(*mode_string, "opacityunknown"))
     elm_win_indicator_opacity_set(eo, ELM_WIN_INDICATOR_OPACITY_UNKNOWN);
   else if (!strcmp(*mode_string, "opacity"))
     elm_win_indicator_opacity_set(eo, ELM_WIN_INDICATOR_OPAQUE);
   else if (!strcmp(*mode_string, "translucent"))
     elm_win_indicator_opacity_set(eo, ELM_WIN_INDICATOR_TRANSLUCENT);
   else if (!strcmp(*mode_string, "transparent"))
     elm_win_indicator_opacity_set(eo, ELM_WIN_INDICATOR_TRANSPARENT);
}

Handle<Value> CElmWindow::keyboard_mode_get() const
{
   switch (elm_win_keyboard_mode_get(eo)) {
     case ELM_WIN_KEYBOARD_OFF:
       return String::NewSymbol("off");
     case ELM_WIN_KEYBOARD_ON:
       return String::NewSymbol("on");
     case ELM_WIN_KEYBOARD_ALPHA:
       return String::NewSymbol("alpha");
     case ELM_WIN_KEYBOARD_NUMERIC:
       return String::NewSymbol("numeric");
     case ELM_WIN_KEYBOARD_PIN:
       return String::NewSymbol("pin");
     case ELM_WIN_KEYBOARD_PHONE_NUMBER:
       return String::NewSymbol("phonenumber");
     case ELM_WIN_KEYBOARD_HEX:
       return String::NewSymbol("hex");
     case ELM_WIN_KEYBOARD_TERMINAL:
       return String::NewSymbol("terminal");
     case ELM_WIN_KEYBOARD_PASSWORD:
       return String::NewSymbol("password");
     case ELM_WIN_KEYBOARD_IP:
       return String::NewSymbol("ip");
     case ELM_WIN_KEYBOARD_HOST:
       return String::NewSymbol("host");
     case ELM_WIN_KEYBOARD_FILE:
       return String::NewSymbol("file");
     case ELM_WIN_KEYBOARD_URL:
       return String::NewSymbol("url");
     case ELM_WIN_KEYBOARD_KEYPAD:
       return String::NewSymbol("keypad");
     case ELM_WIN_KEYBOARD_J2ME:
       return String::NewSymbol("j2me");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmWindow::keyboard_mode_set(Handle<Value> value)
{
   String::Utf8Value mode_string(value->ToString());

   if (!strcmp(*mode_string, "unknown"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_J2ME);
   else if (!strcmp(*mode_string, "off"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_OFF);
   else if (!strcmp(*mode_string, "on"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_ON);
   else if (!strcmp(*mode_string, "alpha"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_ALPHA);
   else if (!strcmp(*mode_string, "numeric"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_NUMERIC);
   else if (!strcmp(*mode_string, "pin"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_PIN);
   else if (!strcmp(*mode_string, "phonenumber"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_PHONE_NUMBER);
   else if (!strcmp(*mode_string, "hex"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_HEX);
   else if (!strcmp(*mode_string, "terminal"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_TERMINAL);
   else if (!strcmp(*mode_string, "password"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_PASSWORD);
   else if (!strcmp(*mode_string, "ip"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_IP);
   else if (!strcmp(*mode_string, "host"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_HOST);
   else if (!strcmp(*mode_string, "file"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_FILE);
   else if (!strcmp(*mode_string, "url"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_URL);
   else if (!strcmp(*mode_string, "keypad"))
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_KEYPAD);
   else
     elm_win_keyboard_mode_set(eo, ELM_WIN_KEYBOARD_UNKNOWN);
}

Handle<Value> CElmWindow::prop_focus_skip_get() const
{
   return prop_focus;
}

void CElmWindow::prop_focus_skip_set(Handle<Value> val)
{
   if (!val->IsBoolean())
     return;

   elm_win_prop_focus_skip_set(eo, val->BooleanValue());
   prop_focus.Dispose();
   prop_focus = Persistent<Value>::New(val);
}

//Property Read Only
Handle<Value> CElmWindow::focus_get() const
{
   return Boolean::New(elm_win_focus_get(eo));
}

Handle<Value> CElmWindow::screen_position_get() const
{
   int x, y;

   elm_win_screen_position_get(eo, &x, &y);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("x"), Integer::New(x));
   obj->Set(String::NewSymbol("y"), Integer::New(y));

   return obj;
}

Handle<Value> CElmWindow::screen_size_get() const
{
   int x, y, width, height;

   elm_win_screen_size_get(eo, &x, &y, &width, &height);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("x"), Integer::New(x));
   obj->Set(String::NewSymbol("y"), Integer::New(y));
   obj->Set(String::NewSymbol("width"), Integer::New(width));
   obj->Set(String::NewSymbol("height"), Integer::New(height));

   return obj;
}

//Methods
Handle<Value> CElmWindow::socket_listen(const Arguments& args)
{
   if((args[0]->IsString()) && (args[1]->IsInt32()) && (args[1]->IsBoolean()))
     return Boolean::New(elm_win_socket_listen(eo,
                              *String::Utf8Value(args[0]),
                              args[1]->ToInt32()->Value(),
                              args[2]->BooleanValue()));

   return Undefined();
}

Handle<Value> CElmWindow::activate(const Arguments&)
{
   elm_win_activate(eo);
   return Undefined();
}

Handle<Value> CElmWindow::lower(const Arguments&)
{
   elm_win_lower(eo);
   return Undefined();
}

Handle<Value> CElmWindow::raise(const Arguments&)
{
   elm_win_raise(eo);
   return Undefined();
}

Handle<Value> CElmWindow::center(const Arguments& args)
{
   if((args[0]->IsBoolean()) && (args[1]->IsBoolean()))
     elm_win_center(eo, args[0]->BooleanValue(), args[1]->BooleanValue());

   return Undefined();
}


CElmWindow::CElmWindow(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_win_add(parent ? parent->GetEvasObject() : NULL, "main", ELM_WIN_BASIC))
{
   if(!main)
     main = GetEvasObject();

   evas_object_focus_set(eo, 1);
   evas_object_show(eo);
   elm_win_autodel_set(eo, true);
   evas_object_smart_callback_add(main, "delete,request", &quit, this);
}

void CElmWindow::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Window"), GetTemplate()->GetFunction());
}

void CElmWindow::quit(void *data, Evas_Object *, void *)
{
   CElmObject *self = static_cast<CElmObject*>(data);
   Handle<Object> obj = self->GetJSObject();

   Local<Function> callback
      (Function::Cast(*obj->Get(String::NewSymbol("on_delete"))));

   if (callback->IsFunction())
     callback->Call(obj, 0, NULL);

   //TODO: check if his window has parent
   if (main == self->GetEvasObject())
     ecore_main_loop_quit();
}

CElmWindow::~CElmWindow()
{
   size_step.Dispose();
   size_base.Dispose();
   prop_focus.Dispose();
}

}
