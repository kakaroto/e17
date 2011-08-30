# Copyright (C) 2010 Samsung Electronics.
#
# This file is part of Editje.
#
# Editje is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Editje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje. If not, see <http://www.gnu.org/licenses/>.

import evas
import elementary

import sysconfig
from floater import Wizard


class About(Wizard):
    def __init__(self, parent):
        Wizard.__init__(self, parent)

        self.page_add("default", "About Editje")

        ico = elementary.Icon(self)
        ico.file_set(sysconfig.theme_file_get("default"), "editje/icon/editje")
        ico.size_hint_align_set(evas.EVAS_HINT_FILL,
                                evas.EVAS_HINT_FILL)
        ico.size_hint_min_set(128, 128)
        ico.size_hint_max_set(128, 128)
        ico.show()
        self.content_add("default", ico)

        name = elementary.Label(self)
        name.text_set("<b>Editje " + str(sysconfig.VERSION) + "</b>")
        name.show()
        self.content_add("default", name)

        name = elementary.Label(self)
        name.text_set("Copyright (C) 2009-2010")
        name.show()
        self.content_add("default", name)

        name = elementary.Label(self)
        name.text_set("Samsung Electronics.")
        name.show()
        self.content_add("default", name)

        url = elementary.Entry(self)
        url.entry_set("http://trac.enlightenment.org/e/wiki/Editje")
        url.editable_set(False)
        url.show()
        self.content_add("default", url)

        sc = elementary.Scroller(self)
        sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        sc.size_hint_align_set(evas.EVAS_HINT_FILL,
                               evas.EVAS_HINT_FILL)
        sc.bounce_set(False, False)
        sc.show()
        self.content_add("default", sc)

        copyright = elementary.Label(self)
        copyright.text_set(
"Editje is free software: you can redistribute it and/or modify it <br>"
"under the terms of the GNU Lesser General Public License as       <br>"
"published by the Free Software Foundation, either version 3 of the<br>"
"License, or (at your option) any later version.                   <br>"
"                                                                  <br>"
"Editje is distributed in the hope that it will be useful,         <br>"
"but WITHOUT ANY WARRANTY; without even the implied warranty of    <br>"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      <br>"
"GNU Lesser General Public License for more details.               <br>"
"                                                                  <br>"
"You should have received a copy of the GNU Lesser General Public  <br>"
"License along with Editje. If not, see                            <br>"
"http://www.gnu.org/licenses/                                      <br>")
        copyright.show()
        sc.content_set(copyright)

        self.action_add("default", "Close", self.close, key="Escape")
