# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by edje.c_edje.pyx

cdef class Message:
    """Base class to represent Edje messages.

    @ivar type: message type.
    @ivar id: message id.
    """
    property type:
        def __get__(self):
            return self._type

    property id:
        def __get__(self):
            return self._id


cdef class MessageString(Message):
    """String message.

    @ivar str: message contents.
    @ivar val: same as str.
    """
    property str:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            if self.obj.str != NULL:
                return self.obj.str

    property val:  # copy from 'str', to make api consistent
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            if self.obj.str != NULL:
                return self.obj.str


cdef class MessageInt(Message):
    """Integer message.

    @ivar val: message contents.
    """
    property val:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.val


cdef class MessageFloat(Message):
    """Float message.

    @ivar val: message contents.
    """
    property val:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.val


cdef class MessageStringSet(Message):
    """String set message.

    @ivar val: message contents.
    """
    property val:
        def __get__(self):
            cdef int i
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            lst = []
            for i from 0 <= i < self.obj.count:
                if self.obj.str[i] != NULL:
                    lst.append(self.obj.str[i])
            return lst

    def __len__(self):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        return self.obj.count

    def __getitem__(self, int index):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        if index < 0 or index >= self.obj.count:
            raise IndexError("list index out of range")
        return self.obj.str[index]


cdef class MessageIntSet(Message):
    """Integer set message.

    @ivar val: message contents.
    """
    property val:
        def __get__(self):
            cdef int i
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            lst = []
            for i from 0 <= i < self.obj.count:
                lst.append(self.obj.val[i])
            return lst

    def __len__(self):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        return self.obj.count

    def __getitem__(self, int index):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        if index < 0 or index >= self.obj.count:
            raise IndexError("list index out of range")
        return self.obj.val[index]


cdef class MessageFloatSet(Message):
    """Float set message.

    @ivar val: message contents.
    """
    property val:
        def __get__(self):
            cdef int i
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            lst = []
            for i from 0 <= i < self.obj.count:
                lst.append(self.obj.val[i])
            return lst

    def __len__(self):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        return self.obj.count

    def __getitem__(self, int index):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        if index < 0 or index >= self.obj.count:
            raise IndexError("list index out of range")
        return self.obj.val[index]


cdef class MessageStringInt(Message):
    """String + Integer message.

    @ivar str: string.
    @ivar val: integer.
    """
    property str:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            if self.obj.str != NULL:
                return self.obj.str

    property val:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.val


cdef class MessageStringFloat(Message):
    """String + float message.

    @ivar str: string.
    @ivar val: float.
    """
    property str:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            if self.obj.str != NULL:
                return self.obj.str

    property val:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.val


cdef class MessageStringIntSet(Message):
    """String + Integer set message.

    @ivar str: string.
    @ivar val: integer list.
    """
    property str:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            if self.obj.str != NULL:
                return self.obj.str

    property val:
        def __get__(self):
            cdef int i
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            lst = []
            for i from 0 <= i < self.obj.count:
                lst.append(self.obj.val[i])
            return lst

    def __len__(self):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        return self.obj.count

    def __getitem__(self, int index):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        if index < 0 or index >= self.obj.count:
            raise IndexError("list index out of range")
        return self.obj.val[index]


cdef class MessageStringFloatSet(Message):
    """String + Float set message.

    @ivar str: string.
    @ivar val: float list.
    """
    property str:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            if self.obj.str != NULL:
                return self.obj.str

    property val:
        def __get__(self):
            cdef int i
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            lst = []
            for i from 0 <= i < self.obj.count:
                lst.append(self.obj.val[i])
            return lst

    def __len__(self):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        return self.obj.count

    def __getitem__(self, int index):
        if self.obj == NULL:
            raise ValueError("Object uninitialized")
        if index < 0 or index >= self.obj.count:
            raise IndexError("list index out of range")
        return self.obj.val[index]


cdef Message MessageString_from_ptr(void *msg):
    cdef MessageString m
    m = MessageString()
    m.obj = <Edje_Message_String*>msg
    return m


cdef Message MessageInt_from_ptr(void *msg):
    cdef MessageInt m
    m = MessageInt()
    m.obj = <Edje_Message_Int*>msg
    return m


cdef Message MessageFloat_from_ptr(void *msg):
    cdef MessageFloat m
    m = MessageFloat()
    m.obj = <Edje_Message_Float*>msg
    return m


cdef Message MessageStringSet_from_ptr(void *msg):
    cdef MessageStringSet m
    m = MessageStringSet()
    m.obj = <Edje_Message_String_Set*>msg
    return m


cdef Message MessageIntSet_from_ptr(void *msg):
    cdef MessageIntSet m
    m = MessageIntSet()
    m.obj = <Edje_Message_Int_Set*>msg
    return m


cdef Message MessageFloatSet_from_ptr(void *msg):
    cdef MessageFloatSet m
    m = MessageFloatSet()
    m.obj = <Edje_Message_Float_Set*>msg
    return m


cdef Message MessageStringInt_from_ptr(void *msg):
    cdef MessageStringInt m
    m = MessageStringInt()
    m.obj = <Edje_Message_String_Int*>msg
    return m


cdef Message MessageStringFloat_from_ptr(void *msg):
    cdef MessageStringFloat m
    m = MessageStringFloat()
    m.obj = <Edje_Message_String_Float*>msg
    return m


cdef Message MessageStringIntSet_from_ptr(void *msg):
    cdef MessageStringIntSet m
    m = MessageStringIntSet()
    m.obj = <Edje_Message_String_Int_Set*>msg
    return m


cdef Message MessageStringFloatSet_from_ptr(void *msg):
    cdef MessageStringFloatSet m
    m = MessageStringFloatSet()
    m.obj = <Edje_Message_String_Float_Set*>msg
    return m


cdef Message Message_from_type(Edje_Message_Type type, int id, void *msg):
    cdef Message m
    if type == EDJE_MESSAGE_NONE:
        return None
    elif type == EDJE_MESSAGE_SIGNAL:
        m = MessageSignal()
    elif type == EDJE_MESSAGE_STRING:
        m = MessageString_from_ptr(msg)
    elif type == EDJE_MESSAGE_INT:
        m = MessageInt_from_ptr(msg)
    elif type == EDJE_MESSAGE_FLOAT:
        m = MessageFloat_from_ptr(msg)
    elif type == EDJE_MESSAGE_STRING_SET:
        m = MessageStringSet_from_ptr(msg)
    elif type == EDJE_MESSAGE_INT_SET:
        m = MessageIntSet_from_ptr(msg)
    elif type == EDJE_MESSAGE_FLOAT_SET:
        m = MessageFloatSet_from_ptr(msg)
    elif type == EDJE_MESSAGE_STRING_INT:
        m = MessageStringInt_from_ptr(msg)
    elif type == EDJE_MESSAGE_STRING_FLOAT:
        m = MessageStringFloat_from_ptr(msg)
    elif type == EDJE_MESSAGE_STRING_INT_SET :
        m = MessageStringIntSet_from_ptr(msg)
    elif type == EDJE_MESSAGE_STRING_FLOAT_SET :
        m = MessageStringFloatSet_from_ptr(msg)

    m._type = type
    m._id = id
    return m
