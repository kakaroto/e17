import ecore.evas
import efl_utils.animations as animations

ee = ecore.evas.SoftwareX11(w=600, h=600)
ee.title = "Transition test: Slide"

canvas = ee.evas

bg = canvas.Rectangle(color="#000000", name="bg")
bg.size = canvas.size
bg.show()
ee.data["bg"] = bg


o1 = canvas.Rectangle(color="#ff0000", name="o1")
o1.show()

def change_color(color, obj):
    r, g, b = (int(x) for x in color)
    obj.color_set(r, g, b, 255)

ta = animations.TimelineAnimation((255, 0, 255), (0, 255, 0), 3.0,
                                  change_color, o1)


def cb_on_resize(ee):
    ee.data["bg"].size = ee.evas.size
ee.callback_resize = cb_on_resize

ee.show()
ecore.main_loop_begin()

