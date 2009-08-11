MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v -id $(srcdir)

filesdir = $(datadir)/$(PACKAGE_NAME)/data
files_DATA = $(builddir)/default.edj

# find . -type f \( -name '*.edc' -o -name '*.png' -o -name '*.jpg' \) | sort -u | sed 's/\(.*\)/\1 \\/g'
EXTRA_DIST = \
./chrome/back.png \
./chrome/chrome.edc \
./chrome/control.png \
./chrome/favorite.png \
./chrome/forward.png \
./chrome/go.png \
./chrome/inset_sunk.png \
./chrome/reload.png \
./chrome/tb_base.png \
./chrome/tb_shine.png \
./default.edc \
./widget/button/button.edc \
./widget/button/img_button_focus.png \
./widget/button/img_button_hover.png \
./widget/button/img_button_normal.png \
./widget/button/img_button_press.png \
./widget/check/check.edc \
./widget/check/img_check_off_focus.png \
./widget/check/img_check_off_hover.png \
./widget/check/img_check_off.png \
./widget/check/img_check_on_focus.png \
./widget/check/img_check_on_hover.png \
./widget/check/img_check_on.png \
./widget/combo/combo.edc \
./widget/combo/combo_focus_button.png \
./widget/combo/combo_focus.png \
./widget/combo/combo_hover_button.png \
./widget/combo/combo_hover.png \
./widget/combo/combo_normal_button.png \
./widget/combo/combo_normal.png \
./widget/combo/combo_press_button.png \
./widget/combo/combo_press.png \
./widget/combo/icon.png \
./widget/entry/entry.edc \
./widget/entry/img_focused.png \
./widget/entry/img_hovered.png \
./widget/entry/img_normal.png \
./widget/file/file.edc \
./widget/file/file_focus.png \
./widget/file/file_hover.png \
./widget/file/file_normal.png \
./widget/file/file_press.png \
./widget/radio/img_radio_off_focus.png \
./widget/radio/img_radio_off_hover.png \
./widget/radio/img_radio_off.png \
./widget/radio/img_radio_on_focus.png \
./widget/radio/img_radio_on_hover.png \
./widget/radio/img_radio_on.png \
./widget/radio/radio.edc \
./widget/scrollbar/scrollbar.edc \
./widget/scrollbar/scrollbar_hilight.png \
./widget/scrollbar/scrollbar_h.png \
./widget/scrollbar/scrollbar_knob_h.png \
./widget/scrollbar/scrollbar_knob_v.png \
./widget/scrollbar/scrollbar_v.png \
./widget/search/cancel/cancel_normal_button2.png \
./widget/search/cancel/cancel_normal_button.png \
./widget/search/cancel/search_cancel.edc \
./widget/search/decoration/decoration_normal_button.png \
./widget/search/decoration/search_decoration.edc \
./widget/search/field/field_focused.png \
./widget/search/field/field_hovered.png \
./widget/search/field/field_normal.png \
./widget/search/field/search_field.edc


all: default.edj

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(srcdir)/default.edc $(builddir)/default.edj

CLEANFILES = $(builddir)/default.edj
