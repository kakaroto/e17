import evas

def event_callback(event_type):
    def deco(func):
        func.evas_event_callback = event_type
        return staticmethod(func)
    return deco

def mouse_in_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_IN
    return staticmethod(func)

def mouse_out_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_OUT
    return staticmethod(func)

def mouse_down_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_DOWN
    return staticmethod(func)

def mouse_up_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_UP
    return staticmethod(func)

def mouse_move_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_MOVE
    return staticmethod(func)

def mouse_wheel_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_WHEEL
    return staticmethod(func)

def free_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_FREE
    return staticmethod(func)

def key_down_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_KEY_DOWN
    return staticmethod(func)

def key_up_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_KEY_UP
    return staticmethod(func)

def focus_in_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_FOCUS_IN
    return staticmethod(func)

def focus_out_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_FOCUS_OUT
    return staticmethod(func)

def show_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_SHOW
    return staticmethod(func)

def hide_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_HIDE
    return staticmethod(func)

def move_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOVE
    return staticmethod(func)

def resize_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_RESIZE
    return staticmethod(func)

def restack_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_RESTACK
    return staticmethod(func)
