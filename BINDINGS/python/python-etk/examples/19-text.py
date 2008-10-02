import etk

v = etk.VBox()
h = etk.HBox()

out_tv = etk.TextView()
in_tv = etk.TextView()

out_tb = out_tv.textblock_get()
in_tb = in_tv.textblock_get()
iter_out = etk.TextblockIter(out_tb)
iter_out.forward_end()

b = etk.Button(label="Send")

#e = etk.Entry()

h.append(in_tv, etk.HBox.START, etk.HBox.EXPAND_FILL, 0)
#h.append(e, etk.HBox.START, etk.HBox.EXPAND_FILL, 0)
h.append(b, etk.HBox.END, etk.HBox.FILL, 0)
v.append(out_tv, etk.VBox.START, etk.VBox.EXPAND_FILL, 0)
v.append(h, etk.VBox.END, etk.VBox.EXPAND_FILL, 0)

w = etk.Window(title="Testing TextView!", size_request=(600, 600), child=v)
w.show_all()


def sendButton_cb(button):
    text = in_tb.text_get(0)
    in_tb.clear()
    #text = e.text
    #e.text = ""
    print text
    out_tb.insert(iter_out, text)
b.on_clicked(sendButton_cb)

def on_destroyed(obj):
    etk.main_quit()
w.connect("destroyed", on_destroyed)


etk.main()
