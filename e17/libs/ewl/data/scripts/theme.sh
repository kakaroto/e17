#!/bin/sh

# Specify the default theme db
DB="./theme.db"

echo -n "Regenerating default theme.db"

# Add keys for boxes
echo -n "."
edb_ed $DB add "/box/horizontal/base" str "appearance/button/default/base.bits.db"
edb_ed $DB add "/box/horizontal/base/visible" str "no"
edb_ed $DB add "/box/vertical/base" str "appearance/button/default/base.bits.db"
edb_ed $DB add "/box/vertical/base/visible" str "no"

# Add keys for buttons
echo -n "."
edb_ed $DB add "/button/default/base" str "appearance/button/default/base.bits.db"
edb_ed $DB add "/button/default/base/visible" str "yes"
edb_ed $DB add "/button/default/label/font" str "nationff"
edb_ed $DB add "/button/default/label/font_size" int 12
edb_ed $DB add "/button/default/label/style" str "shadow"

edb_ed $DB add "/check/base" str "appearance/button/check/base.bits.db"
edb_ed $DB add "/check/base/visible" str "yes"

edb_ed $DB add "/button/check/base" str "appearance/button/check/base.bits.db"
edb_ed $DB add "/button/check/base/visible" str "no"

edb_ed $DB add "/button/check/label/font" str "nationff"
edb_ed $DB add "/button/check/label/font_size" int 12
edb_ed $DB add "/button/check/label/style" str "shadow"

edb_ed $DB add "/radio/base" str "appearance/button/radio/base.bits.db"
edb_ed $DB add "/radio/base/visible" str "yes"

edb_ed $DB add "/button/radio/base" str "appearance/button/radio/base.bits.db"
edb_ed $DB add "/button/radio/base/visible" str "no"

edb_ed $DB add "/button/radio/label/font" str "nationff"
edb_ed $DB add "/button/radio/label/font_size" int 12
edb_ed $DB add "/button/radio/label/style" str "shadow"

# Add keys for text entry widgets
echo -n "."
edb_ed $DB add "/cursor/default/base" str "appearance/cursor/default/base.bits.db"
edb_ed $DB add "/cursor/default/base/visible" str "yes"
edb_ed $DB add "/selection/default/base" str "appearance/selection/default/base.bits.db"
edb_ed $DB add "/selection/default/base/visible" str "yes"

edb_ed $DB add "/entry/default/base" str "appearance/entry/default/base.bits.db"
edb_ed $DB add "/entry/default/base/visible" str "yes"
edb_ed $DB add "/entry/default/text/font" str "nationff"
edb_ed $DB add "/entry/default/text/font_size" int 12
edb_ed $DB add "/entry/default/text/style" str "shadow"

# Add keys for image widget
echo -n "."
edb_ed $DB add "/image/default/404" str "appearance/image/default/404.bits.db"
edb_ed $DB add "/image/default/404/visible" str "yes"

# Add keys for list widget
echo -n "."
edb_ed $DB add "/list/default/base" str "appearance/list/default/base.bits.db"
edb_ed $DB add "/list/default/base/visible" str "yes"
edb_ed $DB add "/list/marker/base" str "appearance/list/marker/base.bits.db"
edb_ed $DB add "/list/marker/base/visible" str "yes"

# Add keys for notebook widget
echo -n "."
edb_ed $DB add "/notebook/default/base" str "appearance/notebook/default/base.bits.db"
edb_ed $DB add "/notebook/default/base/visible" str "yes"
edb_ed $DB add "/notebook/content_box/base" str "appearance/notebook/content_box/base.bits.db"
edb_ed $DB add "/notebook/content_box/base/visible" str "no"
edb_ed $DB add "/notebook/tab_box/base" str "appearance/notebook/tab_box/base.bits.db"
edb_ed $DB add "/notebook/tab_box/base/visible" str "no"
edb_ed $DB add "/notebook/tab_button/base" str "appearance/notebook/tab_button/base-top.bits.db"
edb_ed $DB add "/notebook/tab_button/base/visible" str "yes"
edb_ed $DB add "/notebook/tab_button/label/font" str "nationff"
edb_ed $DB add "/notebook/tab_button/label/font_size" int 12
edb_ed $DB add "/notebook/tab_button/label/style" str "shadow"


# Add keys for seeker widget
echo -n "."
edb_ed $DB add "/seeker/horizontal/base" str "appearance/seeker/horizontal/base.bits.db"
edb_ed $DB add "/seeker/horizontal/base/visible" str "yes"
edb_ed $DB add "/seeker/horizontal/dragbar/base" str "appearance/seeker/horizontal/dragbar/base.bits.db"
edb_ed $DB add "/seeker/horizontal/dragbar/base/visible" str "yes"

edb_ed $DB add "/seeker/vertical/base" str "appearance/seeker/vertical/base.bits.db"
edb_ed $DB add "/seeker/vertical/base/visible" str "yes"
edb_ed $DB add "/seeker/vertical/dragbar/base" str "appearance/seeker/vertical/dragbar/base.bits.db"
edb_ed $DB add "/seeker/vertical/dragbar/base/visible" str "yes"

# Add keys for scrollbar widget
echo -n "."
edb_ed $DB add "/scrollbar/horizontal/base" str "appearance/scrollbar/horizontal/base.bits.db"
edb_ed $DB add "/scrollbar/horizontal/base/visible" str "yes"
edb_ed $DB add "/scrollbar/horizontal/dragbar/base" str "appearance/scrollbar/horizontal/dragbar/base.bits.db"
edb_ed $DB add "/scrollbar/horizontal/dragbar/base/visible" str "yes"
edb_ed $DB add "/scrollbar/horizontal/button_increment/base" str "appearance/scrollbar/horizontal/button_increment/base.bits.db"
edb_ed $DB add "/scrollbar/horizontal/button_increment/base/visible" str "yes"
edb_ed $DB add "/scrollbar/horizontal/button_decrement/base" str "appearance/scrollbar/horizontal/button_decrement/base.bits.db"
edb_ed $DB add "/scrollbar/horizontal/button_decrement/base/visible" str "yes"

edb_ed $DB add "/scrollbar/vertical/base" str "appearance/scrollbar/vertical/base.bits.db"
edb_ed $DB add "/scrollbar/vertical/base/visible" str "yes"
edb_ed $DB add "/scrollbar/vertical/dragbar/base" str "appearance/scrollbar/vertical/dragbar/base.bits.db"
edb_ed $DB add "/scrollbar/vertical/dragbar/base/visible" str "yes"
edb_ed $DB add "/scrollbar/vertical/button_increment/base" str "appearance/scrollbar/vertical/button_increment/base.bits.db"
edb_ed $DB add "/scrollbar/vertical/button_increment/base/visible" str "yes"
edb_ed $DB add "/scrollbar/vertical/button_decrement/base" str "appearance/scrollbar/vertical/button_decrement/base.bits.db"
edb_ed $DB add "/scrollbar/vertical/button_decrement/base/visible" str "yes"

# Add keys for separator widget
echo -n "."
edb_ed $DB add "/separator/horizontal/base" str "appearance/separator/horizontal/base.bits.db"
edb_ed $DB add "/separator/horizontal/base/visible" str "yes"

edb_ed $DB add "/separator/vertical/base" str "appearance/separator/vertical/base.bits.db"
edb_ed $DB add "/separator/vertical/base/visible" str "yes"

# Add keys for table widget
echo -n "."
edb_ed $DB add "/table/default/base" str "appearance/button/default/base.bits.db"
edb_ed $DB add "/table/default/base/visible" str "no"

# Add keys for text widget
echo -n "."
edb_ed $DB add "/text/default/font" str "nationff"
edb_ed $DB add "/text/default/font_size" int 12
edb_ed $DB add "/text/default/style" str "shadow"

# Add keys for the textarea widget
echo -n "."
edb_ed $DB add "/textarea/default/base" str "appearance/textarea/default/base.bits.db"
edb_ed $DB add "/textarea/default/base/visible" str "yes"
edb_ed $DB add "/textarea/default/base/style" str "shadow"
edb_ed $DB add "/textarea/default/base/r" int 255
edb_ed $DB add "/textarea/default/base/g" int 255
edb_ed $DB add "/textarea/default/base/b" int 255
edb_ed $DB add "/textarea/default/base/a" int 255


# Add keys for window widget
echo -n "."
edb_ed $DB add "/window/default/base" str "appearance/window/default/base.bits.db"
edb_ed $DB add "/window/default/base/visible" str "yes"

# Setup author, licence and theme name.
echo -n "."
edb_ed $DB add "/theme/author" str "Christopher 'smugg' Rosendahl and Nathan 'RbdPngn' Ingersoll"
edb_ed $DB add "/theme/licence" str "General Public Licence"
edb_ed $DB add "/theme/name" str "Default"

# Add keys for floater widget
echo -n "."
edb_ed $DB add "/floater/base" str "appearance/box/horizontal/base.bits.db"
edb_ed $DB add "/floater/base/visible" str "yes"

echo
echo "Theme regeneration complete."
