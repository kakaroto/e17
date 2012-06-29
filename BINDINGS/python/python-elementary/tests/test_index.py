#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas


#----- Index -{{{-
def gl_text_get(gl, part, data):
    return str(data)

def cb_idx_item(idx, item, gl_item):
    print(("Current Index: %s" % (item.letter)))
    gl_item.bring_in()

def cb_idx_changed(idx, item):
    print(("changed event on: %s" % (item.letter)))
    
def cb_idx_delay_changed(idx, item):
    print(("delay_changed event on: %s" % (item.letter)))
    # BROKEN (see below)
    # gl_item = item.data["gl_item"]
    # gl_item.bring_in()
    
def cb_idx_selected(idx, item):
    print(("selected event on: %s" % (item.letter)))
    

def index_clicked(obj):
    win = elementary.Window("index", elementary.ELM_WIN_BASIC)
    win.title_set("Index test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    vbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(vbox)
    vbox.show()

    # index
    idx = elementary.Index(win)
    idx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    idx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    idx.callback_delay_changed_add(cb_idx_delay_changed)
    idx.callback_changed_add(cb_idx_changed)
    idx.callback_selected_add(cb_idx_selected)
    win.resize_object_add(idx)
    idx.show()
    
    # genlist
    itc = elementary.GenlistItemClass(item_style="default",
                                      text_get_func=gl_text_get)
                                      # content_get_func=gl_content_get,
                                      # state_get_func=gl_state_get)
    gl = elementary.Genlist(win)
    gl.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    gl.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.pack_end(gl)
    gl.show()

    
    for i in 'ABCDEFGHILMNOPQRSTUVZ':
        for j in 'acegikmo':
            gl_item = gl.item_append(itc, i + j)
            if j == 'a':
                idx_item = idx.item_append(i, cb_idx_item, gl_item)

                # TODO This is broken (data assignment to ItemObject)
                # ...for now just bringin on select
                # idx_item.data["gl_item"] = gl_item
        

    idx.level_go(0)

    sep = elementary.Separator(win)
    sep.horizontal = True
    vbox.pack_end(sep)
    sep.show()

    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    ck = elementary.Check(win)
    ck.text = "autohide_disabled"
    ck.callback_changed_add(lambda ck: idx.autohide_disabled_set(ck.state))
    hbox.pack_end(ck)
    ck.show()

    ck = elementary.Check(win)
    ck.text = "indicator_disabled"
    ck.callback_changed_add(lambda ck: idx.indicator_disabled_set(ck.state))
    hbox.pack_end(ck)
    ck.show()

    ck = elementary.Check(win)
    ck.text = "horizontal"
    ck.callback_changed_add(lambda ck: idx.horizontal_set(ck.state))
    hbox.pack_end(ck)
    ck.show()

    win.resize(320, 480)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    index_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
