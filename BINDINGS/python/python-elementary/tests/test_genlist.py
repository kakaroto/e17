#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas


#----- Genlist -{{{-
def genlist_clicked(obj, it):
    win = elementary.Window("Genlist", elementary.ELM_WIN_BASIC)
    win.title_set("Genlist test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    gl = elementary.Genlist(win)
    def _gl_selected(item, gl, item_data):
        print "selected"
    gl._callback_add("selected", _gl_selected, gl)
    def _gl_clicked(item, gl, item_data):
        print "clicked"
    gl._callback_add("clicked", _gl_clicked, gl)
    def _gl_longpressed(item, gl, item_data):
        print "longpressed"
    gl._callback_add("longpressed", _gl_longpressed, gl)
    gl.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    gl.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(gl)
    gl.show()

    over = evas.Rectangle(win.evas_get())
    over.color_set(0, 0, 0, 0)

    def _gl_move(evas, evt, gl):
        gli = gl.at_xy_item_get(evt.position.canvas.x, evt.position.canvas.y)
        if gli:
            print "over %s", gli
        else:
            print "over none"

    over.event_callback_add(evas.EVAS_CALLBACK_MOUSE_DOWN, _gl_move, gl)
    over.repeat_events_set(True)
    over.show()
    over.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(over)

    def gl_label_get(obj, part, item_data):
        return "Item # %i" % (item_data,)

    def gl_icon_get(obj, part, data):
        ic = elementary.Icon(obj)
        ic.file_set("images/logo_small.png")
        ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
        return ic

    def gl_state_get(obj, part, item_data):
        return False

    itc1 = elementary.GenlistItemClass(item_style="default",
                                       label_get_func=gl_label_get,
                                       icon_get_func=gl_icon_get,
                                       state_get_func=gl_state_get)

    bt_50 = elementary.Button(win)
    bt_50.label_set("Go to 50")
    bt_50.show()
    bx.pack_end(bt_50)

    bt_1500 = elementary.Button(win)
    bt_1500.label_set("Go to 1500")
    bt_1500.show()
    bx.pack_end(bt_1500)

    for i in xrange(0, 2000):
        def gl_sel(gli, gl):
            print "sel item %s on genlist %s" % (gli, gl)
        gli = gl.item_append(itc1, i, func=gl_sel)
        if i == 50:
            def _bt50_cb(button, gli):
                gli.bring_in()
            bt_50._callback_add("clicked", _bt50_cb, gli)
        elif i == 1500:
            def _bt1500_cb(button, gli):
                gli.middle_bring_in()
            bt_1500._callback_add("clicked", _bt1500_cb, gli)

    win.resize(480, 800)
    win.show()

# }}}
#----- Main -{{{-
if __name__ == "__main__":
    def destroy(obj):
        elementary.exit()

    elementary.init()
    win = elementary.Window("test", elementary.ELM_WIN_BASIC)
    win.title_set("python-elementary test application")
    win.callback_destroy_add(destroy)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    box0 = elementary.Box(win)
    box0.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(box0)
    box0.show()

    fr = elementary.Frame(win)
    fr.label_set("Information")
    box0.pack_end(fr)
    fr.show()

    lb = elementary.Label(win)
    lb.label_set("Please select a test from the list below<br>"
                 "by clicking the test button to show the<br>"
                 "test window.")
    fr.content_set(lb)
    lb.show()

    items = [("Genlist", genlist_clicked)]

    li = elementary.List(win)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    box0.pack_end(li)
    li.show()

    for item in items:
        li.item_append(item[0], callback=item[1])

    li.go()

    win.resize(320,520)
    win.show()
    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
