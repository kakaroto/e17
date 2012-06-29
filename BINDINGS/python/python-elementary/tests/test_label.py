#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Label -{{{-

def cb_slide_radio(radio, lb):
    lb.style = radio.text

def cb_slider_duration(slider, lb):
    lb.slide = False
    lb.slide_duration = slider.value
    lb.slide = True

def label_clicked(obj):
    win = elementary.Window("label", elementary.ELM_WIN_BASIC)
    win.title = "Label test"
    win.autodel = True
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    vbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    win.resize_object_add(vbox)
    vbox.show()

    lb = elementary.Label(win)
    lb.text = "<b>This is a small label</b>"
    lb.size_hint_align = (0.0, 0.5)
    vbox.pack_end(lb)
    lb.show()

    lb = elementary.Label(win)
    lb.text = "This is a larger label with newlines<br/>" \
              "to make it bigger, bit it won't expand or wrap<br/>" \
              "just be a block of text that can't change its<br/>" \
              "formatting as it's fixed based on text<br/>"
    lb.size_hint_align = (0.0, 0.5)
    vbox.pack_end(lb)
    lb.show()

    lb = elementary.Label(win)
    lb.line_wrap_set(elementary.ELM_WRAP_CHAR)
    lb.text =  "<b>This is more text designed to line-wrap here as " \
               "This object is resized horizontally. As it is " \
               "resized vertically though, nothing should change. " \
               "The amount of space allocated vertically should " \
               "change as horizontal size changes.</b>"
    lb.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    lb.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.pack_end(lb)
    lb.show()

    lb = elementary.Label(win)
    lb.text = "This small label set to wrap"
    lb.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    lb.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.pack_end(lb)
    lb.show()

    sp = elementary.Separator(win)
    sp.horizontal = True
    vbox.pack_end(sp)
    sp.show()

    gd = elementary.Grid(win)
    gd.size = (100, 100)
    gd.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    gd.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.pack_end(gd)
    gd.show()
    
    lb = elementary.Label(win)
    lb.text = "Test Label Ellipsis:"
    lb.size_hint_align = (0.0, 0.5)
    gd.pack(lb, 5, 5, 90, 15)
    lb.show()

    rect = evas.Rectangle(win.evas)
    rect.color = (255, 125, 125, 255)
    gd.pack(rect, 5, 15, 90, 15)
    rect.show()

    lb = elementary.Label(win)
    lb.text = "This is a label set to ellipsis. " \
              "If set ellipsis to true and the text doesn't fit " \
              "in the label an ellipsis(\"...\") will be shown " \
              "at the end of the widget."
    lb.ellipsis = True
    lb.size_hint_align = (0.0, 0.5)
    gd.pack(lb, 5, 15, 90, 15)
    lb.show()

    lb = elementary.Label(win)
    lb.text = "Test Label Slide:"
    lb.size_hint_align = (0.0, 0.5)
    gd.pack(lb, 5, 40, 90, 15)
    lb.show()

    rect = evas.Rectangle(win.evas)
    rect.color = (255, 125, 125, 255)
    gd.pack(rect, 5, 50, 90, 15)
    rect.show()

    lb = elementary.Label(win)
    lb.text = "This is a label set to slide. " \
              "If set slide to true the text of the label " \
              "will slide/scroll through the length of label." \
              "This only works with the themes \"slide_short\", " \
              "\"slide_long\" and \"slide_bounce\"."
    lb.slide = True
    lb.style = ("slide_short")
    lb.size_hint_align = (0.0, 0.5)
    gd.pack(lb, 5, 50, 90, 15)
    lb.show()

    rd = elementary.Radio(win)
    rd.state_value = 1
    rd.text = "slide_short"
    gd.pack(rd, 5, 65, 30, 15)
    rd.callback_changed_add(cb_slide_radio, lb)
    rd.show()
    rdg = rd

    rd = elementary.Radio(win)
    rd.group_add(rdg)
    rd.state_value = 2
    rd.text = "slide_long"
    gd.pack(rd, 35, 65, 30, 15)
    rd.callback_changed_add(cb_slide_radio, lb)
    rd.show()

    rd = elementary.Radio(win)
    rd.group_add(rdg)
    rd.state_value = 3
    rd.text = "slide_bounce"
    gd.pack(rd, 65, 65, 30, 15)
    rd.callback_changed_add(cb_slide_radio, lb)
    rd.show()

    sl = elementary.Slider(win)
    sl.text = "Slide Duration"
    sl.unit_format = "%1.1f units"
    sl.min_max = (1, 20)
    sl.value = 10
    sl.size_hint_align = (evas.EVAS_HINT_FILL, 0.5)
    sl.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    sl.callback_changed_add(cb_slider_duration, lb)
    gd.pack(sl, 5, 80, 90, 15)
    sl.show()

    win.resize(280, 400)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    label_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
