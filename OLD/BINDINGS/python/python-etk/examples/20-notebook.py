#!/usr/bin/python

import etk

##################################
## Create Widget for the first tab
image = etk.Image()
image.set_from_stock (etk.c_etk.StockEnums.APPLICATIONS_DEVELOPMENT, etk.c_etk.StockEnums.BIG)

buttons = [etk.Button(), etk.CheckButton(), etk.CheckButton()]
buttons[0].set_from_stock (etk.c_etk.StockEnums.DOCUMENT_OPEN)

labels = [etk.Label(text="App Name"), etk.Label(text="Generic Info"),
		etk.Label(text="Comments"), etk.Label(text="Executable"),
		etk.Label(text="Window Name"),etk.Label(text="Window Class"),
		etk.Label(text="Startup Notify"),etk.Label(text="Wait Exit")]

entries = [etk.Entry() for i in range(6)]

table = etk.Table(cols=2, rows=10, homogeneous=etk.Table.NOT_HOMOGENEOUS)
table.attach(image, 0, 0, 0, 0, etk.Table.NONE, 0, 0)
table.attach(buttons[0], 1, 1, 0, 0, etk.Table.HEXPAND, 0, 0)

for i in range(6):
	table.attach(labels[i], 0, 0, 2 + i, 2 + i, etk.Table.HFILL, 0, 0)
	table.attach_default(entries[i], 1, 1, 2 + i, 2 + i)

table.attach(labels[6], 0, 0, 8, 8, etk.Table.HFILL, 0, 0)
table.attach_default(buttons[1], 1, 1, 8, 8)
table.attach(labels[7], 0, 0, 9, 9, etk.Table.HFILL, 0, 0)
table.attach_default(buttons[2], 1, 1, 9, 9)

###################################
## Create Widget for the second tab
align = etk.Alignment(0.5, 0.5, 0.2, 0.0)

vbox = etk.VBox(False, 3)
align.add(vbox)

button_normal = etk.Button(label="Normal button")
vbox.append(button_normal, etk.VBox.START, etk.VBox.NONE, 0)

button_toggle = etk.ToggleButton(label="Toggle button")
vbox.append(button_toggle, etk.VBox.START, etk.VBox.NONE, 0)

button_check = etk.CheckButton(label="Check button 1")
vbox.append(etk.CheckButton(label="Check button 1"), etk.VBox.START, etk.VBox.NONE, 0)

button_check = etk.CheckButton(label="Check Button 2")
vbox.append(button_check, etk.VBox.START, etk.VBox.NONE, 0)

############
## Callbacks

def prev_cb(obj, note):
	note.page_prev()

def next_cb(obj, note):
	note.page_next()

def hide_tabs_toggled_cb(obj, note):
	note.tabs_visible_set(not obj.active_get())

##############
## Main Window
main_vbox = etk.VBox(False, 3)

## Create notebook and add pages
notebook = etk.Notebook()
main_vbox.append(notebook, etk.HBox.START, etk.HBox.EXPAND_FILL, 0)
notebook.page_append("First Tab", table)
notebook.page_append("Second Tab", align)

## Create the prev/next buttons and the "Hide tabs" toggle button
hbox = etk.HBox(True, 0)
main_vbox.append(hbox, etk.VBox.START, etk.VBox.SHRINK_OPPOSITE, 0)

button = etk.Button(label="Previous")
button.set_from_stock(etk.c_etk.StockEnums.GO_PREVIOUS)
button.on_clicked(prev_cb, notebook)
hbox.append(button, etk.HBox.START, etk.HBox.FILL, 0)

button = etk.Button(label="Next")
button.set_from_stock(etk.c_etk.StockEnums.GO_NEXT)
button.on_clicked(next_cb, notebook)
hbox.append(button, etk.HBox.START, etk.HBox.FILL, 0)

button = etk.ToggleButton(label="Hide tabs")
button.on_toggled(hide_tabs_toggled_cb, notebook)
hbox.append(button, etk.HBox.START, etk.HBox.FILL, 0)

w = etk.Window(title="Etk Notebook Example", size_request=(300, 300), child=main_vbox)
w.show_all()

def on_destroyed(obj):
    etk.main_quit()
w.connect("destroyed", on_destroyed)

etk.main()
