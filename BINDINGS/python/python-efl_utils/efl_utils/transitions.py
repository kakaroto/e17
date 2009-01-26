import evas
import ecore

__doc__ = """
Transition effects.

Usually you will create a Container and change it's child with
Container.transition_to() method, giving one effect as parameter,
where effects is one subclass of ContainerEffect, like:
 - L{ContainerEffectFade}
 - L{ContainerEffectSlide}
 - L{ContainerEffectFunction}

ContainerEffectFunction takes functions to do more specific
operations, these can be even mixed with create_composite_effect().
There is one pre-defined function, effect_alpha(), and also some
function factories:
 - L{create_effect_slide()}
 - L{create_effect_scale()}
 - L{create_composite_effect()}

"""


__all__ = (
    "ContainerEffectFade",
    "ContainerEffectSlide",
    "ContainerEffectFunction",
    "Container",
    "effect_alpha",
    "create_effect_slide",
    "create_effect_scale",
    "create_composite_effect",
    )


class Cell(evas.ClippedSmartObject):
    """Smart object to manipulate child contents.

    The purpose of this class is to manipulate child color without
    interfering it's original, this is done by having a clip for this
    object (via ClippedSmartObject).
    """
    def __init__(self, canvas, child):
        evas.ClippedSmartObject.__init__(self, canvas)
        self._child = None
        self.child_set(child)

    def child_set(self, child):
        if self._child:
            self._child.hide()
            self.member_del(self._child)
            if self.parent:
                self.parent.member_add(self._child)
                self.parent.member_del(self)
        self._child = child
        if child:
            parent = child.parent
            if parent:
               parent.member_add(self)

            self.member_add(child)
            child.geometry = self.geometry
            child.show()

    def resize(self, w, h):
        if self._child:
            self._child.resize(w, h)


class ContainerEffect(object):
    def __init__(self):
        self.anim = None
        self.container = None
        self.end_callback = None

    def run(self, container, end_callback=None):
        self.container = container
        self.end_callback = end_callback

    def delete(self):
        if self.anim:
            self.anim.delete()
            self.anim = None

    def stop(self):
        self.delete()


class ContainerEffectTimeline(ContainerEffect):
    def __init__(self, duration=None, delay_in=0.0, duration_in=None,
                 delay_out=0.0, duration_out=None):
        """Base for effects based on time.

        This class provides facilities to do effects based on time, more
        specifically time to enter ("in"), and exit ("out").

        Time unit is seconds.

        @param duration: how long the whole animation will take.
        @param delay_in: delay to bring in/enter the new object.
        @param duration_in: how long the "bring in"/entrance will take.
        @param delay_out: delay to exit the current object.
        @param duration_out: how long the exit of current object will take.

        If either (delay_in + duration_in) or (delay_out + duration_out) are
        greater than duration, the greatest value will be used as duration.

        If either duration_in or duration_out are omitted, they will be
        calculated as (duration - delay_in) and (duration - delay_out),
        respectively.

        Examples:

         - Animation that takes 10 seconds, start to bring in the new object
           at the same time the current object exits:

           >>> ContainerEffectTimeline(duration=10.0)

         - Animation that takes 10 seconds, but first the current object
           exits (in 4 seconds), then the new object enters (in 6 seconds):

           >>> ContainerEffectTimeline(delay_out=0.0, duration_out=4.0,
           ...                         delay_in=4.0, duration_in=6.0)

        Subclasses should implement the following methods:
         - setup(): called before the effect is run().
         - tear_down(): called after the effect finishes.
         - animate(time, time_in, time_out): called on each iteration with
           current elapsed time, progress_in and progress_out. Both
           progress_in and progress_out will be based on elapsed time, so
           they can be less than 0.0 or greater than 1.0
        """
        ContainerEffect.__init__(self)

        if duration is not None:
            if duration_in is None:
                duration_in = duration - delay_in
            if duration_out is None:
                duration_out = duration - delay_out
        else:
            if duration_out is None or duration_in is None:
                raise ValueError("Must specify duration or (duration_in and "
                                 "duration_out)")
            duration = max(duration_in + delay_in,
                           duration_out + delay_out)

        if duration <= 0.0:
            raise ValueError("duration <= 0.0")
        if duration_out <= 0.0:
            raise ValueError("duration_out <= 0.0")
        if duration_in <= 0.0:
            raise ValueError("duration_in <= 0.0")
        if delay_out < 0.0:
            raise ValueError("delay_out < 0.0")
        if delay_in < 0.0:
            raise ValueError("delay_in < 0.0")

        self.t0_in = float(delay_in)
        self.t1_in = float(delay_in + duration_in)
        self.duration_in = float(duration_in)
        self.t0_out = float(delay_out)
        self.t1_out = float(delay_out + duration_out)
        self.duration_out = float(duration_out)
        self.duration = max(float(duration), self.t1_in, self.t1_out)


    def setup(self):
        pass

    def tear_down(self):
        pass

    def animate(self, time, progress_in, progress_out):
        raise NotImplementedError("implement animate()")

    def __animate(self, start_time):
        time = ecore.time_get() - start_time
        if time > self.duration:
            time = self.duration
            last_run = True
        else:
            last_run = False

        p_in = (time - self.t0_in) / self.duration_in
        p_out = (time - self.t0_out) / self.duration_out
        self.animate(time, p_in, p_out)

        if time == self.duration or last_run:
            self.tear_down()
            if self.end_callback:
                self.end_callback(self, self.container)
            self.anim = None

        return not last_run

    def run(self, container, end_callback=None):
        ContainerEffect.run(self, container, end_callback)

        self.setup()
        self.anim = ecore.animator_add(self.__animate, ecore.time_get())


class ContainerEffectFade(ContainerEffectTimeline):
    """Effect to fade out current object and fade in newer."""
    def _add_clip_to(self, obj):
        canvas = obj.evas
        clipper = canvas.Rectangle()
        obj.parent.member_add(clipper)
        clipper.size = canvas.size
        clipper.clip = obj.clip
        obj.clip = clipper
        clipper.show()
        return clipper

    def setup(self):
        current = self.container.current
        if current:
            self.current_clip = self._add_clip_to(current)
        else:
            self.current_clip = None

        new = self.container.new
        if new:
            self.new_clip = self._add_clip_to(new)
            self.new_clip.color_set(0, 0, 0, 0)
        else:
            self.new_clip = None

    def tear_down(self):
        if self.current_clip:
            self.container.current_clip_set(self.current_clip.clip)
            self.current_clip.delete()
            self.current_clip = None
        if self.new_clip:
            self.container.new_clip_set(self.new_clip.clip)
            self.new_clip.delete()
            self.new_clip = None

    def animate(self, time, progress_in, progress_out):
        if 0.0 <= progress_in <= 1.0 and self.new_clip:
            color = int(255 * progress_in)
            self.new_clip.color_set(color, color, color, color)

        if 0.0 <= progress_out <= 1.0 and self.current_clip:
            color = 255 - int(255 * progress_out)
            self.current_clip.color_set(color, color, color, color)


class ContainerEffectSlide(ContainerEffectTimeline):
    """Effect to slide out current object and slide newer."""
    def __init__(self, horizontal=1, vertical=0, duration=None,
                 delay_in=0.0, duration_in=None,
                 delay_out=0.0, duration_out=None):
        ContainerEffectTimeline.__init__(self, duration=duration,
                                         delay_in=delay_in,
                                         duration_in=duration_in,
                                         delay_out=delay_out,
                                         duration_out=duration_out)
        if not vertical and not horizontal:
            raise ValueError("You must specify horizontal or vertical")
        self.horizontal = horizontal
        self.vertical = vertical

    def setup(self):
        x, y, w, h = self.container.geometry
        nw = w * self.horizontal
        nh = h * self.vertical
        self.container.new_move(x + nw, y + nh)
        self.container.current_move(x, y)

    def tear_down(self):
        x, y = self.container.pos
        self.container.current_move(x, y)
        self.container.new_move(x, y)

    def animate(self, time, progress_in, progress_out):
        c = self.container
        x, y, w, h = c.geometry
        if 0.0 <= progress_in <= 1.0:
            p = 1.0 - progress_in
            nw = int(w * self.horizontal * p)
            nh = int(h * self.vertical * p)
            c.new_move(x + nw, y + nh)

        if 0.0 <= progress_out <= 1.0:
            nw = int(w * -self.horizontal * progress_out)
            nh = int(h * -self.vertical * progress_out)
            c.current_move(x + nw, y + nh)


class ContainerEffectFunction(ContainerEffectTimeline):
    """Effect that calls a function to change the state of objects."""
    def __init__(self, func, duration=None,
                 delay_in=0.0, duration_in=None,
                 delay_out=0.0, duration_out=None):
        """Effect that calls a function to change the state of objects.

        @param func: what to callback on each iteration. It's signature is:
                      func(time, progress_in, progress_out, current, new)
        """
        ContainerEffectTimeline.__init__(self, duration=duration,
                                         delay_in=delay_in,
                                         duration_in=duration_in,
                                         delay_out=delay_out,
                                         duration_out=duration_out)
        self.func = func

    def _add_cell_to(self, obj):
        clipper = obj.clip
        geometry = obj.geometry
        cell = Cell(obj.evas, obj)
        cell.geometry = geometry
        cell.clip = clipper
        cell.show()
        return cell

    def setup(self):
        current = self.container.current
        if current:
            self.current_cell = self._add_cell_to(current)
        else:
            self.current_cell = None

        new = self.container.new
        if new:
            self.new_cell = self._add_cell_to(new)
        else:
            self.new_cell = None

    def tear_down(self):
        x, y, w, h = self.container.geometry
        if self.current_cell:
            self.container.current.clip_set(self.current_cell.clip)
            self.container.current.geometry_set(x, y, w, h)
            self.current_cell.child_set(None)
            self.current_cell.delete()
            self.current_cell = None
        if self.new_cell:
            self.container.new_clip_set(self.new_cell.clip)
            self.container.new.geometry_set(x, y, w, h)
            self.new_cell.child_set(None)
            self.new_cell.delete()
            self.new_cell = None

    def animate(self, time, progress_in, progress_out):
        self.func(self, time, progress_in, progress_out,
                  self.current_cell, self.new_cell)


class Container(evas.SmartObject):
    """Smart object that do effect transitions to change child object.

    This class provides transition_to() method that will do animation
    or any effect in order to change current child with the provided one.
    """
    def __init__(self, canvas, child):
        self._current = None
        self._new = None
        evas.SmartObject.__init__(self, canvas)
        self.current_set(child)

    def _remove_child(self, child):
        if child is None:
            return
        child.hide()
        child.clip_unset()
        self.member_del(child)

    def _add_child(self, child):
        if child is None:
            return
        child.geometry = self.geometry
        child.clip = self.clip
        self.member_add(child)

    def current_set(self, child):
        self._remove_child(self._current)
        self._add_child(child)
        self._current = child
        if child:
            child.show()

    def current_get(self):
        return self._current

    current = property(current_get, current_set)

    def new_set(self, child):
        self._remove_child(self._new)
        self._add_child(child)
        self._new = child
        if child:
            child.hide()

    def new_get(self):
        return self._new

    new = property(new_get, new_set)

    def transition_to(self, new_child, effect, end_callback=None):
        """Replace current child with new_child, executing effect
        and calling end_callback when it finishes.
        """
        self.new_set(new_child)
        self.new_show()
        def on_end(effect, container):
            self.new_set(None)
            self.current_set(new_child)
            if end_callback:
                end_callback(self, new_child, effect)

        effect.run(self, on_end)

    # -- SmartObject API with individual setters to current/new ---------
    def current_color_set(self, r, g, b, a):
        if self._current:
            self._current.color_set(r, g, b, a)

    def new_color_set(self, r, g, b, a):
        if self._new:
            self._new.color_set(r, g, b, a)

    def color_set(self, r, g, b, a):
        self.current_color_set(r, g, b, a)
        self.new_color_set(r, g, b, a)

    def current_clip_set(self, clip):
        if self._current:
            self._current.clip_set(clip)

    def new_clip_set(self, clip):
        if self._new:
            self._new.clip_set(clip)

    def clip_set(self, clip):
        self.current_clip_set(clip)
        self.new_clip_set(clip)

    def current_clip_unset(self):
        if self._current:
            self._current.clip_unset()

    def new_clip_unset(self):
        if self._new:
            self._new.clip_unset()

    def clip_unset(self):
        self.current_clip_unset()
        self.new_clip_unset()

    def current_resize(self, w, h):
        if self._current:
            self._current.resize(w, h)

    def new_resize(self, w, h):
        if self._new:
            self._new.resize(w, h)

    def resize(self, w, h):
        self.current_resize(w, h)
        self.new_resize(w, h)

    def current_move(self, x, y):
        if self._current:
            self._current.move(x, y)

    def new_move(self, x, y):
        if self._new:
            self._new.move(x, y)

    def move(self, x, y):
        self.current_move(x, y)
        self.new_move(x, y)

    def current_show(self):
        if self._current:
            self._current.show()

    def new_show(self):
        if self._new:
            self._new.show()

    def show(self):
        self.current_show()

    def current_hide(self):
        if self._current:
            self._current.hide()

    def new_hide(self):
        if self._new:
            self._new.hide()

    def hide(self):
        self.current_hide()
        self.new_hide()


def effect_alpha(effect, time, progress_in, progress_out, current, new):
    """Effect function to fade out current object and fade in newer.
    """
    if 0.0 <= progress_in <= 1.0 and new:
        c = int(255 * progress_in)
        new.color_set(c, c, c, c)

    if 0.0 <= progress_out <= 1.0 and current:
        c = 255 - int(255 * progress_out)
        current.color_set(c, c, c, c)


def create_effect_slide(horizontal=-1, vertical=0):
    """Create an effect function to slide objects.

    This can be used to change objects by slide effect, new object
    can come from left and current object exit to the right, for example.

    Examples:

     - Effect to slide from left:

       >>> effect_func = create_effect_slide(-1, 0)

     - Effect to slide from bottom:

       >>> effect_func = create_effect_slide(0, 1)

     - Effect to slide from right-top:

       >>> effect_func = create_effect_slide(1, -1)


    @param horizontal: scale based on width to consider as initial position
           of entering object. Its final position will always be 0.
    @param vertical: scale based on height to consider as initial position
           of entering object. Its final position will always be 0.

    @return: new effect function to be used in ContainerEffectFunction.
    """
    def effect_slide(effect, time, progress_in, progress_out, current, new):
        x, y, w, h = effect.container.geometry
        if 0.0 <= progress_in <= 1.0 and new:
            p = 1.0 - progress_in
            nw = int(w * horizontal * p)
            nh = int(h * vertical * p)
            new.move(x + nw, y + nh)

        if 0.0 <= progress_out <= 1.0 and current:
            nw = int(w * -horizontal * progress_out)
            nh = int(h * -vertical * progress_out)
            current.move(x + nw, y + nh)

    return effect_slide

def create_effect_scale(horizontal_in=0.0, vertical_in=0.0,
                        horizontal_out=0.0, vertical_out=0.0,
                        center_x=True, center_y=True):
    """Create an effect function to scale objects.

    Object entering will start with size scaled by (horizontal_in,
    vertical_in) and end with full size. That is, if these values are
    0.5, it will start half size and then grow to full size (1.0, 1.0).

    Object exiting will start with full size and end with
    (horizontal_out, vertical_out). If these values are 0.5, object will
    start effect with full size and end with half size.

    @param horizontal_in: initial horizontal scale for entering object.
    @param vertical_in: initial vertical scale for entering object.
    @param horizontal_out: final horizontal scale for exiting object.
    @param vertical_out: final vertical scale for exiting object.
    @param center_x: if objects should remain horizontally centered,
           otherwise its left side will remain.
    @param center_y: if objects should remain vertically centered,
           otherwise its top side will remain.
    @return: new effect function to be used in ContainerEffectFunction.
    """
    def effect_scale(effect, time, progress_in, progress_out, current, new):
        w, h = effect.container.size
        if 0.0 <= progress_in <= 1.0 and new:
            x, y = new.center
            p = progress_in
            dw = w * horizontal_in
            dh = h * vertical_in
            nw = int((w - dw) * p + dw)
            nh = int((h - dh) * p + dh)
            new.resize(nw, nh)

            r = new.rect
            if center_x:
                r.center_x = x
            if center_y:
                r.center_y = y
            if center_x or center_y:
                new.rect = r

        if 0.0 <= progress_out <= 1.0 and current:
            x, y = current.center
            p = 1.0 - progress_out
            dw = w * horizontal_out
            dh = h * vertical_out
            nw = int((w - dw) * p + dw)
            nh = int((h - dh) * p + dh)
            current.resize(nw, nh)

            r = current.rect
            if center_x:
                r.center_x = x
            if center_y:
                r.center_y = y
            if center_x or center_y:
                current.rect = r

    return effect_scale


def create_composite_effect(*funcs):
    """Create an effect function composed by other effect functions.

    Composed effect will execute functions in given order.

    @return: new effect function to be used in ContainerEffectFunction.
    """
    def effect_mixer(effect, time, progress_in, progress_out, current, new):
        for f in funcs:
            f(effect, time, progress_in, progress_out, current, new)
    return effect_mixer
