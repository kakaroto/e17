#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- FileSelector -{{{-
def fileselector_clicked(obj, it):
    win = elementary.Window("fileselector", elementary.ELM_WIN_BASIC)
    win.title_set("File selector test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    win.resize_object_add(vbox)
    vbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.show()

    fs = elementary.Fileselector(win)
    fs.is_save_set(True)
    fs.expandable_set(False)
    fs.path_set(os.getenv("HOME"))
    fs.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fs.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.pack_end(fs)
    fs.show()

    def fs_cb_done(fs, selected, win):
        win.delete()
    fs.callback_done_add(fs_cb_done, win)
    def fs_cb_selected(fs, selected, win):
        print "Selected file:", selected
        print "or:", fs.selected_get()
    fs.callback_selected_add(fs_cb_selected, win)

    hbox = elementary.Box(win)
    hbox.horizontal_set(True)
    vbox.pack_end(hbox)
    hbox.show()

    bt = elementary.Button(win)
    bt.label_set("Toggle is_save")
    def bt_cb_is_save(bt, fs):
        print "Toggle is save"
        fs.is_save_set(not fs.is_save_get())
    bt.callback_clicked_add(bt_cb_is_save, fs)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("sel get")
    def bt_cb_sel_get(bt, fs):
        print "Get Selected:", fs.selected_get()
    bt.callback_clicked_add(bt_cb_sel_get, fs)
    hbox.pack_end(bt)
    bt.show()

    win.resize(240, 350)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    fileselector_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
