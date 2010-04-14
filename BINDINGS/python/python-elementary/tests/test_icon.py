#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Icon Transparent -{{{-
def icon_transparent_clicked(obj, it):
    win = elementary.Window("icon-transparent", elementary.ELM_WIN_BASIC)
    win.title_set("Icon Transparent")
    win.autodel_set(True)
    win.alpha_set(True)

    icon = elementary.Icon(win)
    icon.file_set("images/logo.png")
    icon.scale_set(0, 0)
    win.resize_object_add(icon)
    icon.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    icon_transparent_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
