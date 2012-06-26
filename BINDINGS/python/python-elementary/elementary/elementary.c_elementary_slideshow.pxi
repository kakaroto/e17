# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

cdef _py_elm_slideshow_item_call(func, Evas_Object *obj, data) with gil:
    try:
        o = Object_from_instance(obj)
        (args, kwargs) = data
        return func(o, *args, **kwargs)
    except Exception as e:
        traceback.print_exc()
        return None

cdef Evas_Object *_py_elm_slideshow_item_get(void *data, Evas_Object *obj) with gil:
    cdef SlideshowItem item = <object>data
    cdef object params = item.params
    cdef evasObject icon
    cdef SlideshowItemClass itc = params[0]

    func = itc._get_func
    if func is None:
        return NULL

    ret = _py_elm_slideshow_item_call(func, obj, params[1])
    if ret is not None:
        try:
            icon = ret
            return icon.obj
        except Exception as e:
            traceback.print_exc()
            return NULL
    else:
        return NULL

cdef void _py_elm_slideshow_item_del(void *data, Evas_Object *obj) with gil:
    cdef SlideshowItem item = <object>data
    cdef object params = item.params
    cdef SlideshowItemClass itc = params[0]

    func = itc._del_func
    if func is not None:
        try:
            o = Object_from_instance(obj)
            func(o, params[1])
        except Exception as e:
            traceback.print_exc()
    item._unset_obj()
    Py_DECREF(item)

cdef int _py_elm_slideshow_compare_func(const_void *data1, const_void *data2) with gil:
    cdef SlideshowItem item1    = <object>data1
    cdef SlideshowItem item2    = <object>data2
    cdef object params          = item1.params
    cdef object func            = params[2]

    if func is None:
        return 0

    ret = func(item1, item2)
    if ret is not None:
        try:
            return ret
        except Exception as e:
            traceback.print_exc()
            return 0
    else:
        return 0

cdef class SlideshowItemClass (object):
    """Defines the behavior of each slideshow item.

    This class should be created and handled to the Slideshow itself.

    It may be subclassed, in this case the methods L{get()} and C{delete()}
    will be used.

    It may also be instantiated directly, given getters to override as
    constructor parameters.

    @param get_func: if provided will override the behavior
        defined by L{get()} in this class. Its purpose is
        to return the icon object to be used (swalloed) by a
        given part and row. This function should have the
        signature:
        C{func(obj, item_data) -> obj}

    @param del_func: if provided will override the behavior
        defined by C{delete()} in this class. Its purpose is to be
        called when item is deleted, thus finalizing resources
        and similar. This function should have the signature:
        C{func(obj, item_data)}

    @note: In all these signatures, 'obj' means Slideshow and
        'item_data' is the value given to Slideshow item append/prepend
        methods, it should represent your item model as you want.

    """
    cdef Elm_Slideshow_Item_Class obj
    cdef readonly object _get_func
    cdef readonly object _del_func

    def __cinit__(self, *a, **ka):
        self._get_func = None
        self._del_func = None

        self.obj.func.get = _py_elm_slideshow_item_get
        self.obj.func.del_ = _py_elm_slideshow_item_del

    def __init__(self, get_func=None, del_func=None):
        if get_func and not callable(get_func):
            raise TypeError("get_func is not callable!")
        elif get_func:
            self._get_func = get_func
        else:
            self._get_func = self.get

        if del_func and not callable(del_func):
            raise TypeError("del_func is not callable!")
        elif del_func:
            self._del_func = del_func
        else:
            try:
                self._del_func = self.delete
            except AttributeError:
                pass

    def __str__(self):
        return ("%s(get_func=%s, del_func=%s)") % \
               (self.__class__.__name__,
                self._get_func,
                self._del_func)

    def __repr__(self):
        return ("%s(%#x, refcount=%d, Elm_Slideshow_Item_Class=%#x, "
                "get_func=%s, del_func=%s)") % \
               (self.__class__.__name__,
                <unsigned long><void *>self,
                PY_REFCOUNT(self),
                <unsigned long>&self.obj,
                self._get_func,
                self._del_func)

    def get(self, evasObject obj, item_data):
        """get(obj, item_data)

        To be called by Slideshow for each item to get its icon.

        @param obj: the Slideshow instance
        @param item_data: the value given to slideshow append/prepend.

        @return: icon object to be used and swallowed.
        @rtype: evas Object or None
        """
        return None

cdef class SlideshowItem(ObjectItem):

    """An item for Slideshow."""

    cdef int _set_obj(self, Elm_Object_Item *item, params=None) except 0:
        assert self.item == NULL, "Object must be clean"
        self.item = item
        Py_INCREF(self)
        return 1

    cdef void _unset_obj(self):
        assert self.item != NULL, "Object must wrap something"
        self.item = NULL

    def __str__(self):
        return "%s(item_class=%s, func=%s, item_data=%s)" % \
               (self.__class__.__name__,
                self.params[0].__class__.__name__,
                self.params[3],
                self.params[1])

    def __repr__(self):
        return ("%s(%#x, refcount=%d, Elm_Object_Item=%#x, "
                "item_class=%s, func=%s, item_data=%r)") % \
               (self.__class__.__name__,
                <unsigned long><void*>self,
                PY_REFCOUNT(self),
                <unsigned long>self.obj,
                self.params[0].__class__.__name__,
                self.params[3],
                self.params[1])

    property object:
        """Get the real Evas object created to implement the view of a given
        slideshow item.

        This returns the actual Evas object used to implement the specified
        slideshow item's view. This may be C{None}, as it may not have been
        created or may have been deleted, at any time, by the slideshow.
        B{Do not modify this object} (move, resize, show, hide, etc.), as
        the slideshow is controlling it. This function is for querying,
        emitting custom signals or hooking lower level callbacks for events
        on that object. Do not delete this object under any circumstances.

        @see: L{ObjectItem.data}

        @type: L{Slideshow}

        """
        def __get__(self):
            return Object_from_instance(elm_slideshow_item_object_get(self.item))

    def show(self):
        """show()

        Display a given slideshow widget's item, programmatically.

        The change between the current item and this item will use the
        transition the slideshow object is set to use.

        @see: L{Slideshow.transition_set()}

        """
        elm_slideshow_item_show(self.item)



cdef public class Slideshow(LayoutClass) [object PyElementarySlideshow, type PyElementarySlideshow_Type]:

    """This widget, as the name indicates, is a pre-made image
    slideshow panel, with API functions acting on (child) image
    items presentation. Between those actions, are:
        - advance to next/previous image
        - select the style of image transition animation
        - set the exhibition time for each image
        - start/stop the slideshow

    The transition animations are defined in the widget's theme,
    consequently new animations can be added without having to
    update the widget's code.

    Slideshow items
    ===============

    For slideshow items, just like for L{Genlist} "genlist" ones,
    the user defines a B{classes}, specifying functions that will be
    called on the item's creation and deletion times.

    The Elm_Slideshow_Item_Class structure contains the following
    members:

        - C{func.get} - When an item is displayed, this function is
          called, and it's where one should create the item object, de
          facto. For example, the object can be a pure Evas image object
          or an Elementary L{Photocam} "photocam" widget.
          See #SlideshowItemGetFunc.
        - C{func.del} - When an item is no more displayed, this function
          is called, where the user must delete any data associated to
          the item. See #SlideshowItemDelFunc.

    Slideshow caching
    =================

    The slideshow provides facilities to have items adjacent to the
    one being displayed B{already "realized"} (i.e. loaded) for
    you, so that the system does not have to decode image data
    anymore at the time it has to actually switch images on its
    viewport. The user is able to set the numbers of items to be
    cached B{before} and B{after} the current item, in the widget's
    item list.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{"changed"} - when the slideshow switches its view to a new item.
            event_info parameter in callback contains the current visible item
        - C{"transition,end"} - when a slide transition ends. event_info
            parameter in callback contains the current visible item

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_slideshow_add(parent.obj))

    def item_add(self, SlideshowItemClass item_class not None, *args, **kwargs):
        """item_add(item_class, *args, **kwargs)

        Add (append) a new item in a given slideshow widget.

        Add a new item to C{obj's} internal list of items, appending it.
        The item's class must contain the function really fetching the
        image object to show for this item, which could be an Evas image
        object or an Elementary photo, for example. The C{data}
        parameter is going to be passed to both class functions of the
        item.

        @see: Elm_Slideshow_Item_Class
        @see: L{item_sorted_insert()}
        @see: L{ObjectItem.data}

        @param item_class: The item class for the item
        @type item_class: SlideshowItemClass

        @return: A handle to the item added or C{None}, on errors
        @rtype: L{SlideshowItem}

        """
        cdef SlideshowItem ret = SlideshowItem()
        cdef Elm_Object_Item *item

        item_data = (args, kwargs)
        ret.params = (item_class, item_data)
        item = elm_slideshow_item_add(self.obj, &item_class.obj, <void*>ret)
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            return None

    def item_sorted_insert(self, SlideshowItemClass item_class not None,
                            func not None, *args, **kwargs):
        """item_sorted_insert(itc, func, *args, **kwargs)

        Insert a new item into the given slideshow widget, using the C{func}
        function to sort items (by item handles).

        Add a new item to C{obj}'s internal list of items, in a position
        determined by the C{func} comparing function. The item's class
        must contain the function really fetching the image object to
        show for this item, which could be an Evas image object or an
        Elementary photo, for example. The C{data} parameter is going to
        be passed to both class functions of the item.

        The compare function compares data1 and data2. If data1 is 'less'
        than data2, -1 must be returned, if it is 'greater', 1 must be
        returned, and if they are equal, 0 must be returned.

        @see: L{SlideshowItemClass}
        @see: L{item_add()}

        @param itc: The item class for the item
        @param func: The comparing function to be used to sort slideshow
            items B{by SlideshowItemClass item handles}
        @return: Returns The slideshow item handle, on success, or
            C{None}, on errors

        """
        cdef SlideshowItem ret = SlideshowItem()
        cdef Elm_Object_Item *item
        cdef Eina_Compare_Cb compare

        if callable(func):
            compare = _py_elm_slideshow_compare_func
        else:
            raise TypeError("func is not None or callable")

        item_data = (args, kwargs)
        ret.params = (item_class, item_data, func)
        item = elm_slideshow_item_sorted_insert(self.obj, &item_class.obj, <void*>ret, compare)
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            return None

    def next(self):
        """next()

        Slide to the B{next} item, in a given slideshow widget

        The sliding animation the object is set to use will be the
        transition effect used, after this call is issued.

        @note: If the end of the slideshow's internal list of items is
            reached, it'll wrap around to the list's beginning, again.

        """
        elm_slideshow_next(self.obj)

    def previous(self):
        """previous()

        Slide to the B{previous} item, in a given slideshow widget

        The sliding animation the object is set to use will be the
        transition effect used, after this call is issued.

        @note: If the beginning of the slideshow's internal list of items
            is reached, it'll wrap around to the list's end, again.

        """
        elm_slideshow_previous(self.obj)

    property transitions:
        """Returns the list of sliding transition/effect names available,
        for a given slideshow widget.

        The transitions, which come from the objects theme, must be an EDC
        data item named C{"transitions"} on the theme file, with (prefix)
        names of EDC programs actually implementing them.

        The available transitions for slideshows on the default theme are:
            - C{"fade"} - the current item fades out, while the new one
              fades in to the slideshow's viewport.
            - C{"black_fade"} - the current item fades to black, and just
              then, the new item will fade in.
            - C{"horizontal"} - the current item slides horizontally, until
              it gets out of the slideshow's viewport, while the new item
              comes from the left to take its place.
            - C{"vertical"} - the current item slides vertically, until it
              gets out of the slideshow's viewport, while the new item comes
              from the bottom to take its place.
            - C{"square"} - the new item starts to appear from the middle of
              the current one, but with a tiny size, growing until its
              target (full) size and covering the old one.

        @see: L{transition}

        @type: tuple of strings

        """
        def __get__(self):
            return tuple(_strings_to_python(elm_slideshow_transitions_get(self.obj)))

    property transition:
        """The slide transition/effect in use for a given slideshow widget

        If C{transition} is implemented in C{obj's} theme (i.e., is
        contained in the list returned by L{transitions}), this new sliding
        effect will be used on the widget.

        @type: string

        """
        def __set__(self, transition):
            elm_slideshow_transition_set(self.obj, _cfruni(transition))
        def __get__(self):
            return _ctouni(elm_slideshow_transition_get(self.obj))

    property timeout:
        """The interval between each image transition on a given
        slideshow widget, B{and start the slideshow, itself}

        After setting this, the slideshow widget will start cycling its
        view, sequentially and automatically, with the images of the
        items it has. The time between each new image displayed is going
        to be C{timeout} in B{seconds}. If a different timeout was set
        previously and an slideshow was in progress, it will continue
        with the new time between transitions, after this call.

        @note: A value less than or equal to 0 on C{timeout} will disable
            the widget's internal timer, thus halting any slideshow which
            could be happening on C{obj}.

        @type: float

        """
        def __set__(self, timeout):
            elm_slideshow_timeout_set(self.obj, timeout)
        def __get__(self):
            return elm_slideshow_timeout_get(self.obj)

    property loop:
        """If, after a slideshow is started, for a given slideshow
        widget, its items should be displayed cyclically or not.

        @note: L{next()} and L{previous()} will
            B{ignore} what is set by this property, i.e., they'll B{always}
            cycle through items. This affects only the "automatic"
            slideshow, as set by L{timeout}.

        @type: bool

        """
        def __set__(self, loop):
            elm_slideshow_loop_set(self.obj, loop)
        def __get__(self):
            return bool(elm_slideshow_loop_get(self.obj))

    def clear(self):
        """clear()

        Remove all items from a given slideshow widget.

        This removes (and deletes) all items in the object, leaving it empty.

        @see: L{ObjectItem.delete()}, to remove just one item.

        """
        elm_slideshow_clear(self.obj)

    property items:
        """Get the internal list of items in a given slideshow widget.

        This list is B{not} to be modified in any way and must not be
        freed. Use the list members with functions like
        L{ObjectItem.delete()}, L{ObjectItem.data}.

        @warning: This list is only valid until C{obj} object's internal
            items list is changed. It should be fetched again with another
            call to this function when changes happen.

        @type: tuple of L{SlideshowItem}s

        """
        def __get__(self):
            return tuple(_object_item_list_to_python(elm_slideshow_items_get(self.obj)))

    property current_item:
        """The currently displayed item, in a given slideshow widget

        @type: L{SlideshowItem}

        """
        def __get__(self):
            return _object_item_to_python(elm_slideshow_item_current_get(self.obj))

    def nth_item_get(self, nth):
        """nth_item_get(nth)

        Get the the item, in a given slideshow widget, placed at position
        C{nth} in its internal items list.

        @param nth: The number of the item to grab a handle to (0 being the
            first)
        @type nth: int

        @return: The item stored in C{obj} at position C{nth} or C{None},
            if there's no item with that index (and on errors)
        @rtype: L{SlideshowItem}

        """
        return _object_item_to_python(elm_slideshow_item_nth_get(self.obj, nth))

    property layout:
        """The current slide layout in use for a given slideshow widget

        If C{layout} is implemented in C{obj's} theme (i.e., is contained
        in the list returned by elm_slideshow_layouts_get()), this new
        images layout will be used on the widget.

        @type: string

        """
        def __set__(self, layout):
            elm_slideshow_layout_set(self.obj, _cfruni(layout))
        def __get__(self):
            return _ctouni(elm_slideshow_layout_get(self.obj))

    property layouts:
        """Returns the list of B{layout} names available, for a given
        slideshow widget.

        Slideshow layouts will change how the widget is to dispose each
        image item in its viewport, with regard to cropping, scaling,
        etc.

        The layouts, which come from the object theme, must be an EDC
        data item name C{"layouts"} on the theme file, with (prefix)
        names of EDC programs actually implementing them.

        The available layouts for slideshows on the default theme are:
            - C{"fullscreen"} - item images with original aspect, scaled to
              touch top and down slideshow borders or, if the image's height
              is not enough, left and right slideshow borders.
            - C{"not_fullscreen"} - the same behavior as the C{"fullscreen"}
              one, but always leaving 10% of the slideshow's dimensions of
              distance between the item image's borders and the slideshow
              borders, for each axis.

        @see: L{layout}

        @type: tuple of strings

        """
        def __get__(self):
            return tuple(_strings_to_python(elm_slideshow_layouts_get(self.obj)))

    property cache_before:
        """The number of items to cache, on a given slideshow widget,
        B{before the current item}

        The default value for this property is C{2}. See
        L{Slideshow_Caching} "slideshow caching" for more details.

        @type: int

        """
        def __set__(self, count):
            elm_slideshow_cache_before_set(self.obj, count)
        def __get__(self):
            return elm_slideshow_cache_before_get(self.obj)

    property cache_after:
        """The number of items to cache, on a given slideshow widget,
        B{after the current item}

        The default value for this property is C{2.} See
        L{Slideshow_Caching} "slideshow caching" for more details.

        @see: L{cache_after_get()}

        @type: int

        """
        def __set__(self, count):
            elm_slideshow_cache_after_set(self.obj, count)
        def __get__(self):
            return elm_slideshow_cache_after_get(self.obj)

    property count:
        """Get the number of items stored in a given slideshow widget

        @type: int

        """
        def __get__(self):
            return elm_slideshow_count_get(self.obj)

    def callback_changed_add(self, func, *args, **kwargs):
        """When the slideshow switches its view to a new item. event_info
        parameter in callback contains the current visible item."""
        self._callback_add_full("changed", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del_full("changed", _cb_object_item_conv, func)

    def callback_transition_end_add(self, func, *args, **kwargs):
        """When a slide transition ends. event_info parameter in callback
        contains the current visible item."""
        self._callback_add_full("transition,end", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_transition_end_del(self, func):
        self._callback_del_full("transition,end", _cb_object_item_conv, func)

_elm_widget_type_register("slideshow", Slideshow)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementarySlideshow_Type # hack to install metaclass
_install_metaclass(&PyElementarySlideshow_Type, ElementaryObjectMeta)
