import ecore.evas
import efl_utils.transitions as transitions

ee = ecore.evas.SoftwareX11(w=600, h=600)
ee.title = "Transition test: Fade"

canvas = ee.evas

bg = canvas.Rectangle(color="#000000", name="bg")
bg.size = canvas.size
bg.show()
ee.data["bg"] = bg


o1 = canvas.Rectangle(color="#ff0000", name="o1")
o1.show()

cc = transitions.Container(canvas, o1)
cc.name = "cc"
cc.geometry = (200, 200, 100, 100)
cc.show()

overlay = canvas.Rectangle(color=(100, 100, 100, 100),
                           geometry=cc.geometry,
                           name="overlay")
overlay.show()

cc.current_set(o1)

o2 = canvas.Rectangle(color="#0000ff", name="o2")
o2.geometry = (100, 100, 200, 200)
o2.show()

def end_callback(container, object, effect):
    print "it ended:", container, object, effect

effect = transitions.ContainerEffectFade(1.0)
cc.transition_to(o2, effect, end_callback)

def cb_on_resize(ee):
    ee.data["bg"].size = ee.evas.size
ee.callback_resize = cb_on_resize

ee.show()
ecore.main_loop_begin()

