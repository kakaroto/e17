#!/usr/bin/python

import ecore.evas
import ecore # Ecore is required to get main loop


def resize_bg_cb(ee):
    bg = ee.data["bg"]
    canvas = ee.evas
    bg.size = canvas.size


if __name__ == "__main__":
    ee = ecore.evas.SoftwareX11(w=800, h=600)

    canvas = ee.evas # get drawing area created by SoftwareX11

    # We MUST create a background otherwise we may get garbage displayed
    # since Evas does not paint any default background color/image/...
    #
    # color parameter can be:
    #   * (r, g, b, a): in pre-multiplied format
    #   * integer: in 0x[aa][rr][gg][bb], 8 bits per component
    #   * string: in 'internet' notation #[aa][rr][gg][bb], components
    #     being hexadecimal numbers.
    # used color is white (opaque).
    bg = canvas.Rectangle(color=(255, 255, 255, 255))
    # resize object to cover the whole canvas:
    bg.size = canvas.size
    # show object.
    bg.show()

    # remember "bg" for later use in 'resize_bg_cb()'
    ee.data["bg"] = bg
    # call function resize_bg_cb() when window is resized so we resize "bg"
    ee.callback_resize = resize_bg_cb

    # show X11 window created by SoftwareX11
    ee.show()

    # enter main loop and wait until window is closed
    ecore.main_loop_begin()
