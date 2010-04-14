#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

# ----- Progressbar {{{
my_progressbar_run = False
my_progressbar_timer = None

def my_progressbar_value_set(pb1, pb2, pb3, pb4, pb5, pb6, pb7):
    progress = pb1.value_get()
    if progress < 1.0:
        progress += 0.0123
    else:
        progress = 0.0
    pb1.value_set(progress)
    pb4.value_set(progress)
    pb3.value_set(progress)
    pb6.value_set(progress)
    if progress < 1.0:
        return ecore.ECORE_CALLBACK_RENEW
    global my_progressbar_run
    my_progressbar_run = False
    return ecore.ECORE_CALLBACK_CANCEL

def my_progressbar_test_start(obj, *args, **kwargs):
    (pb1, pb2, pb3, pb4, pb5, pb6, pb7) = args
    pb2.pulse(True)
    pb5.pulse(True)
    pb7.pulse(True)
    global my_progressbar_run
    global my_progressbar_timer
    if not my_progressbar_run:
        my_progressbar_timer = ecore.timer_add(0.1, my_progressbar_value_set,
                                               *args)
        my_progressbar_run = True

def my_progressbar_test_stop(obj, pb1, pb2, pb3, pb4, pb5, pb6, pb7):
    pb2.pulse(False)
    pb5.pulse(False)
    pb7.pulse(False)
    global my_progressbar_run
    global my_progressbar_timer
    if my_progressbar_run:
        my_progressbar_timer.delete()
        my_progressbar_run = False

def my_progressbar_destroy(obj, *args):
    my_progressbar_test_stop(None, *args)
    obj.delete()

def progressbar_clicked(obj, it):
    win = elementary.Window("progressbar", elementary.ELM_WIN_BASIC)
    win.title_set("Progressbar test")

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    pb1 = elementary.Progressbar(win)
    pb1.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    pb1.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    bx.pack_end(pb1)
    pb1.show()

    pb2 = elementary.Progressbar(win)
    pb2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    pb2.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb2.label_set("Infinite bounce")
    pb2.pulse_set(True)
    bx.pack_end(pb2)
    pb2.show()

    ic1 = elementary.Icon(win)
    ic1.file_set('images/logo_small.png')
    ic1.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    pb3 = elementary.Progressbar(win)
    pb3.label_set("Label")
    pb3.icon_set(ic1)
    pb3.inverted_set(True)
    pb3.unit_format_set("%1.1f units")
    pb3.span_size_set(200)
    pb3.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb3.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(pb3)
    ic1.show()
    pb3.show()

    hbx = elementary.Box(win)
    hbx.horizontal_set(True)
    hbx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    hbx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(hbx)
    hbx.show()

    pb4 = elementary.Progressbar(win)
    pb4.horizontal_set(False)
    pb4.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    pb4.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    hbx.pack_end(pb4)
    pb4.span_size_set(60)
    pb4.label_set("percent")
    pb4.show()

    pb5 = elementary.Progressbar(win)
    pb5.horizontal_set(False)
    pb5.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb5.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    pb5.span_size_set(80)
    pb5.pulse_set(True)
    pb5.unit_format_set(None)
    pb5.label_set("Infinite bounce")
    hbx.pack_end(pb5)
    pb5.show()

    ic2 = elementary.Icon(win)
    ic2.file_set('images/logo_small.png')
    ic2.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_HORIZONTAL, 1, 1)

    pb6 = elementary.Progressbar(win)
    pb6.horizontal_set(False)
    pb6.label_set("Label")
    pb6.icon_set(ic2)
    pb6.inverted_set(True)
    pb6.unit_format_set("%1.2f%%")
    pb6.span_size_set(200)
    pb6.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb6.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    hbx.pack_end(pb6)
    ic2.show()
    pb6.show()

    pb7 = elementary.Progressbar(win)
    pb7.style_set("wheel")
    pb7.label_set("Style: wheel")
    pb7.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb7.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(pb7)
    pb7.show()

    bt_bx = elementary.Box(win)
    bt_bx.horizontal_set(True)
    bt_bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(bt_bx)
    bt_bx.show()

    pbt = (pb1, pb2, pb3, pb4, pb5, pb6, pb7)

    bt = elementary.Button(win)
    bt.label_set("Start")
    bt.callback_clicked_add(my_progressbar_test_start, *pbt)
    bt_bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Stop")
    bt.callback_clicked_add(my_progressbar_test_stop, *pbt)
    bt_bx.pack_end(bt)
    bt.show()

    win.callback_destroy_add(my_progressbar_destroy, *pbt)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    progressbar_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
