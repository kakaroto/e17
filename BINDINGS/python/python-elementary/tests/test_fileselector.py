#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- FileSelector -{{{-
def fs_cb_done(fs, selected, win):
    win.delete()

def fs_cb_selected(fs, selected, win):
    print "Selected file:", selected
    print "           or:", fs.selected_get()

def fs_cb_directory_open(fs, folder, win):
    print "Folder open:", folder

def ck_cb_is_save(bt, fs):
    print "Toggle is save"
    fs.is_save = not fs.is_save

def ck_cb_folder_only(bt, fs):
    print "Toggle folder_only"
    fs.folder_only = not fs.folder_only

def ck_cb_expandable(bt, fs):
    print "Toggle expandable"
    fs.expandable = not fs.expandable

def ck_cb_buttons(bt, fs):
    print "Toggle buttons_ok_cancel"
    fs.buttons_ok_cancel = not fs.buttons_ok_cancel

def bt_cb_sel_get(bt, fs):
    print "Get Selected:", fs.selected_get()

def bt_cb_path_get(bt, fs):
    print "Get Path:", fs.path_get()

def bt_cb_mode_cycle(bt, fs):
    mode = fs.mode + 1
    fs.mode_set(mode if mode < 2 else 0)

def fileselector_clicked(obj):
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
    fs.folder_only_set(False)
    fs.path_set(os.getenv("HOME"))
    fs.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fs.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    fs.callback_done_add(fs_cb_done, win)
    fs.callback_selected_add(fs_cb_selected, win)
    fs.callback_directory_open_add(fs_cb_directory_open, win)
    vbox.pack_end(fs)
    fs.show()

    sep = elementary.Separator(win)
    sep.horizontal_set(True)
    vbox.pack_end(sep)
    sep.show()
    
    hbox = elementary.Box(win)
    hbox.horizontal_set(True)
    vbox.pack_end(hbox)
    hbox.show()

    ck = elementary.Check(win)
    ck.text_set("is_save")
    ck.state_set(fs.is_save)
    ck.callback_changed_add(ck_cb_is_save, fs)
    hbox.pack_end(ck)
    ck.show()

    ck = elementary.Check(win)
    ck.text_set("folder_only")
    ck.state_set(fs.folder_only)
    ck.callback_changed_add(ck_cb_folder_only, fs)
    hbox.pack_end(ck)
    ck.show()

    ck = elementary.Check(win)
    ck.text_set("expandable")
    ck.state_set(fs.expandable)
    ck.callback_changed_add(ck_cb_expandable, fs)
    hbox.pack_end(ck)
    ck.show()
    
    ck = elementary.Check(win)
    ck.text_set("buttons")
    ck.state_set(fs.buttons_ok_cancel)
    ck.callback_changed_add(ck_cb_buttons, fs)
    hbox.pack_end(ck)
    ck.show()
    
    hbox = elementary.Box(win)
    hbox.horizontal_set(True)
    vbox.pack_end(hbox)
    hbox.show()

    bt = elementary.Button(win)
    bt.text_set("selected_get")
    bt.callback_clicked_add(bt_cb_sel_get, fs)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("path_get")
    bt.callback_clicked_add(bt_cb_path_get, fs)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("mode cycle")
    bt.callback_clicked_add(bt_cb_mode_cycle, fs)
    hbox.pack_end(bt)
    bt.show()


    win.resize(240, 350)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    fileselector_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
