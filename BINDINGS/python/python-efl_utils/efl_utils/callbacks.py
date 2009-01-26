def generate_multiple_wait(n_wait, end_callback):
    """Generate a callback that will call end_callback after it's called n_wait
       times.

       Signature: C{function(return_values)}

       where return_values is a list of tuples with (args, kargs) given to
       the generated callback.
    """
    finished = []
    def cb(*args, **kargs):
        finished.append((args, kargs))
        if len(finished) == n_wait:
            end_callback(finished)
    return cb
