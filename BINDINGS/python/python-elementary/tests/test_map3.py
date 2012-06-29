#!/usr/bin/env python
# encoding: utf-8
import os
import random
import elementary
import edje
import ecore
import evas


_from = None
_to = None

#----- Map Route -{{{-

def cb_btn_goto(bt, Map):
    Map.zoom = 12
    Map.region_show(9.204, 45.446)

def cb_ctx_set_from(ctx, item, Map, lon, lat):
    global _from

    ctx.dismiss()
    if _from is None:
        _from = Map.overlay_add(lon, lat)
        _from.color = (150, 0, 0, 150)
    else:
        _from.region = (lon, lat)

def cb_ctx_set_to(ctx, item, Map, lon, lat):
    global _to

    ctx.dismiss()
    if _to is None:
        _to = Map.overlay_add(lon, lat)
        _to.color = (0, 0, 150, 150)
    else:
        _to.region = (lon, lat)

def cb_btn_calc_route(bt, Map):
    if not (_from and _to):
        return

    (flon, flat) = _from.region
    (tlon, tlat) = _to.region
    Map.route_add(elementary.ELM_MAP_ROUTE_TYPE_MOTOCAR,
                  elementary.ELM_MAP_ROUTE_METHOD_FASTEST,
                  flon, flat, tlon, tlat, cb_route)

    lb = Map.data["lb_distance"]
    lb.text = "requesting route..."

def cb_btn_search_name(bt, Map, en):
    Map.name_add(en.text, 0, 0, cb_search_name, en)
    en.text = "searching..."

def cb_search_name(Map, name, en):
    global _name

    en.text = name.address
    (lon, lat) = name.region
    Map.zoom = 12
    Map.region_show(lon, lat)

    if _from is None:
        _from = Map.overlay_add(lon, lat)
        _from.color = (150, 0, 0, 150)
    else:
        _from.region = (lon, lat)

def cb_btn_search_region(bt, Map, en):
    if _from is None:
        return
    (lon, lat) = _from.region
    Map.name_add(None, lon, lat, cb_search_region, en)
    en.text = "searching..."

def cb_search_region(Map, name, en):
    global _name

    en.text = name.address

def cb_route(Map, route):
    nodes = route.node.count('\n')
    lb = Map.data["lb_distance"]
    lb.text = "distance: %.2f Km   nodes:%d" % (route.distance, nodes)

    ov = Map.overlay_route_add(route)

    print("Node: %s" % (route.node))
    print("Waypoint %s" % (route.waypoint))

def cb_btn_clear_overlays(btn, Map):
    for ov in Map.overlays:
        if ov != _from and ov != _to:
            ov.delete()

def cb_map_clicked(Map):
    (x, y) = Map.evas.pointer_canvas_xy_get()
    (lon, lat) = Map.canvas_to_region_convert(x, y)
    cp = elementary.Ctxpopup(Map)
    cp.item_append("%f  %f" % (lon, lat)).disabled = True
    cp.item_append("Set start point", None, cb_ctx_set_from, Map, lon, lat)
    cp.item_append("Set end point", None, cb_ctx_set_to, Map, lon, lat)
    cp.move(x, y)
    cp.show()

def cb_map_load(Map):
    lb = Map.data["lb_load_status"]
    lb.text = "tile_load_status: %d / %d" % Map.tile_load_status_get()

def cb_hovsel_selected(hov, item, Map, type, name):
    Map.source_set(type, item.text)
    hov.text = "%s: %s" % (name, item.text)

def map_route_clicked(obj):
    win = elementary.Window("maproute", elementary.ELM_WIN_BASIC)
    win.title = "Map Route test"
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

    Map = elementary.Map(win)
    Map.zoom = 2
    Map.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    Map.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    Map.callback_clicked_add(cb_map_clicked)
    Map.callback_tile_load_add(cb_map_load)
    Map.callback_tile_loaded_add(cb_map_load)
    vbox.pack_end(Map)
    Map.show()

    lb = elementary.Label(win)
    lb.text = "load_status: 0 / 0"
    vbox.pack_end(lb)
    lb.show()
    Map.data["lb_load_status"] = lb

    lb = elementary.Label(win)
    lb.text = "First set Start and End point and then click 'Calc Route !'"
    vbox.pack_end(lb)
    lb.show()
    Map.data["lb_distance"] = lb

    # info
    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    hbox.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    # route
    ho = elementary.Hoversel(win)
    ho.hover_parent_set(win)
    type = elementary.ELM_MAP_SOURCE_TYPE_ROUTE
    ho.text_set("Routes: %s" % (Map.source_get(type)))
    for src in Map.sources_get(type):
        ho.item_add(src)
    ho.callback_selected_add(cb_hovsel_selected, Map, type, "Routes")
    hbox.pack_end(ho)
    ho.show()

    sep = elementary.Separator(win)
    sep.show()
    hbox.pack_end(sep)
    
    bt = elementary.Button(win)
    bt.text = "GOTO"
    bt.callback_clicked_add(cb_btn_goto, Map)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "Calc Route !"
    bt.callback_clicked_add(cb_btn_calc_route, Map)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "clear route overlays"
    bt.callback_clicked_add(cb_btn_clear_overlays, Map)
    hbox.pack_end(bt)
    bt.show()

    # names
    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    hbox.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    ho = elementary.Hoversel(win)
    ho.hover_parent_set(win)
    type = elementary.ELM_MAP_SOURCE_TYPE_NAME
    ho.text_set("Names: %s" % (Map.source_get(type)))
    for src in Map.sources_get(type):
        ho.item_add(src)
    ho.callback_selected_add(cb_hovsel_selected, Map, type, "Names")
    hbox.pack_end(ho)
    ho.show()

    en = elementary.Entry(win)
    en.scrollable = True
    en.single_line = True
    en.text = "type an address here"
    en.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    en.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    hbox.pack_end(en)
    en.show()

    bt = elementary.Button(win)
    bt.text = "Search Address !"
    bt.callback_clicked_add(cb_btn_search_name, Map, en)
    hbox.pack_end(bt)
    bt.show()

    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    hbox.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    en = elementary.Entry(win)
    en.scrollable = True
    en.single_line = True
    en.disabled = True
    en.text = "place the start point and press the button"
    en.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    en.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    hbox.pack_end(en)
    en.show()

    bt = elementary.Button(win)
    bt.text = "Search start point Region"
    bt.callback_clicked_add(cb_btn_search_region, Map, en)
    hbox.pack_start(bt)
    bt.show()


    
    win.resize(600, 600)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    map_route_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
