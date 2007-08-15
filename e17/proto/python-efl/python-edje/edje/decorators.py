def signal_callback(emission, source):
    def deco(func):
        func.edje_signal_callback = (emission, source)
        return staticmethod(func)
    return deco


def text_change_callback(func):
    func.edje_text_change_callback = True
    return staticmethod(func)


def message_handler(func):
    func.edje_message_handler = True
    return staticmethod(func)
