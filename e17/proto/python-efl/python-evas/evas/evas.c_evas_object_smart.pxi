# This file is included verbatim by c_evas.pyx

cdef object _smart_objects
_smart_objects = dict()
cdef object _smart_classes
_smart_classes = list()

import traceback

cdef void _smart_object_delete(Evas_Object *o):
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")

    try:
        obj._m_delete(obj)
    except Exception, e:
        traceback.print_exc()

    try:
        del obj.delete
    except AttributeError, e:
        pass
    try:
        del obj.move
    except AttributeError, e:
        pass
    try:
        del obj.resize
    except AttributeError, e:
        pass
    try:
        del obj.show
    except AttributeError, e:
        pass
    try:
        del obj.hide
    except AttributeError, e:
        pass
    try:
        del obj.color_set
    except AttributeError, e:
        pass
    try:
        del obj.clip_set
    except AttributeError, e:
        pass
    try:
        del obj.clip_unset
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


cdef void _smart_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y):
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_move is not None:
        try:
            obj._m_move(obj, x, y)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h):
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_resize is not None:
        try:
            obj._m_resize(obj, w, h)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_show(Evas_Object *o):
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_show is not None:
        try:
            obj._m_show(obj)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_hide(Evas_Object *o):
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_hide is not None:
        try:
            obj._m_hide(obj)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_color_set(Evas_Object *o, int r, int g, int b, int a):
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_color_set is not None:
        try:
            obj._m_color_set(obj, r, g, b, a)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_clip_set(Evas_Object *o, Evas_Object *clip):
    cdef SmartObject obj
    cdef Object other
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    other = Object_from_instance(clip)
    if obj._m_clip_set is not None:
        try:
            obj._m_clip_set(obj, other)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_object_clip_unset(Evas_Object *o):
    cdef SmartObject obj
    obj = <SmartObject>evas_object_data_get(o, "python-evas")
    if obj._m_clip_unset is not None:
        try:
            obj._m_clip_unset(obj)
        except Exception, e:
            traceback.print_exc()


cdef void _smart_callback(void *data, Evas_Object *o, void *event_info):
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
    cls_def.data = NULL

    cls = evas_smart_class_new(cls_def);
    return <long>cls


cdef Evas_Smart *_smart_class_from_name(char *name) except NULL:
    cdef long addr
    key = name
    # XXX: for optimizations this should be a try-except KeyError,
    # XXX: but it just don't work, I'm getting non-released objects
    # XXX: due pending references (Pyrex 0.9.5.1a).
    if key in _smart_objects:
        addr = _smart_objects[key]
    else:
        addr = _smart_object_class_new(name)
        _smart_objects[key] = addr
    return <Evas_Smart*>addr


cdef object _smart_class_get_impl_method(object cls, char *name):
    meth = getattr(cls, name)
    orig = getattr(Object, name)
    if meth is orig:
        return None
    else:
        return meth


cdef class SmartObject(Object):
    """Smart Evas Objects.

    Smart objects are user-defined Evas components, it can be used to group
    multiple basic elements and other things. See evas documentation for
    more details.

    You should never instantiate the SmartObject base class directly, but
    inherit and implement methods, then instantiate this new subclass.

    Behavior is defined by defining the following methods:
     * delete(): called in order to remove object from canvas and deallocate
       its resources. Usually you delete object's children here. Default
       implementation delete all registered children.
     * move(): called in order to move object to given position. Usually
       you move children here. Default implementation calculates offset
       and move registered children by it.
     * resize(): called in order to resize object. No default implementation.
     * show(): called in order to show the given element. Usually you call
       the same function on children. No default implementation.
     * hide(): called in order to hide the given element. Usually you call
       the same function on children. No default implementation.
     * color_set(): called in order to change object color. No default
       implementation.
     * clip_set(): called in order to limit object's visible area.
       No default implementation.
     * clip_unset(): called in order to unlimit object's visible area.
       No default implementation.

    Notes:
     * If you redefine object's __init__(), you MUST call your parent!
       Failing to do so will result in objects that just work from
       Python and not from C, for instance, adding your object to Edje
       swallow that clips or set color it will not behave as expected.
     * Do not call your parent on methods you want to replace the behavior
       instead of extending it. For example, some methods have default
       implementation, you may want to remove and replace it with something
       else.
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

    def __dealloc__(self):
        self._smart_callbacks = None

    def __init__(self, Canvas canvas not None, **kargs):
        cdef Evas_Smart *cls
        if type(self) is SmartObject:
            raise TypeError("Must not instantiate SmartObject, but subclasses")
        Object.__init__(self, canvas)
        if self.obj == NULL:
            cls = _smart_class_from_name(self.__class__.__name__)
            self._set_obj(evas_object_smart_add(self._evas.obj, cls))
        self._set_common_params(**kargs)

    def member_add(self, Object child):
        evas_object_smart_member_add(child.obj, self.obj)

    def member_del(self, Object child):
        evas_object_smart_member_del(child.obj)

    def members_get(self):
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
        e = event
        lst = self._smart_callbacks.setdefault(e, [])
        if not lst:
            evas_object_smart_callback_add(self.obj, event, _smart_callback,
                                           <void *>e)
        lst.append((func, args, kargs))

    def callback_del(self, char *event, func):
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
        evas_object_smart_callback_call(self.obj, event, <void*>event_info)

    def delete(self):
        cdef Evas_List *lst, *itr
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            evas_object_del(<Evas_Object*>itr.data)
            itr = itr.next
        evas_list_free(lst)

    def move(self, int x, int y):
        cdef Evas_List *lst, *itr
        cdef Evas_Object *o
        cdef int orig_x, orig_y, dx, dy
        evas_object_geometry_get(self.obj, &orig_x, &orig_y, NULL, NULL)
        dx = x - orig_x
        dy = y - orig_y
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            o = <Evas_Object*>itr.data
            evas_object_geometry_get(o, &orig_x, &orig_y, NULL, NULL)
            evas_object_move(o, orig_x + dx, orig_y + dy)
            itr = itr.next
        evas_list_free(lst)


    # Factory
    def Rectangle(self, **kargs):
        obj = Rectangle(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Line(self, **kargs):
        obj = Line(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Image(self, **kargs):
        obj = Image(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Gradient(self, **kargs):
        obj = Gradient(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Polygon(self, **kargs):
        obj = Polygon(self.evas, **kargs)
        self.member_add(obj)
        return obj

    def Text(self, **kargs):
        obj = Text(self.evas, **kargs)
        self.member_add(obj)
        return obj
