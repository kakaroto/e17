# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Ulisses Furquim
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by c_evas.pyx

cdef object _smart_classes
_smart_classes = list()

import traceback
import types

cdef void _smart_object_delete(Evas_Object *o) with gil:
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")

    try:
        obj._m_delete(obj)
    except Exception, e:
        traceback.print_exc()

    if type(obj.delete) is types.MethodType:
        try:
            del obj.delete
        except AttributeError, e:
            pass
    if type(obj.move) is types.MethodType:
        try:
            del obj.move
        except AttributeError, e:
            pass
    if type(obj.resize) is types.MethodType:
        try:
            del obj.resize
        except AttributeError, e:
            pass
    if type(obj.show) is types.MethodType:
        try:
            del obj.show
        except AttributeError, e:
            pass
    if type(obj.hide) is types.MethodType:
        try:
            del obj.hide
        except AttributeError, e:
            pass
    if type(obj.color_set) is types.MethodType:
        try:
            del obj.color_set
        except AttributeError, e:
            pass
    if type(obj.clip_set) is types.MethodType:
        try:
            del obj.clip_set
        except AttributeError, e:
            pass
    if type(obj.clip_unset) is types.MethodType:
        try:
            del obj.clip_unset
        except AttributeError, e:
            pass
    if type(obj.calculate) is types.MethodType:
        try:
            del obj.calculate
        except AttributeError, e:
            pass

    obj._smart_callbacks = None
    obj._m_delete = None
    obj._m_move = None
    obj._m_resize = None
    obj._m_show = None
    obj._m_hide = None
    obj._m_color_set = None
    obj._m_clip_set = None
    obj._m_clip_unset = None
    obj._m_calculate = None


cdef void _smart_object_move(Evas_Object *o,
                             Evas_Coord x, Evas_Coord y) with gil:
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_move is not None:
        try:
            obj._m_move(obj, x, y)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_resize(Evas_Object *o,
                               Evas_Coord w, Evas_Coord h) with gil:
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_resize is not None:
        try:
            obj._m_resize(obj, w, h)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_show(Evas_Object *o) with gil:
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_show is not None:
        try:
            obj._m_show(obj)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_hide(Evas_Object *o) with gil:
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_hide is not None:
        try:
            obj._m_hide(obj)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_color_set(Evas_Object *o,
                                  int r, int g, int b, int a) with gil:
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_color_set is not None:
        try:
            obj._m_color_set(obj, r, g, b, a)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_clip_set(Evas_Object *o, Evas_Object *clip) with gil:
    cdef SmartObject obj
    cdef Object other
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    other = Object_from_instance(clip)
    if obj._m_clip_set is not None:
        try:
            obj._m_clip_set(obj, other)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_clip_unset(Evas_Object *o) with gil:
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_clip_unset is not None:
        try:
            obj._m_clip_unset(obj)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_calculate(Evas_Object *o) with gil:
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_calculate is not None:
        try:
            obj._m_calculate(obj)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_callback(void *data,
                          Evas_Object *o, void *event_info) with gil:
    cdef SmartObject obj
    cdef object event, ei
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    event = <object>data
    ei = <object>event_info
    try:
        lst = obj._smart_callbacks[event]
    except KeyError, e:
        traceback.print_exc()
        return
    for func, args, kargs in lst:
        try:
            func(obj, ei, *args, **kargs)
        except Exception, e:
            traceback.print_exc()


cdef long _smart_object_class_new(char *name) except 0:
    cdef Evas_Smart_Class *cls_def
    cdef Evas_Smart *cls

    cls_def = <Evas_Smart_Class*>python.PyMem_Malloc(sizeof(Evas_Smart_Class))
    if cls_def == NULL:
        return 0

    _smart_classes.append(<long>cls_def)
    cls_def.name = name
    cls_def.version = EVAS_SMART_CLASS_VERSION
    cls_def.add = NULL # use python constructor
    cls_def.delete = _smart_object_delete
    cls_def.move = _smart_object_move
    cls_def.resize = _smart_object_resize
    cls_def.show = _smart_object_show
    cls_def.hide = _smart_object_hide
    cls_def.color_set = _smart_object_color_set
    cls_def.clip_set = _smart_object_clip_set
    cls_def.clip_unset = _smart_object_clip_unset
    cls_def.calculate = _smart_object_calculate
    cls_def.data = NULL

    cls = evas_smart_class_new(cls_def);
    return <long>cls


class EvasSmartObjectMeta(EvasObjectMeta):
    def __init__(cls, name, bases, dict_):
        EvasObjectMeta.__init__(cls, name, bases, dict_)
        cls._setup_smart_class()

    def _setup_smart_class(cls):
        if "__evas_smart_class__" in cls.__dict__:
            return

        cdef long addr
        addr = _smart_object_class_new(cls.__name__)
        cls.__evas_smart_class__ = addr


cdef object _smart_class_get_impl_method(object cls, char *name):
    meth = getattr(cls, name)
    orig = getattr(Object, name)
    if meth is orig:
        return None
    else:
        return meth


cdef object _smart_class_get_impl_method_cls(object cls, object parent_cls,
                                             char *name):
    meth = getattr(cls, name)
    orig = getattr(parent_cls, name)
    if meth is orig:
        return None
    else:
        return meth


cdef public class SmartObject(Object) [object PyEvasSmartObject,
                                       type PyEvasSmartObject_Type]:
    """Smart Evas Objects.

    Smart objects are user-defined Evas components, often used to group
    multiple basic elements, associate an object with a clip and deal with
    them as an unit. See evas documentation for more details.

    Recommended use is to create an B{clipper} object and clip every other
    member to it, then you can have all your other members to be always
    visible and implement L{hide()}, L{show()}, L{color_set()}, L{clip_set()}
    and L{clip_unset()} to just affect the B{clipper}. See
    L{ClippedSmartObject}.

    B{Pay attention that just creating an object within the SmartObject
    doesn't make it a member!} You must do L{member_add()} or use one of
    the provided factories to ensure that. Failing to do so will leave
    created objects on different layer and no stacking will be done for you.

    Behavior is defined by defining the following methods:
     - L{delete()}: called in order to remove object from canvas and
       deallocate its resources. Usually you delete object's children here.
       I{Default implementation delete all registered children.}
     - L{move()}: called in order to move object to given position. Usually
       you move children here. I{Default implementation calculates offset
       and move registered children by it.}
     - L{resize()}: called in order to resize object. I{No default
       implementation.}
     - L{show()}: called in order to show the given element. Usually you
       call the same function on children. I{No default implementation.}
     - L{hide()}: called in order to hide the given element. Usually you
       call the same function on children. I{No default implementation.}
     - L{color_set()}: called in order to change object color. I{No default
       implementation.}
     - L{clip_set()}: called in order to limit object's visible area.
       I{No default implementation.}
     - L{clip_unset()}: called in order to unlimit object's visible area.
       I{No default implementation.}
     - L{calculate()}: called before object is used for rendering and it is
       marked as dirty/changed with L{changed()}. I{Default implementation
       does nothing.}

    @note: You should never instantiate the SmartObject base class directly,
       but inherit and implement methods, then instantiate this new subclass.
    @note: If you redefine object's __init__(), you MUST call your parent!
       Failing to do so will result in objects that just work from
       Python and not from C, for instance, adding your object to Edje
       swallow that clips or set color it will not behave as expected.
    @note: Do not call your parent on methods you want to replace the behavior
       instead of extending it. For example, some methods have default
       implementation, you may want to remove and replace it with something
       else.

    @group Children manipulation: member_add, member_del, members_get,
       members
    @group Factories: Rectangle, Line, Image, FilledImage, Gradient,
       Polygon, Text
    @group Default implementations: delete, move, calculate
    @group Missing implementations: resize, show, hide, color_set,
       clip_set, clip_unset
    @group Event system: callback_add, callback_del, callback_call

    @see: L{ClippedSmartObject}
    """
    def __new__(self, *a, **ka):
        self._smart_callbacks = dict()
        cls = self.__class__
        self._m_delete = _smart_class_get_impl_method(cls, "delete")
        if self._m_delete is not None:
            self.delete = python.PyMethod_New(Object.delete, self, cls)
        self._m_move = _smart_class_get_impl_method(cls, "move")
        if self._m_move is not None:
            self.move = python.PyMethod_New(Object.move, self, cls)
        self._m_resize = _smart_class_get_impl_method(cls, "resize")
        if self._m_resize is not None:
            self.resize = python.PyMethod_New(Object.resize, self, cls)
        self._m_show = _smart_class_get_impl_method(cls, "show")
        if self._m_show is not None:
            self.show = python.PyMethod_New(Object.show, self, cls)
        self._m_hide = _smart_class_get_impl_method(cls, "hide")
        if self._m_hide is not None:
            self.hide = python.PyMethod_New(Object.hide, self, cls)
        self._m_color_set = _smart_class_get_impl_method(cls, "color_set")
        if self._m_color_set is not None:
            self.color_set = python.PyMethod_New(Object.color_set, self, cls)
        self._m_clip_set = _smart_class_get_impl_method(cls, "clip_set")
        if self._m_clip_set is not None:
            self.clip_set = python.PyMethod_New(Object.clip_set, self, cls)
        self._m_clip_unset = _smart_class_get_impl_method(cls, "clip_unset")
        if self._m_clip_unset is not None:
            self.clip_unset = python.PyMethod_New(Object.clip_unset, self, cls)
        self._m_calculate = _smart_class_get_impl_method_cls(
            cls, SmartObject, "calculate")
        if self._m_calculate is not None:
            self.calculate = python.PyMethod_New(
                SmartObject.calculate, self, cls)

    def __dealloc__(self):
        self._smart_callbacks = None

    def __init__(self, Canvas canvas not None, **kargs):
        cdef long addr
        if type(self) is SmartObject:
            raise TypeError("Must not instantiate SmartObject, but subclasses")
        Object.__init__(self, canvas)
        if self.obj == NULL:
            addr = self.__evas_smart_class__
            self._set_obj(evas_object_smart_add(self.evas.obj,
                                                <Evas_Smart*>addr))
        self._set_common_params(**kargs)

    def member_add(self, Object child):
        """Set an evas object as a member of this object.

        Members will automatically be stacked and layered with the smart
        object. The various stacking function will operate on members relative
        to the other members instead of the entire canvas.

        Non-member objects can not interleave a smart object's members.

        @note: if B{child} is already member of another SmartObject, it
           will be deleted from that membership and added to this object.
        """
        evas_object_smart_member_add(child.obj, self.obj)

    def member_del(self, Object child):
        """Removes a member object from a smart object.

        @attention: this will actually map to C API as
           C{evas_object_smart_member_del(child)}, so the object will
           loose it's parent B{event if the object is not part of this object}.
        """
        evas_object_smart_member_del(child.obj)

    def members_get(self):
        "@rtype: tuple of L{Object}"
        cdef Evas_List *lst, *itr
        cdef Object o
        ret = []
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            o = Object_from_instance(<Evas_Object*>itr.data)
            ret.append(o)
            itr = itr.next
        evas_list_free(lst)
        return tuple(ret)

    property members:
        def __get__(self):
            return self.members_get()

    def callback_add(self, char *event, func, *args, **kargs):
        """Add a callback for the smart event specified by event.

        @parm: B{event} event name
        @parm: B{func} what to callback. Should have the signature:
           C{function(object, event_info, *args, **kargs)}

        @raise TypeError: if B{func} is not callable.
        @warning: B{event_info} will always be a python object, if the
           signal is provided by a C-only class, it will crash.
        """
        if not callable(func):
            raise TypeError("func must be callable")

        e = event
        lst = self._smart_callbacks.setdefault(e, [])
        if not lst:
            evas_object_smart_callback_add(self.obj, event, _smart_callback,
                                           <void *>e)
        lst.append((func, args, kargs))

    def callback_del(self, char *event, func):
        """Remove a smart callback.

        Removes a callback that was added by L{callback_add()}.

        @parm: B{event} event name
        @parm: B{func} what to callback, should have be previously registered.
        @precond: B{event} and B{func} must be used as parameter for
           L{callback_add()}.

        @raise ValueError: if there was no B{func} connected with this event.
        """
        try:
            lst = self._smart_callbacks[event]
        except KeyError, e:
            raise ValueError("Unknown event %r" % e)

        i = -1
        for i, (f, a, k) in enumerate(lst):
            if func == f:
                break
        else:
            raise ValueError("Callback %s was not registered with event %r" %
                             (func, e))

        del lst[i]
        if not lst:
            del self._smart_callbacks[event]
            evas_object_smart_callback_del(self.obj, event, _smart_callback)

    def callback_call(self, char *event, event_info=None):
        """Call any smart callbacks for event.

        @parm: B{event} the event name
        @parm: B{event_info} an event specific info to pass to the callback.

        This should be called internally in the smart object when some
        specific event has occured. The documentation for the smart object
        should include a list of possible events and what type of B{event_info}
        to expect.

        @attention: B{event_info} will always be a python object.
        """
        evas_object_smart_callback_call(self.obj, event, <void*>event_info)

    def delete(self):
        "Default implementation to delete all children."
        cdef Evas_List *lst, *itr
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            evas_object_del(<Evas_Object*>itr.data)
            itr = itr.next
        evas_list_free(lst)

    def move_children_relative(self, int dx, int dy):
        "Move all children relatively."
        cdef Evas_List *lst, *itr
        cdef Evas_Object *o
        cdef int orig_x, orig_y

        if dx == 0 and dy == 0:
            return

        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            o = <Evas_Object*>itr.data
            evas_object_geometry_get(o, &orig_x, &orig_y, NULL, NULL)
            evas_object_move(o, orig_x + dx, orig_y + dy)
            itr = itr.next
        evas_list_free(lst)

    def move(self, int x, int y):
        "Default implementation to move all children."
        cdef int orig_x, orig_y, dx, dy
        evas_object_geometry_get(self.obj, &orig_x, &orig_y, NULL, NULL)
        dx = x - orig_x
        dy = y - orig_y
        self.move_children_relative(dx, dy)

    def resize(self, int w, int h):
        "Virtual method resize(w, h) of SmartObject"
        print "%s.resize(w, h) not implemented." % self.__class__.__name__

    def show(self):
        "Virtual method show() of SmartObject"
        print "%s.show() not implemented." % self.__class__.__name__

    def hide(self):
        "Virtual method hide() of SmartObject"
        print "%s.hide() not implemented." % self.__class__.__name__

    def color_set(self, int r, int g, int b, int a):
        "Virtual method color_set(r, g, b, a) of SmartObject"
        print "%s.color_set(r, g, b, a) not implemented." % \
              self.__class__.__name__

    def clip_set(self, Object clip):
        "Virtual method clip(object) of SmartObject"
        print "%s.clip_set(object) not implemented." % self.__class__.__name__

    def clip_unset(self):
        "Virtual method clip_unset() of SmartObject"
        print "%s.clip_unset() not implemented." % self.__class__.__name__

    def calculate(self):
        "Request object to recalculate it's internal state."
        evas_object_smart_calculate(self.obj)

    def changed(self):
        """Mark object as changed, so it's L{calculate()} will be called.

        If an object is changed and it provides a calculate() method,
        it will be called from L{Evas.render()}, what we call pre-render
        calculate.

        This can be used to postpone heavy calculations until you need to
        display the object, example: layout calculations.
        """
        evas_object_smart_changed(self.obj)

    def need_recalculate_set(self, unsigned int value):
        """Set need_recalculate flag.

        Set the need_recalculate flag of given smart object.

        If this flag is set then calculate() callback (method) of the
        given smart object will be called, if one is provided, during
        render phase usually evas_render(). After this step, this flag
        will be automatically unset.

        If no calculate() is provided, this flag will be left unchanged.

        @note just setting this flag will not make scene dirty and
        evas_render() will have no effect. To do that, use
        evas_object_smart_changed(), that will automatically call this
        function with 1 as parameter.

        """
        evas_object_smart_need_recalculate_set(self.obj, value)

    def need_recalculate_get(self):
        """Get the current value of need_recalculate flag.

        @note this flag will be unset during the render phase, after
        calculate() is called if one is provided.  If no calculate()
        is provided, then the flag will be left unchanged after render
        phase.
        """
        return evas_object_smart_need_recalculate_get(self.obj)

    property need_recalculate:
        def __set__(self, value):
            self.need_recalculate_set(value)

        def __get__(self):
            self.need_recalculate_get()

    # Factory
    def Rectangle(self, **kargs):
        """Factory of children L{evas.Rectangle}.
        @rtype: L{Rectangle<evas.Rectangle>}
        """
        obj = Rectangle(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Line(self, **kargs):
        """Factory of children L{evas.Line}.
        @rtype: L{Line<evas.Line>}
        """
        obj = Line(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Image(self, **kargs):
        """Factory of children L{evas.Image}.
        @rtype: L{Image<evas.Image>}
        """
        obj = Image(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def FilledImage(self, **kargs):
        """Factory of L{evas.FilledImage} associated with this canvas.
        @rtype: L{FilledImage<evas.FilledImage>}
        """
        obj = FilledImage(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Gradient(self, **kargs):
        """Factory of children L{evas.Gradient}.
        @rtype: L{Gradient<evas.Gradient>}
        """
        obj = Gradient(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Polygon(self, **kargs):
        """Factory of children L{evas.Polygon}.
        @rtype: L{Polygon<evas.Polygon>}
        """
        obj = Polygon(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Text(self, **kargs):
        """Factory of children L{evas.Text}.
        @rtype: L{Text<evas.Text>}
        """
        obj = Text(self.evas, **kargs)
        self.member_add(obj)
        return obj


cdef extern from "Python.h":
    cdef python.PyTypeObject PyEvasSmartObject_Type # hack to install metaclass

_install_metaclass(&PyEvasSmartObject_Type, EvasSmartObjectMeta)


cdef public class ClippedSmartObject(SmartObject) \
         [object PyEvasClippedSmartObject, type PyEvasClippedSmartObject_Type]:
    """SmartObject subclass that automatically handles an internal clipper.

    This class is optimized for the recommended SmartObject usage of
    having an internal clipper, with all member objects clipped to it and
    operations like L{hide()}, L{show()}, L{color_set()}, L{clip_set()} and
    L{clip_unset()} operating on it.

    This internal clipper size is huge by default (and not the same as the
    object size), this means that you should clip this object to another
    object clipper to get its contents restricted. This is the default
    because many times what we want are contents that can overflow SmartObject
    boudaries (ie: members with animations coming in from outside).

    @group Children manipulation: member_add, member_del
    @group Default implementations: delete, move, show, hide, color_set,
       clip_set, clip_unset, calculate
    @ivar clipper: the internal object used for clipping. You shouldn't
       mess with it.
    """
    def __init__(self, Canvas canvas not None, **kargs):
        if type(self) is ClippedSmartObject:
            raise TypeError("Must not instantiate ClippedSmartObject, but "
                            "subclasses")
        SmartObject.__init__(self, canvas, **kargs)
        if self.clipper is None:
            self.clipper = Rectangle(canvas)
            evas_object_move(self.clipper.obj, -10000, -10000);
            evas_object_resize(self.clipper.obj, 20000, 20000);
            evas_object_pass_events_set(self.clipper.obj, 1);
            evas_object_smart_member_add(self.clipper.obj, self.obj)

    def member_add(self, Object child):
        "Set an evas object as a member of this object, already clipping."
        evas_object_clip_set(child.obj, self.clipper.obj)
        evas_object_smart_member_add(child.obj, self.obj)
        if evas_object_visible_get(self.obj):
            evas_object_show(self.clipper.obj)

    def member_del(self, Object child):
        "Removes a member object from a smart object, already unsets its clip."
        evas_object_clip_unset(child.obj)
        evas_object_smart_member_del(child.obj)
        if evas_object_clipees_get(self.clipper.obj) == NULL:
            evas_object_hide(self.clipper.obj)

    def show(self):
        "Default implementation that acts on the the clipper."
        if evas_object_clipees_get(self.clipper.obj) != NULL:
            evas_object_show(self.clipper.obj)

    def hide(self):
        "Default implementation that acts on the the clipper."
        evas_object_hide(self.clipper.obj)

    def color_set(self, int r, int g, int b, int a):
        "Default implementation that acts on the the clipper."
        evas_object_color_set(self.clipper.obj, r, g, b, a)

    def clip_set(self, Object clip):
        "Default implementation that acts on the the clipper."
        evas_object_clip_set(self.clipper.obj, clip.obj)

    def clip_unset(self):
        "Default implementation that acts on the the clipper."
        evas_object_clip_unset(self.clipper.obj)


cdef extern from "Python.h":
    cdef python.PyTypeObject PyEvasClippedSmartObject_Type # hack to install metaclass

_install_metaclass(&PyEvasClippedSmartObject_Type, EvasSmartObjectMeta)
