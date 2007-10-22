__callbacks = (
    "mouse_in",
    "mouse_out",
    "mouse_down",
    "mouse_up",
    "mouse_move",
    "mouse_wheel",
    "free",
    "del",
    "key_down",
    "key_up",
    "focus_in",
    "focus_out",
    "show",
    "hide",
    "move",
    "resize",
    "restack",
    )


def __get_callback(observer, name):
    try:
        attr = getattr(observer, "cb_on_%s" % name)
        if callable(attr):
            return attr
    except AttributeError, e:
        return None


def connect_observer(evas, observer):
    """Connect methods from observer to Evas callbacks.

    Observer must have methods with name scheme: C{cb_on_<callback>},
    examples:
     - cb_on_resize
     - cb_on_move
     - cb_on_show
    """
    for cb_name in __callbacks:
        cb = __get_callback(observer, cb_name)
        if cb:
            setter = getattr(evas, "on_%s_add" % cb_name)
            setter(cb)


def disconnect_observer(evas, observer):
    """Disconnect observer connected using connect_observer()"""
    for cb_name in __callbacks:
        cb = __get_callback(observer, cb_name)
        if cb:
            unsetter = getattr(evas, "on_%s_del" % cb_name)
            unsetter(cb)


def connect_callbacks_by_name(evas, mapping):
    """Connect callbacks specified in mapping to Evas callbacks.

    Mapping must be a dict or a list of tuples with callback name and
    desired function, example:
     - mapping = C{(("resize", my_on_resize), ("show", my_on_show))}
     - mapping = C{{"resize": my_on_resize, "show": my_on_show}}
    """
    if isinstance(mapping, dict):
        mapping = mapping.iteritems()
    for name, func in mapping:
        try:
            setter = getattr(evas, "on_%s_add" % name)
        except AttributeError, e:
            raise ValueError("invalid callback name: %s" % name)
        setter(func)

def disconnect_callbacks_by_name(evas, mapping):
    """Disconnect callbacks specified in mapping to Evas callbacks."""
    if isinstance(mapping, dict):
        mapping = mapping.iteritems()
    for name, func in mapping:
        try:
            unsetter = getattr(evas, "on_%s_del" % name)
        except AttributeError, e:
            raise ValueError("invalid callback name: %s" % name)
        unsetter(func)
