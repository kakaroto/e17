# Copyright (c) 2008-2009 Simon Busch
# Copyright (c) 2010-2010 ProFUSION embedded systems
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

cdef class NaviframeItem(ObjectItem):

    """An item for the Naviframe widget."""

    def item_pop_to(self):
        _METHOD_DEPRECATED(self, "pop_to")
        elm_naviframe_item_pop_to(self.item)

    def pop_to(self):
        """item_pop_to()

        Pop the items between the top and the above one on the given item.

        """
        elm_naviframe_item_pop_to(self.item)

    def item_promote(self):
        _METHOD_DEPRECATED(self, "promote")
        elm_naviframe_item_promote(self.item)

    def promote(self):
        """item_promote()

        Promote an item already in the naviframe stack to the top of the stack

        This will take the indicated item and promote it to the top of the stack
        as if it had been pushed there. The item must already be inside the
        naviframe stack to work.

        """
        elm_naviframe_item_promote(self.item)

    def style_set(self, style):
        """Set an item style

        The following styles are available for this item:
            - C{"default"}

        @see: L{style_get()}

        @param item_style: The item style name. C{None} would be default
        @type item_style: string

        """
        elm_naviframe_item_style_set(self.item, _cfruni(style))

    def style_get(self):
        """Get an item style

        @see: L{style_set()}

        @return: The current item style name
        @rtype: string

        """
        return _ctouni(elm_naviframe_item_style_get(self.item))

    property style:
        """The item style.

        The following styles are available for this item:
            - C{"default"}

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_naviframe_item_style_get(self.item))
        def __set__(self, style):
            elm_naviframe_item_style_set(self.item, _cfruni(style))

    def title_visible_set(self, visible):
        """Show/Hide the title area

        When the title area is invisible, then the controls would be hidden
        so as to expand the content area to full-size.

        @see: L{title_visible_get()}

        @param visible: If C{True}, title area will be visible, hidden
            otherwise
        @type visible: bool

        """
        elm_naviframe_item_title_visible_set(self.item, visible)

    def title_visible_get(self):
        """Get a value whether title area is visible or not.

        @see: L{title_visible_set()}

        @return: If C{True}, title area is visible

        """
        return bool(elm_naviframe_item_title_visible_get(self.item))

    property title_visible:
        """Show/Hide the title area

        When the title area is invisible, then the controls would be hidden
        so as to expand the content area to full-size.

        @type: bool

        """
        def __get__(self):
            return bool(elm_naviframe_item_title_visible_get(self.item))
        def __set__(self, visible):
            elm_naviframe_item_title_visible_set(self.item, visible)

cdef public class Naviframe(LayoutClass) [object PyElementaryNaviframe, type PyElementaryNaviframe_Type]:

    """Naviframe stands for navigation frame. It's a views manager
    for applications.

    A naviframe holds views (or pages) as its items. Those items are
    organized in a stack, so that new items get pushed on top of the
    old, and only the topmost view is displayed at one time. The
    transition between views is animated, depending on the theme
    applied to the widget.

    Naviframe views hold spaces to various elements, which are:
        - back button, used to navigate to previous views,
        - next button, used to navigate to next views in the stack,
        - title label,
        - sub-title label,
        - title icon and
        - content area.

    Becase this widget is a layout, one places content on those areas
    by using L{content_set()} on the right swallow part names
    expected for each, which are:
        - C{"default"} - The main content of the current page
        - C{"icon"} - An icon in the title area of the current page
        - C{"prev_btn"} - A button of the current page to go to the
                         previous page
        - C{"next_btn"} - A button of the current page to go to the next
                         page

    For text, L{text_set()} will work here on:
        - C{"default"} - Title label in the title area of the current
                        page
        - C{"subtitle"} - Sub-title label in the title area of the
                         current page

    Most of those content objects can be passed at the time of an item
    creation (see L{item_push()}).

    Naviframe items can have different styles, which affect the
    transition between views, for example. On the default theme, two of
    them are supported:
        - C{"basic"}   - views are switched sliding horizontally, one after
                      the other
        - C{"overlap"} - like the previous one, but the previous view stays
                      at its place and is ovelapped by the new


    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{"transition,finished"} - When the transition is finished in
                                    changing the item
        - C{"title,clicked"} - User clicked title area

    All the parts, for content and text, described here will also be
    reachable by naviframe B{items} direct calls:
        - L{ObjectItem.part_text_set}
        - L{ObjectItem.part_text_get}
        - L{ObjectItem.part_content_set}
        - L{ObjectItem.part_content_get}
        - L{ObjectItem.part_content_unset}
        - L{ObjectItem.signal_emit}

    What happens is that the topmost item of a naviframe will be the
    widget's target layout, when accessed directly. Items lying below
    the top one can be interacted with this way.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_naviframe_add(parent.obj))

    def item_push(self, title_label, evasObject prev_btn, evasObject next_btn, evasObject content, const_char_ptr item_style):
        """item_push(title_label, prev_btn, next_btn, content, item_style)

        Push a new item to the top of the naviframe stack (and show it).

        The item pushed becomes one page of the naviframe, this item will be
        deleted when it is popped.

        The following styles are available for this item:
            - C{"default"}

        @see: L{NaviframeItem.style_set()}
        @see: L{NaviframeItem.insert_before()}
        @see: L{NaviframeItem.insert_after()}

        @param title_label: The label in the title area. The name of the title
            label part is "elm.text.title"
        @type title_label: string
        @param prev_btn: The button to go to the previous item. If it is None,
            then naviframe will create a back button automatically. The name of
            the prev_btn part is "elm.swallow.prev_btn"
        @type prev_btn: L{Button}
        @param next_btn: The button to go to the next item. Or It could be just an
            extra function button. The name of the next_btn part is
            "elm.swallow.next_btn"
        @type next_btn: L{Button}
        @param content: The main content object. The name of content part is
            "elm.swallow.content"
        @type content: L{Object}
        @param item_style: The current item style name. C{None} would be default.
        @type item_style: string

        @return: The created item or C{None} upon failure.
        @rtype: L{NaviframeItem}

        """
        cdef NaviframeItem ret = NaviframeItem()
        cdef Elm_Object_Item *item

        item = elm_naviframe_item_push(self.obj, _cfruni(title_label),
                                       prev_btn.obj if prev_btn else NULL,
                                       next_btn.obj if next_btn else NULL,
                                       content.obj if content else NULL,
                                       _cfruni(item_style) if item_style else NULL)
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            return None

    def item_insert_before(self, NaviframeItem before, title_label, evasObject prev_btn, evasObject next_btn, evasObject content, const_char_ptr item_style):
        """item_push(before, title_label, prev_btn, next_btn, content, item_style)

        Insert a new item into the naviframe before item C{before}.

        The item is inserted into the naviframe straight away without any
        transition operations. This item will be deleted when it is popped.

        The following styles are available for this item:
            - C{"default"}

        @see: L{NaviframeItem.style_set()}
        @see: L{NaviframeItem.push()}
        @see: L{NaviframeItem.insert_after()}

        @param before: The naviframe item to insert before.
        @type before: L{NaviframeItem}
        @param title_label: The label in the title area. The name of the title
            label part is "elm.text.title"
        @type title_label: string
        @param prev_btn: The button to go to the previous item. If it is None,
            then naviframe will create a back button automatically. The name of
            the prev_btn part is "elm.swallow.prev_btn"
        @type prev_btn: L{Button}
        @param next_btn: The button to go to the next item. Or It could be just an
            extra function button. The name of the next_btn part is
            "elm.swallow.next_btn"
        @type next_btn: L{Button}
        @param content: The main content object. The name of content part is
            "elm.swallow.content"
        @type content: L{Object}
        @param item_style: The current item style name. C{None} would be default.
        @type item_style: string
        @return: The created item or C{None} upon failure.
        @rtype: L{NaviframeItem}

        """
        cdef NaviframeItem ret = NaviframeItem()
        cdef Elm_Object_Item *item

        item = elm_naviframe_item_insert_before(self.obj, before.item, _cfruni(title_label), prev_btn.obj, next_btn.obj, content.obj, _cfruni(item_style))
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            return None

    def item_insert_after(self, NaviframeItem after, title_label, evasObject prev_btn, evasObject next_btn, evasObject content, const_char_ptr item_style):
        """item_push(after, title_label, prev_btn, next_btn, content, item_style)

        Insert a new item into the naviframe after item C{after}.

        The item is inserted into the naviframe straight away without any
        transition operations. This item will be deleted when it is popped.

        The following styles are available for this item:
            - C{"default"}

        @see: L{NaviframeItem.style_set()}
        @see: L{NaviframeItem.push()}
        @see: L{NaviframeItem.insert_before()}

        @param after: The naviframe item to insert after.
        @type after: L{NaviframeItem}
        @param title_label: The label in the title area. The name of the title
            label part is "elm.text.title"
        @type title_label: string
        @param prev_btn: The button to go to the previous item. If it is None,
            then naviframe will create a back button automatically. The name of
            the prev_btn part is "elm.swallow.prev_btn"
        @type prev_btn: L{Button}
        @param next_btn: The button to go to the next item. Or It could be just an
            extra function button. The name of the next_btn part is
            "elm.swallow.next_btn"
        @type next_btn: L{Button}
        @param content: The main content object. The name of content part is
            "elm.swallow.content"
        @type content: L{Object}
        @param item_style: The current item style name. C{None} would be default.
        @type item_style: string
        @return: The created item or C{None} upon failure.
        @rtype: L{NaviframeItem}

        """
        cdef NaviframeItem ret = NaviframeItem()
        cdef Elm_Object_Item *item

        item = elm_naviframe_item_insert_after(self.obj, after.item, _cfruni(title_label), prev_btn.obj, next_btn.obj, content.obj, _cfruni(item_style))
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            return None

    def item_pop(self):
        """item_pop()

        Pop an item that is on top of the stack

        This pops an item that is on the top (visible) of the naviframe,
        makes it disappear, then deletes the item. The item that was
        underneath it on the stack will become visible.

        @see: L{content_preserve_on_pop_get()}

        @return: C{None} or the content object(if the
            L{content_preserve_on_pop_get} is True).
        @rtype: L{NaviframeItem}

        """
        return Object_from_instance(elm_naviframe_item_pop(self.obj))

    def content_preserve_on_pop_set(self, preserve):
        """Preserve the content objects when items are popped.

        @see: L{content_preserve_on_pop_get()}

        @param preserve: Enable the preserve mode if True, disable otherwise
        @type preserve: bool

        """
        elm_naviframe_content_preserve_on_pop_set(self.obj, preserve)

    def content_preserve_on_pop_get(self):
        """Get a value whether preserve mode is enabled or not.

        @see: L{content_preserve_on_pop_set()}

        @return: If C{True}, preserve mode is enabled
        @rtype: bool

        """
        return bool(elm_naviframe_content_preserve_on_pop_get(self.obj))

    property content_preserve_on_pop:
        """Preserve the content objects when items are popped.

        @type: bool

        """
        def __get__(self):
            return bool(elm_naviframe_content_preserve_on_pop_get(self.obj))
        def __set__(self, preserve):
            elm_naviframe_content_preserve_on_pop_set(self.obj, preserve)

    def top_item_get(self):
        """Get a top item on the naviframe stack

        @return: The top item on the naviframe stack or C{None}, if the
            stack is empty
        @rtype: L{NaviframeItem}

        """
        return _object_item_to_python(elm_naviframe_top_item_get(self.obj))

    property top_item:
        """Get a top item on the naviframe stack

        @type: L{NaviframeItem}

        """
        def __get__(self):
            return _object_item_to_python(elm_naviframe_top_item_get(self.obj))

    def bottom_item_get(self):
        """Get a bottom item on the naviframe stack

        @return: The bottom item on the naviframe stack or C{None}, if the
            stack is empty
        @rtype: L{NaviframeItem}

        """
        return _object_item_to_python(elm_naviframe_bottom_item_get(self.obj))

    property bottom_item:
        """Get a bottom item on the naviframe stack

        @type: L{NaviframeItem}

        """
        def __get__(self):
            return _object_item_to_python(elm_naviframe_bottom_item_get(self.obj))

    def prev_btn_auto_pushed_set(self, auto_pushed):
        """Set creating prev button automatically or not

        @see: L{item_push()}

        @param auto_pushed: If C{True}, the previous button(back button) will
            be created internally when you pass the C{None} to the prev_btn
            parameter in L{item_push}
        @type auto_pushed: bool

        """
        elm_naviframe_prev_btn_auto_pushed_set(self.obj, auto_pushed)

    def prev_btn_auto_pushed_get(self):
        """Get a value whether prev button(back button) will be auto pushed or
        not.

        @see: L{item_push()}
        @see: L{prev_btn_auto_pushed_set()}

        @return: If C{True}, prev button will be auto pushed.
        @rtype: bool

        """
        return bool(elm_naviframe_prev_btn_auto_pushed_get(self.obj))

    property prev_btn_auto_pushed:
        """Whether prev button(back button) will be created automatically or
        not.

        @see: L{item_push()}

        @type: bool

        """
        def __get__(self):
            return bool(elm_naviframe_prev_btn_auto_pushed_get(self.obj))
        def __set__(self, auto_pushed):
            elm_naviframe_prev_btn_auto_pushed_set(self.obj, auto_pushed)

    def items_get(self):
        """Get a list of all the naviframe items.

        @return: A tuple of naviframe items or C{None} on failure.
        @rtype: tuple of L{NaviframeItem}s

        """
        return _object_item_list_to_python(elm_naviframe_items_get(self.obj))

    property items:
        """Get a list of all the naviframe items.

        @type: tuple of L{NaviframeItem}s

        """
        def __get__(self):
            return _object_item_list_to_python(elm_naviframe_items_get(self.obj))

    def event_enabled_set(self, enabled):
        """Set the event enabled when pushing/popping items

        If C{enabled} is True, the contents of the naviframe item will
        receives events from mouse and keyboard during view changing such as
        item push/pop.

        @warning: Events will be blocked by calling
            L{freeze_events_set()} internally. So don't call the API
            while pushing/popping items.

        @see: L{event_enabled_get()}
        @see: L{evasObject.freeze_events_set()}

        @param enabled: Events are received when enabled is C{True}, and
            ignored otherwise.
        @type enabled: bool

        """
        elm_naviframe_event_enabled_set(self.obj, enabled)

    def event_enabled_get(self):
        """Get the value of event enabled status.

        @see: L{event_enabled_set()}

        @return: True, when event is enabled
        @rtype: bool

        """
        return bool(elm_naviframe_event_enabled_get(self.obj))

    property event_enabled:
        """Whether the event when pushing/popping items is enabled

        If is True, the contents of the naviframe item will receives events
        from mouse and keyboard during view changing such as item push/pop.

        @warning: Events will be blocked by calling
            L{freeze_events_set()} internally. So don't call the API
            while pushing/popping items.

        @see: L{evasObject.freeze_events_set()}

        @type: bool

        """
        def __get__(self):
            return bool(elm_naviframe_event_enabled_get(self.obj))
        def __set__(self, enabled):
            elm_naviframe_event_enabled_set(self.obj, enabled)

    def item_simple_push(self, evasObject content):
        """item_simple_push(content)

        Simple version of item_push.

        @see: L{item_push}

        """
        cdef NaviframeItem ret = NaviframeItem()
        cdef Elm_Object_Item *item

        item = elm_naviframe_item_simple_push(self.obj, content.obj)
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            return None

    def item_simple_promote(self, evasObject content):
        """item_simple_promote(content)

        Simple version of item_promote.

        @see: L{NaviframeItem.item_promote}

        """
        elm_naviframe_item_simple_promote(self.obj, content.obj)

    def callback_transition_finished_add(self, func, *args, **kwargs):
        """When the transition is finished in changing the item."""
        self._callback_add("transition,finished", func, *args, **kwargs)

    def callback_transition_finished_del(self, func):
        self._callback_del("transition,finished", func)

    def callback_title_clicked_add(self, func, *args, **kwargs):
        """User clicked title area."""
        self._callback_add("title,clicked", func, *args, **kwargs)

    def callback_title_clicked_del(self, func):
        self._callback_del("title,clicked", func)

_elm_widget_type_register("naviframe", Naviframe)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryNaviframe_Type # hack to install metaclass
_install_metaclass(&PyElementaryNaviframe_Type, ElementaryObjectMeta)
