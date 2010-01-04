#include <Elementary.h>
#include <Ecore_Config.h>
#include <locale.h>
#include "../config.h"
#define _(string) gettext(string)

typedef struct _Config_Values Config_Values;

struct _Config_Values
{
  int autologin_mode;
  char *autologin_user;
  char *greeting_after;
  char *greeting_before;
  int greeting_usedefault;
  int presel_mode;
  int halt;
  int reboot;
  int remember;
  int remember_n;
  int auth;
  char *date_format;
  char *theme;
  char *time_format;
  int attempts;
  char *xserver;
};


#ifndef ELM_LIB_QUICKLAUNCH
static Config_Values vals;
static const char *file;
static Evas_Object *ck_defgreet, *ck_autolog, *ck_presel, *ck_remember, *ck_reboot, *ck_shutdown, *ck_pam, *en_autolog, *en_greetb, *en_greeta, *en_date, *en_time, *en_theme, *en_xserver, *sl_attempts, *sl_remember;

// for compatibilty, does not break new elementary but helps with old
static const char *
strip_br(const char *string)
{
  char buf[128];
  snprintf(buf, sizeof(buf), "%s<br>", eina_stringshare_add_length(string, strlen(string) - 4));
  if (!strcmp(string,buf))
    return eina_stringshare_add_length(string, strlen(string) - 4);
  else
    return string;
}

static void
close_app(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static void
get_settings(void)
{
  ecore_config_int_default("/entrance/autologin/mode", 0);
  ecore_config_string_default("/entrance/autologin/user", "set_user_here");
  ecore_config_string_default("/entrance/greeting/after", "");
  ecore_config_string_default("/entrance/greeting/before", "Welcome to");
  ecore_config_int_default("/entrance/greeting/use_default", 1);
  ecore_config_int_default("/entrance/presel/mode", 1);
  ecore_config_int_default("/entrance/system/halt", 1);
  ecore_config_int_default("/entrance/system/reboot", 1);
  ecore_config_int_default("/entrance/user/remember", 1);
  ecore_config_int_default("/entrance/user/remember_n", 5);
  ecore_config_int_default("/entrance/auth", 1);
  ecore_config_string_default("/entrance/date_format", "%A %B %e, %Y");
  ecore_config_string_default("/entrance/theme", "deafult.edj");
  ecore_config_string_default("/entrance/time_format", "%l:%M:%S %p");
  ecore_config_int_default("/entranced/attempts", 5);
  ecore_config_string_default("/entranced/xserver", "/usr/bin/X -quiet -br -nolisten tcp vt7");
  vals.autologin_mode = ecore_config_int_get("/entrance/autologin/mode");
  vals.autologin_user = ecore_config_string_get("/entrance/autologin/user");
  vals.greeting_after = ecore_config_string_get("/entrance/greeting/after");
  vals.greeting_before = ecore_config_string_get("/entrance/greeting/before");
  vals.greeting_usedefault = ecore_config_int_get("/entrance/greeting/use_default");
  vals.presel_mode = ecore_config_int_get("/entrance/presel/mode");
  vals.halt = ecore_config_int_get("/entrance/system/halt");
  vals.reboot = ecore_config_int_get("/entrance/system/reboot");
  vals.remember = ecore_config_int_get("/entrance/user/remember");
  vals.remember_n = ecore_config_int_get("/entrance/user/remember_n");
  vals.auth = ecore_config_int_get("/entrance/auth");
  vals.date_format = ecore_config_string_get("/entrance/date_format");
  vals.theme = ecore_config_string_get("/entrance/theme");
  vals.time_format = ecore_config_string_get("/entrance/time_format");
  vals.attempts = ecore_config_int_get("/entranced/attempts");
  vals.xserver = ecore_config_string_get("/entranced/xserver");
}

static void
save_settings(void *data, Evas_Object *obj, void *event_info)
{
  char buf[PATH_MAX];
  vals.autologin_mode = elm_check_state_get(ck_autolog);
  vals.autologin_user = strip_br(elm_entry_entry_get(en_autolog));
  vals.greeting_usedefault = elm_check_state_get(ck_defgreet);
  vals.presel_mode = elm_check_state_get(ck_presel);
  vals.halt = elm_check_state_get(ck_shutdown);
  vals.reboot = elm_check_state_get(ck_reboot);
  vals.remember = elm_check_state_get(ck_remember);
  vals.auth = elm_check_state_get(ck_pam);
  vals.remember_n = elm_slider_value_get(sl_remember);
  vals.attempts = elm_slider_value_get(sl_attempts);
  vals.greeting_after = strip_br(elm_entry_entry_get(en_greeta));
  vals.greeting_before = strip_br(elm_entry_entry_get(en_greetb));
  vals.date_format = strip_br(elm_entry_entry_get(en_date));
  vals.time_format = strip_br(elm_entry_entry_get(en_time));
  vals.theme = strip_br(elm_entry_entry_get(en_theme));
  vals.xserver = strip_br(elm_entry_entry_get(en_xserver));
  ecore_config_int_set("/entrance/autologin/mode", vals.autologin_mode);
  ecore_config_string_set("/entrance/autologin/user", vals.autologin_user);
  ecore_config_string_set("/entrance/greeting/after", vals.greeting_after);
  ecore_config_string_set("/entrance/greeting/before", vals.greeting_before);
  ecore_config_int_set("/entrance/greeting/use_default", vals.greeting_usedefault);
  ecore_config_int_set("/entrance/presel/mode", vals.presel_mode);
  ecore_config_int_set("/entrance/system/halt", vals.halt);
  ecore_config_int_set("/entrance/system/reboot", vals.reboot);
  ecore_config_int_set("/entrance/user/remember", vals.remember);
  ecore_config_int_set("/entrance/user/remember_n", vals.remember_n);
  ecore_config_int_set("/entrance/auth", vals.auth);
  ecore_config_string_set("/entrance/date_format", vals.date_format);
  ecore_config_string_set("/entrance/theme", vals.theme);
  ecore_config_string_set("/entrance/time_format", vals.time_format);
  ecore_config_int_set("/entranced/attempts", vals.attempts);
  ecore_config_string_set("/entranced/xserver", vals.xserver);
  ecore_config_file_save(file);
  // workaround - ecore_config sets bad permissions
  snprintf(buf, sizeof(buf), "chmod 644 %s", file);
  system(buf);
}

static void
create_gui(void)
{
   Evas_Object *win, *bg, *bx0, *bx1, *bx, *fr, *ck, *sc, *en, *lb, *sl, *bt;

   win = elm_win_add(NULL, "entrance_edit_elm", ELM_WIN_BASIC);
   elm_win_title_set(win, _("Entrance Edit GUI"));
   evas_object_smart_callback_add(win, "delete,request", close_app, NULL);
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   
   bx0 = elm_box_add(win);
   evas_object_size_hint_weight_set(bx0, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_horizontal_set(bx0, 1);
   elm_win_resize_object_add(win, bx0);
   evas_object_show(bx0);
   
   bx1 = elm_box_add(win);
   evas_object_size_hint_weight_set(bx1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_horizontal_set(bx1, 0);
   elm_box_pack_end(bx0, bx1);
   evas_object_show(bx1);
   
   fr = elm_frame_add(win);
   elm_frame_label_set(fr, _("Appearance"));
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx1, fr);
   evas_object_show(fr);
   
   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_frame_content_set(fr, bx);
   evas_object_show(bx);
   
   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_check_label_set(ck, _("Use builtin(translatable) greeting message"));
   elm_check_state_set(ck, vals.greeting_usedefault);
   elm_box_pack_end(bx, ck);
   ck_defgreet = ck;
   evas_object_show(ck);
   
   lb = elm_label_add(win);
   elm_label_label_set(lb, _("Greeting prefix:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, sc);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, 1);
   elm_entry_entry_set(en, vals.greeting_before);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
   elm_scroller_content_set(sc, en);
   en_greetb = en;
   evas_object_show(en);

   evas_object_show(sc);
   
   lb = elm_label_add(win);
   elm_label_label_set(lb, _("Greeting suffix:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, sc);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, 1);
   elm_entry_entry_set(en, vals.greeting_after);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
   elm_scroller_content_set(sc, en);
   en_greeta = en;
   evas_object_show(en);

   evas_object_show(sc);
   
   lb = elm_label_add(win);
   elm_label_label_set(lb, _("Date format:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, sc);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, 1);
   elm_entry_entry_set(en, vals.date_format);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
   elm_scroller_content_set(sc, en);
   en_date = en;
   evas_object_show(en);

   evas_object_show(sc);
   
   lb = elm_label_add(win);
   elm_label_label_set(lb, _("Time format:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, sc);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, 1);
   elm_entry_entry_set(en, vals.time_format);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
   elm_scroller_content_set(sc, en);
   en_time = en;
   evas_object_show(en);

   evas_object_show(sc);
   
   lb = elm_label_add(win);
   elm_label_label_set(lb, _("Entrance theme:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, sc);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, 1);
   elm_entry_entry_set(en, vals.theme);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
   elm_scroller_content_set(sc, en);
   en_theme = en;
   evas_object_show(en);

   evas_object_show(sc);
   
   fr = elm_frame_add(win);
   elm_frame_label_set(fr, _("Entrance Daemon settings"));
   elm_box_pack_end(bx1, fr);
   evas_object_show(fr);
   
   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_frame_content_set(fr, bx);
   evas_object_show(bx);
   
   lb = elm_label_add(win);
   elm_label_label_set(lb, _("Number of attempts:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sl = elm_slider_add(win);
   elm_slider_unit_format_set(sl, "%1.0f");
   elm_slider_min_max_set(sl, 1.0, 50.0);
   elm_slider_span_size_set(sl, 200);
   elm_slider_value_set(sl, vals.attempts);
   evas_object_size_hint_align_set(sl, EVAS_HINT_FILL, 0.5);
   evas_object_size_hint_weight_set(sl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, sl);
   sl_attempts = sl;
   evas_object_show(sl);
   
   lb = elm_label_add(win);
   elm_label_label_set(lb, _("X server:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, sc);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, 1);
   elm_entry_entry_set(en, vals.xserver);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
   elm_scroller_content_set(sc, en);
   en_xserver = en;
   evas_object_show(en);

   evas_object_show(sc);
   
   bx1 = elm_box_add(win);
   evas_object_size_hint_weight_set(bx1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_horizontal_set(bx1, 0);
   elm_box_pack_end(bx0, bx1);
   evas_object_show(bx1);
   
   fr = elm_frame_add(win);
   elm_frame_label_set(fr, "Login");
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx1, fr);
   evas_object_show(fr);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_frame_content_set(fr, bx);
   evas_object_show(bx);
   
   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_check_label_set(ck, _("Automatic login"));
   elm_check_state_set(ck, vals.autologin_mode);
   elm_box_pack_end(bx, ck);
   ck_autolog = ck;
   evas_object_show(ck);
   
   lb = elm_label_add(win);
   elm_label_label_set(lb, _("Autologin user:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, sc);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, 1);
   elm_entry_entry_set(en, vals.autologin_user);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, 0.5);
   elm_scroller_content_set(sc, en);
   en_autolog = en;
   evas_object_show(en);

   evas_object_show(sc);

   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_check_label_set(ck, _("Pre-selected user"));
   elm_check_state_set(ck, vals.presel_mode);
   elm_box_pack_end(bx, ck);
   ck_presel = ck;
   evas_object_show(ck);

   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_check_label_set(ck, _("Remember users"));
   elm_check_state_set(ck, vals.remember);
   elm_box_pack_end(bx, ck);
   ck_remember = ck;
   evas_object_show(ck);

   lb = elm_label_add(win);
   elm_label_label_set(lb, _("Number of remembers:"));
   elm_box_pack_end(bx, lb);
   evas_object_show(lb);
   
   sl = elm_slider_add(win);
   elm_slider_unit_format_set(sl, "%1.0f");
   elm_slider_min_max_set(sl, 1.0, 50.0);
   elm_slider_span_size_set(sl, 200);
   elm_slider_value_set(sl, vals.remember_n);
   evas_object_size_hint_align_set(sl, EVAS_HINT_FILL, 0.5);
   evas_object_size_hint_weight_set(sl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, sl);
   sl_remember = sl;
   evas_object_show(sl);

   fr = elm_frame_add(win);
   elm_frame_label_set(fr, _("System"));
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx1, fr);
   evas_object_show(fr);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_frame_content_set(fr, bx);
   evas_object_show(bx);
   
   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_check_label_set(ck, _("Reboot allowed"));
   elm_check_state_set(ck, vals.reboot);
   elm_box_pack_end(bx, ck);
   ck_reboot = ck;
   evas_object_show(ck);

   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_check_label_set(ck, _("Shutdown allowed"));
   elm_check_state_set(ck, vals.halt);
   elm_box_pack_end(bx, ck);
   ck_shutdown = ck;
   evas_object_show(ck);

   ck = elm_check_add(win);
   evas_object_size_hint_weight_set(ck, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ck, EVAS_HINT_FILL, 0.5);
   elm_check_label_set(ck, _("PAM Authentication"));
   elm_check_state_set(ck, vals.auth);
   elm_box_pack_end(bx, ck);
   ck_pam = ck;
   evas_object_show(ck);
   
   fr = elm_frame_add(win);
   elm_frame_label_set(fr, _("Don't forget to save your settings before closing app."));
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx1, fr);
   evas_object_show(fr);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_horizontal_set(bx, 1);
   elm_frame_content_set(fr, bx);
   evas_object_show(bx);
   
   bt = elm_button_add(win);
   elm_button_label_set(bt, _("Save settings"));
   evas_object_smart_callback_add(bt, "clicked", save_settings, NULL);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   
   bt = elm_button_add(win);
   elm_button_label_set(bt, _("Exit application"));
   evas_object_smart_callback_add(bt, "clicked", close_app, NULL);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   evas_object_resize(win, 240, 480);
   evas_object_show(win);
}

EAPI int
elm_main(int argc, char **argv)
{
   /* Set locale to user's environment */
   if (!(setlocale(LC_ALL, "")))
      fprintf(stderr,
              _("Locale set failed!\nPlease make sure you have your locale files installed for \"%s\"\n"),
              getenv("LANG"));
   bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
   textdomain(PACKAGE);
   
   if (getuid() != 0)
     {
       fprintf(stderr, _("You need root permissions to run this\n"));
       return 1;
     }
   ecore_init();
   ecore_config_init("entrance");
   ecore_file_init();
   if (argv[1])
     {
       if (!ecore_file_exists(argv[1]))
         file = argv[1];
       else
         {
           file = "/etc/entrance_config.cfg";
           if (!ecore_file_exists(file)) return 1;
         }
     }
   else
     {
       file = "/etc/entrance_config.cfg";
       if (!ecore_file_exists(file)) return 1;
     }
   ecore_config_file_load(file);
   get_settings();
   create_gui();
   elm_run();
   elm_shutdown();
   return 0;
}
#endif

ELM_MAIN()
