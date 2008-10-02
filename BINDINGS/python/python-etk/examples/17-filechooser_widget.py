#!/usr/bin/python

import etk

# Functions called on events
def print_selected(obj):
	''' Prints out selected files '''
	if filechooser.selected_files_get() is not None:
		print "Selected Files: %s" % " ".join([file for file in filechooser.selected_files_get()])
	else:
		print "No file selected."

# Used when check buttons toggled
def chk_btns(obj, data):
	''' Activate / Disactivate multiple selection
	and Show / Hide hidden files '''
	if data == "multiple":
		filechooser.select_multiple_set(obj.active_get())
	if data == "hidden":
		filechooser.show_hidden_set(obj.active_get())
	if data == "save":
		filechooser.is_save_set(obj.active_get())

# File chooser
filechooser = etk.FilechooserWidget()

# Dialog
dialog = etk.Dialog(title="FilechooserWidget Test Python-Etk")

# Check boxes
select_multiple = etk.CheckButton (label="Select Multiple")
show_hidden = etk.CheckButton (label="Show hidden")
is_save = etk.CheckButton (label="Is save")

# Place the elements into the dialog
dialog.pack_in_main_area (filechooser, etk.VBox.START, etk.VBox.EXPAND_FILL, 0)
dialog.pack_widget_in_action_area(select_multiple, etk.VBox.START, etk.VBox.NONE, 0)
dialog.pack_widget_in_action_area(show_hidden, etk.VBox.START, etk.VBox.NONE, 0)
dialog.pack_widget_in_action_area(is_save, etk.VBox.START, etk.VBox.NONE, 0)
open = dialog.button_add_from_stock(etk.c_etk.StockEnums.DOCUMENT_OPEN, 1)
close = dialog.button_add_from_stock(etk.c_etk.StockEnums.DIALOG_CLOSE, 2)

# Events catchers
select_multiple.connect("toggled", chk_btns, "multiple")
show_hidden.connect("toggled", chk_btns, "hidden")
is_save.connect("toggled", chk_btns, "save")
open.connect("clicked", print_selected)
close.connect("clicked", lambda x: etk.main_quit())

dialog.on_destroyed(lambda x: etk.main_quit())
dialog.show_all()

etk.main()
