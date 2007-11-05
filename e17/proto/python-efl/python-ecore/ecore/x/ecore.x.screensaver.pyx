__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def event_available_get():
    return bool(ecore_x_screensaver_event_available_get())

def idle_time_get():
    """Get the number of seconds since the last input was received.

    @rtype: int
    """
    return ecore_x_screensaver_idle_time_get()

def screensaver_set(int timeout, int interval, int prefer_blanking,
                    int allow_exposures):
    """Configure screensaver.

    @parm timeout: how long to wait until screen saver turns on (in seconds).
    @parm interval: time between screen saver alterations (in seconds).
    @parm prefer_blanking: True to enable blank, False to disable.
    @parm allow_exposures: if to allow expose generation event or not.
    """
    ecore_x_screensaver_set(timeout, interval, prefer_blanking,
                            allow_exposures)

def timeout_set(int timeout):
    ecore_x_screensaver_timeout_set(timeout)

def timeout_get():
    return ecore_x_screensaver_timeout_get()

def interval_set(int timeout):
    ecore_x_screensaver_interval_set(timeout)

def interval_get():
    return ecore_x_screensaver_interval_get()

def blank_set(int timeout):
    ecore_x_screensaver_blank_set(timeout)

def blank_get():
    return bool(ecore_x_screensaver_blank_get())

def expose_set(int timeout):
    ecore_x_screensaver_expose_set(timeout)

def expose_get():
    return bool(ecore_x_screensaver_expose_get())

def event_listen_set(int on):
    ecore_x_screensaver_event_listen_set(on)
