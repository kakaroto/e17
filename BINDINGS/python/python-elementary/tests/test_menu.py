#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Menu -{{{-
def menu_show(rect, evtinfo, menu):
    (x,y) = evtinfo.position.canvas
    menu.move(x, y)
    menu.show()

def menu_populate_4(menu, item):
    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    menu.item_add(item, "menu 2", ic)

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')

    item2 = menu.item_add(item, "menu 3", ic)

    menu.item_separator_add(item)

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    item3 = menu.item_add(item, "Disabled item", ic);
    item3.disabled = True

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    item3 = menu.item_add(item, "Disabled item", ic);
    item3.disabled = True

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    item3 = menu.item_add(item, "Disabled item", ic);
    item3.disabled = True

def menu_populate_3(menu, item):
    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    menu.item_add(item, "menu 2", ic)

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')

    item2 = menu.item_add(item, "menu 3", ic)

    menu.item_separator_add(item)

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')

    item3 = menu.item_add(item, "Disabled item", ic);
    item3.disabled = True

def menu_populate_2(menu, item):
    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    menu.item_add(item, "menu 2", ic)

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    item2 = menu.item_add(item, "menu 3", ic)

    menu_populate_3(menu, item2)

    menu.item_separator_add(item)
    menu.item_separator_add(item)
    menu.item_separator_add(item)
    menu.item_separator_add(item)
    menu.item_separator_add(item)
    menu.item_separator_add(item)
    menu.item_separator_add(item)

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    item2 = menu.item_add(item, "menu 2", ic);

    menu.item_separator_add(item)

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    item3 = menu.item_add(item, "Disabled item", ic);
    item3.disabled = True

    menu_populate_4(menu, item2)

def menu_populate_1(menu, item):
    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    item2 = menu.item_add(item, "menu 1", ic)

    menu_populate_2(menu, item2)

def menu_clicked(obj, it):
    win = elementary.Window("menu", elementary.ELM_WIN_BASIC)
    win.title_set("Menu test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    rect = evas.Rectangle(win.evas_get())
    win.resize_object_add(rect)
    rect.color_set(0, 0, 0, 0)
    rect.show()

    menu = elementary.Menu(win)
    item = menu.item_add(None, "first item")

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')

    item = menu.item_add(None, "second item", ic)
    menu_populate_1(menu, item)

    ic = elementary.Icon(menu)
    ic.file_set('images/logo_small.png')
    menu.item_add(item, "sub menu", ic)

    rect.event_callback_add(evas.EVAS_CALLBACK_MOUSE_DOWN, menu_show, menu)

    win.resize(350, 200)
    win.show()

# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    menu_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
