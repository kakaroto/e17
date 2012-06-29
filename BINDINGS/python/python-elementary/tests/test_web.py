#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Web -{{{-
def web_clicked(obj):
    if not elementary.need_web():
        print("EFL-webkit not available!")
        return

    win = elementary.Window("web", elementary.ELM_WIN_BASIC)
    win.title_set("Web")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbx = elementary.Box(win)
    win.resize_object_add(vbx)
    vbx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbx.show()

    web = elementary.Web(win)
    web.uri_set("http://enlightenment.org/")
    web.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    web.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    web.size_hint_min_set(100, 100)
    vbx.pack_end(web)
    web.show()

    # Debug:
    def dbg(*args):
        print(("DEBUG: %s" % args[-1], " ".join(repr(x) for x in args[1:-1])))
    web.callback_link_hover_in_add(dbg, "link in")
    web.callback_link_hover_out_add(dbg, "link out")

    web.callback_uri_changed_add(dbg, "uri")
    web.callback_title_changed_add(dbg, "title")
    web.callback_load_finished_add(dbg, "load finished")
    web.callback_load_finished_add(dbg, "load error")
    web.callback_load_progress_add(dbg, "load progress")
    web.callback_load_provisional_add(dbg, "load provisional")
    web.callback_load_started_add(dbg, "load started")

    # JS debug to console:
    def console_msg(obj, msg, line, src):
        print(("CONSOLE: %s:%d %r" % (src, line, msg)))
    web.console_message_hook_set(console_msg)

    # navigation bar:
    hbx = elementary.Box(win)
    hbx.horizontal_set(True)
    hbx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    hbx.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    vbx.pack_start(hbx)
    hbx.show()

    bt = elementary.Button(win)
    bt.text_set("Back")
    bt.callback_clicked_add(lambda x: web.back())
    hbx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Forward")
    bt.callback_clicked_add(lambda x: web.forward())
    hbx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Reload")
    bt.callback_clicked_add(lambda x: web.reload())
    hbx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Stop")
    bt.callback_clicked_add(lambda x: web.stop())
    hbx.pack_end(bt)
    bt.show()

    en = elementary.Entry(win)
    en.scrollable_set(True)
    en.editable_set(True)
    en.single_line_set(True)
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    hbx.pack_end(en)
    en.show()

    # Sync navigation entry and current URI
    def do_change_uri(en):
        web.uri = en.entry

    def did_change_uri(web, uri, en):
        en.entry = uri

    en.callback_activated_add(do_change_uri)
    web.callback_uri_changed_add(did_change_uri, en)

    # Sync title
    def did_change_title(web, title, win):
        win.title_set("Web - %s" % title)
    web.callback_title_changed_add(did_change_title, win)

    win.resize(800, 600)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()
    elementary.policy_set(elementary.ELM_POLICY_QUIT,
                          elementary.ELM_POLICY_QUIT_LAST_WINDOW_CLOSED)

    web_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
