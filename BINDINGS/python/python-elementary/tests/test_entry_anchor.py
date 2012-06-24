#!/usr/bin/python
# encoding: utf-8
import elementary
import edje
import ecore
import evas

class Test(Window):
    def __init__(self):
        Window.__init__(self, "entry-test", ELM_WIN_BASIC)
        
        self.title_set("Entry test")
        self.callback_delete_request_add(self.quit)
        
        bg = Background(self)
        bg.size_hint_weight_set(1.0, 1.0)
        self.resize_object_add(bg)
        bg.show()
        
        box = Box(self)
        box.size_hint_weight_set(1.0, 1.0)
        self.resize_object_add(box)
        
        entry = Entry(self)
        entry.text_set("<a href=url:http://www.enlightenment.org/>Enlightenment</a>")
        entry.callback_anchor_clicked_add(self.anchor_clicked)
        entry.anchor_hover_style_set("popout")
        entry.anchor_hover_parent_set(self)
        entry.callback_anchor_hover_opened_add(self.anchor_hover_opened)
        entry.show()
        
        frame = Frame(self)
        frame.size_hint_align_set(-1.0, -1.0)
        frame.text_set("Entry test")
        frame.content_set(entry)
        frame.show()
        
        box.pack_end(frame)
        box.show()
        
        self.resize(400, 400)
        self.show()
        
    def anchor_clicked(self, obj, event_info):
        print(("Entry object is %s" % (obj)))
        print(("We should have EntryAnchorInfo here: %s" % (str(event_info))))
        print(("EntryAnchorInfo has the following properties and methods: %s" % (dir(event_info))))
        
    def anchor_hover_opened(self, obj, event_info):
        print(("We should have EntryAnchorHoverInfo here: %s" % (event_info)))
        print(("EntryAnchorHoverInfo has the following properties and methods: %s" % (dir(event_info))))
        btn = Button(obj)
        btn.text_set("Testing entry anchor")
        event_info.hover.content_set("middle", btn)
        btn.show()
        
    def quit(self, *args):
        exit()

if __name__ == "__main__":
    init()
    test = Test()
    run()
    shutdown()
