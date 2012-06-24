#!/usr/bin/env python
# encoding: utf-8
import elementary

def config_clicked(obj):
    # TODO: Write a better test!
    c = elementary.Configuration()
    for i in (c.engine, c.finger_size, c.mirrored, c.profile_list, c.font_overlay_list, c.text_classes_list):
        print(i)
    print((elementary.config_finger_size_get()))

if __name__ == "__main__":
    elementary.init()

    config_clicked(None)

    elementary.run()
    elementary.shutdown()
