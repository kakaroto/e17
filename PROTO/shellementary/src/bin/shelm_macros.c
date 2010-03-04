#include "shelm.h"

SH_API Evas_Object *
create_window(const char *window_name, const char *window_title, void *destroy)
{
  Evas_Object *window;

  window = elm_win_add(NULL, window_name, ELM_WIN_DIALOG_BASIC);

  elm_win_title_set(window, window_title);
  evas_object_smart_callback_add(window, "delete,request", destroy, NULL);

  return window;
}

SH_API Evas_Object *
create_background(Evas_Object *parent, const char *window_background, Eina_Bool window_resizable)
{
  Evas_Object *background;

  background = elm_bg_add(parent);

  if (window_background) elm_bg_file_set(background, window_background, NULL);

  if (window_resizable) evas_object_size_hint_weight_set(background, 1.0, 1.0);

  return background;
}

SH_API Evas_Object *
create_box(Evas_Object *parent, Eina_Bool is_horizontal)
{
  Evas_Object *box;

  box = elm_box_add(parent);

  elm_box_horizontal_set(box, is_horizontal);

  return box;
}

SH_API Evas_Object *
create_frame(Evas_Object *parent, Eina_Bool window_resizable)
{
  Evas_Object *frame;

  frame = elm_frame_add(parent);

  if (window_resizable) evas_object_size_hint_weight_set(frame, 1.0, 1.0);

  return frame;
}

SH_API Evas_Object *
create_button(Evas_Object *parent, const char *button_icon, const char *label)
{
  Evas_Object *button;

  button = elm_button_add(parent);

  if (button_icon)
    {
      Evas_Object *icon;
      icon = elm_icon_add(parent);

      elm_icon_file_set(icon, button_icon, NULL);
      evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
      evas_object_show(icon);
      elm_button_icon_set(button, icon);
    }

  elm_button_label_set(button, label);

  return button;
}

SH_API Evas_Object *
create_label(Evas_Object *parent, const char *text)
{
  Evas_Object *label;

  label = elm_label_add(parent);

  elm_label_label_set(label, text);

  return label;
}
			
SH_API Evas_Object *
create_scroller(Evas_Object *parent, Eina_Bool scroller_has_one_line)
{
  Evas_Object *scroller;

  scroller = elm_scroller_add(parent);

  evas_object_size_hint_weight_set(scroller, 1.0, 1.0);
  evas_object_size_hint_align_set(scroller, -1.0, -1.0);

  if (scroller_has_one_line)
    elm_scroller_content_min_limit(scroller, 0, 1);

  return scroller;
}

SH_API Evas_Object *
create_entry(Evas_Object *parent, Eina_Bool entry_has_one_line, const char *entry_text, Eina_Bool entry_hide_text, Eina_Bool entry_line_nowrap, Eina_Bool entry_editable)
{
  Evas_Object *entry;
  char *s = NULL;

  if (entry_text) s = elm_entry_utf8_to_markup(entry_text);

  entry = elm_entry_add(parent);

  if (entry_line_nowrap)
    elm_entry_line_wrap_set(entry, 0);
  else
    elm_entry_line_wrap_set(entry, 1);
  if (s)
    {
      elm_entry_entry_set(entry, s);
      E_FREE(s);
    }
  elm_entry_password_set(entry, entry_hide_text);

  evas_object_size_hint_weight_set(entry, 1.0, 1.0);
  evas_object_size_hint_align_set(entry, -1.0, -1.0);

  elm_entry_single_line_set(entry, entry_has_one_line);
  elm_entry_editable_set(entry, entry_editable);

  return entry;
}

SH_API Evas_Object *
create_icon(Evas_Object *parent, const char *iconfile)
{
  Evas_Object *icon;

  icon = elm_icon_add(parent);

  elm_icon_file_set(icon, iconfile, NULL);
  elm_icon_scale_set(icon, 0, 0);

  return icon;
}

SH_API Evas_Object *
create_list(Evas_Object *parent)
{
  Evas_Object *elmlist;

  elmlist = elm_list_add(parent);

  evas_object_size_hint_weight_set(elmlist, 1.0, 1.0);
  evas_object_size_hint_align_set(elmlist, -1.0, -1.0);

  return elmlist;
}

SH_API Evas_Object *
create_clock(Evas_Object *parent, Eina_Bool show_seconds, Eina_Bool show_am_pm, const char *time, Eina_Bool is_editable)
{
  Evas_Object *clock;

  clock = elm_clock_add(parent);
  elm_clock_show_seconds_set(clock, show_seconds);
  elm_clock_show_am_pm_set(clock, show_am_pm);
  if (time)
    {
      char **timelist;
      timelist = eina_str_split(time, ',', 3);
      elm_clock_time_set(clock, atoi(timelist[0]), atoi(timelist[1]), atoi(timelist[2]));
      timelist = NULL;
      free(timelist);
    }
  elm_clock_edit_set(clock, is_editable);

  return clock;
}

static void
slider_print_value(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object *slider = data;
  double value;

  value = elm_slider_value_get(slider);

  printf("%f\n", value);
}

SH_API Evas_Object *
create_slider(Evas_Object *parent, double slider_value, double slider_min_value, double slider_max_value, const char *slider_step, Eina_Bool slider_partial, Eina_Bool slider_hide_value, Eina_Bool slider_inverted, const char *slider_unit_format, const char *slider_label, const char *slider_icon, Eina_Bool slider_vertical)
{
  Evas_Object *slider;

  slider = elm_slider_add(parent);

  evas_object_size_hint_align_set(slider, -1.0, -1.0);
  if (slider_partial) evas_object_smart_callback_add(slider, "delay,changed", slider_print_value, slider);
  if (!slider_hide_value)
    {
      if (slider_step)
	{
	  char buf[PATH_MAX];
	  snprintf(buf, sizeof(buf), "%%%sf", slider_step);
	  elm_slider_indicator_format_set(slider, buf);
	}
      else
	elm_slider_indicator_format_set(slider, "%0.0f");
    }

  if (!slider_min_value) slider_min_value = 0.0;
  if (!slider_max_value) slider_max_value = 100.0;

  elm_slider_min_max_set(slider, slider_min_value, slider_max_value);
  elm_slider_inverted_set(slider, slider_inverted);

  if (slider_unit_format) elm_slider_unit_format_set(slider, slider_unit_format);
  if (slider_label) elm_slider_label_set(slider, slider_label);
  if (slider_icon)
    {
      Evas_Object *icon;
      icon = elm_icon_add(parent);

      elm_icon_file_set(icon, slider_icon, NULL);
      evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
      evas_object_show(icon);
      elm_slider_icon_set(slider, icon);
    }
  if (slider_vertical) elm_slider_horizontal_set(slider, 0);
  if (slider_value) elm_slider_value_set(slider, slider_value);

  return slider;
}
