# This file is included verbatim by c_evas.pyx

cdef object _smart_objects
_smart_objects = dict()
_smart_classes = list()


cdef void _smart_object_delete(Evas_Object *o):
    cdef SmartObject obj
    obj = <SmartObject>Object_from_instance(o)
    if obj._m_delete is not None:
        obj._m_delete(obj)


cdef void _smart_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y):
    cdef SmartObject obj
    obj = <SmartObject>Object_from_instance(o)
    if obj._m_move is not None:
        obj._m_move(obj, x, y)


cdef void _smart_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h):
    cdef SmartObject obj
    obj = <SmartObject>Object_from_instance(o)
    if obj._m_resize is not None:
        obj._m_resize(obj, w, h)


cdef void _smart_object_show(Evas_Object *o):
    cdef SmartObject obj
    obj = <SmartObject>Object_from_instance(o)
    if obj._m_show is not None:
        obj._m_show(obj)


cdef void _smart_object_hide(Evas_Object *o):
    cdef SmartObject obj
    obj = <SmartObject>Object_from_instance(o)
    if obj._m_hide is not None:
        obj._m_hide(obj)


cdef void _smart_object_color_set(Evas_Object *o, int r, int g, int b, int a):
    cdef SmartObject obj
    obj = <SmartObject>Object_from_instance(o)
    if obj._m_color_set is not None:
        obj._m_color_set(obj, r, g, b, a)


cdef void _smart_object_clip_set(Evas_Object *o, Evas_Object *clip):
    cdef SmartObject obj
    cdef Object other
    obj = <SmartObject>Object_from_instance(o)
    other = Object_from_instance(clip)
    if obj._m_clip_set is not None:
        obj._m_clip_set(obj)


cdef void _smart_object_clip_unset(Evas_Object *o):
    cdef SmartObject obj
    obj = <SmartObject>Object_from_instance(o)
    if obj._m_clip_unset is not None:
        obj._m_clip_unset(obj)


cdef void _smart_callback(void *data, Evas_Object *o, void *event_info):
    cdef SmartObject obj
    cdef object event, ei
    obj = <SmartObject>Object_from_instance(o)
    event = <object>data
    ei = <object>event_info
    lst = obj._smart_callbacks[event]
    for func, args, kargs in lst:
        func(obj, ei, *args, **kargs)


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
    cdef Evas_Smart *cls
    if _smart_objects.has_key(name):
        addr = _smart_objects[name]
    else:
        addr = _smart_object_class_new(name)
        _smart_objects[name] = addr
    cls = <Evas_Smart*>addr
    return cls


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
       the same function on children. Default implementation calls show()
       on registered children.
     * hide(): called in order to hide the given element. Usually you call
       the same function on children. Default implementation calls hide()
       on registered children.
     * color_set(): called in order to change object color. No default
       implementation.
     * clip_set(): called in order to limit object's visible area.
       Default implementation calls clip_set() on every registered
       children.
     * clip_unset(): called in order to unlimit object's visible area.
       Default implementation calls clip_unset() on every registered
       children.

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
    def __new__(self, Canvas evas):
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

    def _new_obj(self):
        cdef Evas_Smart *cls
        if self.obj == NULL:
            cls = _smart_class_from_name(self.__class__.__name__)
            self._set_obj(evas_object_smart_add(self._evas.obj, cls))

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
        lst = self._smart_callbacks[event]
        i = -1
        for i, (f, a, k) in enumerate(lst):
            if func == f:
                break
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

    def show(self):
        cdef Evas_List *lst, *itr
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            evas_object_show(<Evas_Object*>itr.data)
            itr = itr.next
        evas_list_free(lst)

    def hide(self):
        cdef Evas_List *lst, *itr
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            evas_object_hide(<Evas_Object*>itr.data)
            itr = itr.next
        evas_list_free(lst)

    def color_set(self, int r, int g, int b, int a):
        cdef Evas_List *lst, *itr
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            evas_object_color_set(<Evas_Object*>itr.data, r, g, b, a)
            itr = itr.next
        evas_list_free(lst)

    def clip_set(self, obj):
        cdef Evas_List *lst, *itr
        cdef Object o
        cdef Evas_Object *clip
        if obj is None:
            clip = NULL
        elif isinstance(obj, Object):
            o = <Object>obj
            clip = o.obj
        else:
            raise ValueError("clip must be instance of evas.Object or None.")
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            evas_object_clip_set(<Evas_Object*>itr.data, clip)
            itr = itr.next
        evas_list_free(lst)

    def clip_unset(self):
        cdef Evas_List *lst, *itr
        lst = evas_object_smart_members_get(self.obj)
        itr = lst
        while itr:
            evas_object_clip_unset(<Evas_Object*>itr.data)
            itr = itr.next
        evas_list_free(lst)

    # Factory
    def Rectangle(self, size=None, pos=None, geometry=None, color=None,
                  name=None):
        obj = Rectangle(self.evas)
        obj._new_obj()
        obj._set_common_params(size=size, pos=pos, geometry=geometry,
                               color=color, name=name)
        self.member_add(obj)
        return obj

    def Line(self, start=None, end=None, size=None, pos=None,
             geometry=None, color=None, name=None):
        obj = Line(self.evas)
        obj._new_obj()
        obj._set_common_params(start=start, end=end, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        self.member_add(obj)
        return obj


    def Image(self, file=None, size=None, pos=None, geometry=None,
              color=None, name=None):
        obj = Image(self.evas)
        obj._new_obj()
        obj._set_common_params(file=file, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        self.member_add(obj)
        return obj

    def Gradient(self, size=None, pos=None, geometry=None, color=None,
                 name=None):
        obj = Gradient(self.evas)
        obj._new_obj()
        obj._set_common_params(size=size, pos=pos, geometry=geometry,
                               color=color, name=name)
        self.member_add(obj)
        return obj

    def Polygon(self, points=None, size=None, pos=None, geometry=None,
                color=None, name=None):
        obj = Polygon(self.evas)
        obj._new_obj()
        obj._set_common_params(points=points, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        self.member_add(obj)
        return obj
