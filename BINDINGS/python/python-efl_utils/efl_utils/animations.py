import ecore
import math

__doc__ = """
Animation utilities.

@todo: get this merged in ecore.
"""

# TODO: this should be in ecore
class TimelineAnimation(object):
    """Animation with limited duration.

    This class makes easier to transition from one value (or set of values)
    to another in a given time.
    """
    def __init__(self, start, end, duration, callback, *args, **kargs):
        """Constructor.

        @param start: initial value (or list/tuple of values).
        @param end: final value (or list/tuple of values).
        @param duration: in seconds.
        @param callback: function to use at every animation tick.
           Signature: C{function(value, *args, **kargs)}, value will be a tuple
           if tuple were given as B{start} and B{end}.
        """
        self.duration = duration
        self.time_start = ecore.time_get()
        if isinstance(start, (tuple, list)) and \
               isinstance(end, (tuple, list)):
            self.value_start = tuple(start)
            self.value_range = tuple((e - s) for s, e in zip(start, end))
        else:
            self.value_start = start
            self.value_range = end - start
        self.func = callback
        self.args = args
        self.kargs = kargs
        self.anim = None
        self._run()

    def _run(self):
        def animate():
            t = ecore.time_get()
            progress = (t - self.time_start) / self.duration
            if progress >= 1.0:
                progress = 1.0
                self.anim = None
            self._exec(progress)
            return progress < 1.0

        self.anim = ecore.animator_add(animate)

    def _exec(self, progress):
        if isinstance(self.value_start, tuple):
            val = tuple((progress * r + s) for r, s in
                        zip(self.value_range, self.value_start))
        else:
            val = progress * self.value_range + self.value_start
        self.func(val, *self.args, **self.kargs)

    def delete(self):
        """Stop and delete the animation.

        @note: this will call the provide function as if it reached the final
           time (progress being 1.0, value being B{end}).
        """
        if self.anim:
            self.anim.delete()
            self.anim = None
        self._exec(1.0)

    def stop(self):
        "Same as L{delete()}"
        self.delete()


class SinusoidalTimelineAnimation(TimelineAnimation):
    __slots__ = ("cos", "pi")
    cos = math.cos
    pi = math.pi

    def _exec(self, progress):
        if progress < 1.0:
            p = (1.0 - self.cos(progress * self.pi)) / 2.0
        else:
            p = 1.0
        TimelineAnimation._exec(self, p)


class DecelerateTimelineAnimation(TimelineAnimation):
    __slots__ = ("sin", "pi2")
    sin = math.sin
    pi2 = math.pi / 2

    def _exec(self, progress):
        if progress < 1.0:
            p = self.sin(progress * self.pi2)
        else:
            p = 1.0
        TimelineAnimation._exec(self, p)


class AccelerateTimelineAnimation(TimelineAnimation):
    __slots__ = ("sin", "pi2")
    sin = math.sin
    pi2 = math.pi / 2

    def _exec(self, progress):
        if progress < 1.0:
            p = 1.0 - self.sin(self.pi2 + (progress * self.pi2))
        else:
            p = 1.0
        TimelineAnimation._exec(self, p)
