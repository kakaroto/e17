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

from datetime import datetime

cdef public class Datetime(Object) [object PyElementaryDatetime, type PyElementaryDatetime_Type]:

    """Datetime widget is used to display and input date & time values.

    This widget displays date and time as per the B{system's locale} settings
    (Date includes Day, Month & Year along with the defined separators and Time
    includes Hour, Minute & AM/PM fields). Separator for AM/PM field is ignored.

    The corresponding Month, AM/PM strings are displayed according to the
    system’s language settings.

    Datetime format is a combination of LIBC standard characters like "%%d %%b
    %%Y %%I : %%M  %%p" which, as a whole represents both Date as well as Time
    format.

    Elm_datetime supports only the following sub set of libc date format specifiers:

    B{%%Y} : The year as a decimal number including the century (example: 2011).

    B{%%y} : The year as a decimal number without a century (range 00 to 99)

    B{%%m} : The month as a decimal number (range 01 to 12).

    B{%%b} : The abbreviated month name according to the current locale.

    B{%%B} : The full month name according to the current locale.

    B{%%h} : The abbreviated month name according to the current locale(same as %%b).

    B{%%d} : The day of the month as a decimal number (range 01 to 31).

    B{%%e} : The day of the month as a decimal number (range 1 to 31). single
    digits are preceded by a blank.

    B{%%I} : The hour as a decimal number using a 12-hour clock (range 01 to 12).

    B{%%H} : The hour as a decimal number using a 24-hour clock (range 00 to 23).

    B{%%k} : The hour (24-hour clock) as a decimal number (range 0 to 23). single
    digits are preceded by a blank.

    B{%%l} : The hour (12-hour clock) as a decimal number (range 1 to 12); single
    digits are preceded by a blank.

    B{%%M} : The minute as a decimal number (range 00 to 59).

    B{%%p} : Either 'AM' or 'PM' according to the given time value, or the
    corresponding strings for the current locale. Noon is treated as 'PM'
    and midnight as 'AM'

    B{%%P} : Like %p but in lower case: 'am' or 'pm' or a corresponding string for
    the current locale.

    B{%%c} : The preferred date and time representation for the current locale.

    B{%%x} : The preferred date representation for the current locale without the time.

    B{%%X} : The preferred time representation for the current locale without the date.

    B{%%r} : The complete calendar time using the AM/PM format of the current locale.

    B{%%R} : The hour and minute in decimal numbers using the format %H:%M.

    B{%%T} : The time of day in decimal numbers using the format %H:%M:%S.

    B{%%D} : The date using the format %%m/%%d/%%y.

    B{%%F} : The date using the format %%Y-%%m-%%d.

    (For more reference on the available B{LIBC date format specifiers}, please
    visit the link:
    http://www.gnu.org/s/hello/manual/libc.html#Formatting-Calendar-Time )

    Datetime widget can provide Unicode B{separators} in between its fields
    except for AM/PM field.
    A separator can be any B{Unicode character} other than the LIBC standard
    date format specifiers.( Example: In the format %%b %%d B{,} %%y %%H B{:} %%M
    comma(,) is separator for date field %%d and colon(:) is separator for
    hour field %%H ).

    The default format is a predefined one, based on the system Locale.

    Hour format 12hr(1-12) or 24hr(0-23) display can be selected by setting
    the corresponding user format.

    Datetime supports six fields: Year, Month, Date, Hour, Minute, AM/PM.
    Depending on the Datetime module that is loaded, the user can see
    different UI to select the individual field values.

    The individual fields of Datetime can be arranged in any order according to
    the format set by application.

    There is a provision to set the visibility of a particular field as TRUE/
    FALSE so that B{only time/ only date / only required fields} will be
    displayed.

    Each field is having a default minimum and maximum values just like the daily
    calendar information. These min/max values can be modified as per the
    application usage.

    User can enter the values only in between the range of maximum and minimum.
    Apart from these APIs, there is a provision to display only a limited set of
    values out of the possible values. APIs to select the individual field limits
    are intended for this purpose.

    The whole widget is left aligned and its size grows horizontally depending
    on the current format and each field's visible/disabled state.

    Datetime individual field selection is implemented in a modular style.
    Module can be implemented as a Ctxpopup based selection or an ISE based
    selection or even a spinner like selection etc.

    B{Datetime Module design:}

    The following functions are expected to be implemented in a Datetime module:

    B{Field creation::}
     __________                                            __________
    |          |----- obj_hook() ---------------------->>>|          |
    |          |<<<----------------returns Mod_data ------|          |
    | Datetime |_______                                   |          |
    |  widget  |       |Assign module call backs          |  Module  |
    |   base   |<<<____|                                  |          |
    |          |                                          |          |
    |          |----- field_create() ------------------>>>|          |
    |__________|<<<----------------returns field_obj -----|__________|

    B{Field value setting::}
     __________                                          __________
    |          |                                        |          |
    | Datetime |<<<----------elm_datetime_value_set()---|          |
    |  widget  |                                        |  Module  |
    |   base   |----display_field_value()------------>>>|          |
    |__________|                                        |__________|

    B{del_hook::}
     __________                                          __________
    |          |                                        |          |
    | Datetime |----obj_unhook()-------------------->>>>|          |
    |  widget  |                                        |  Module  |
    |   base   |         <<<-----frees mod_data---------|          |
    |__________|                                        |__________|


    Any module can use the following shared functions that are implemented in
    elm_datetime.c :

    B{field_format_get()} - gives the field format.

    B{field_limit_get()}  - gives the field minimum, maximum limits.

    To enable a module, set the ELM_MODULES environment variable as shown:

    B{export ELM_MODULES="datetime_input_ctxpopup>datetime/api"}

    This widget emits the following signals, besides the ones sent from L{Layout}:
        - B{"changed"} - whenever Datetime field value is changed, this
            signal is sent.
        - B{"language,changed"} - whenever system locale changes, this
            signal is sent.

    """
    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_datetime_add(parent.obj))

    property format:
        """The datetime format. Format is a combination of allowed Libc date format
        specifiers like: "%b %d, %Y %I : %M %p".

        Maximum allowed format length is 64 chars.

        Format can include separators for each individual datetime field except
        for AM/PM field.

        Each separator can be a maximum of 6 UTF-8 bytes.
        Space is also taken as a separator.

        Following are the allowed set of format specifiers for each datetime field.

        B{%%Y} : The year as a decimal number including the century.

        B{%%y} : The year as a decimal number without a century (range 00 to 99).

        B{%%m} : The month as a decimal number (range 01 to 12).

        B{%%b} : The abbreviated month name according to the current locale.

        B{%%B} : The full month name according to the current locale.

        B{%%h} : The abbreviated month name according to the current locale(same as %%b).

        B{%%d} : The day of the month as a decimal number (range 01 to 31).

        B{%%e} : The day of the month as a decimal number (range 1 to 31). single
             digits are preceded by a blank.

        B{%%I} : The hour as a decimal number using a 12-hour clock (range 01 to 12).

        B{%%H} : The hour as a decimal number using a 24-hour clock (range 00 to 23).

        B{%%k} : The hour (24-hour clock) as a decimal number (range 0 to 23). single
             digits are preceded by a blank.

        B{%%l} : The hour (12-hour clock) as a decimal number (range 1 to 12); single
             digits are preceded by a blank.

        B{%%M} : The minute as a decimal number (range 00 to 59).

        B{%%p} : Either 'AM' or 'PM' according to the given time value, or the
             corresponding strings for the current locale. Noon is treated as 'PM'
             and midnight as 'AM'.

        B{%%P} : Like %p but in lower case: 'am' or 'pm' or a corresponding string for
             the current locale.

        B{%%c} : The preferred date and time representation for the current locale.

        B{%%x} : The preferred date representation for the current locale without the time.

        B{%%X} : The preferred time representation for the current locale without the date.

        B{%%r} : The complete calendar time using the AM/PM format of the current locale.

        B{%%R} : The hour and minute in decimal numbers using the format %H:%M.

        B{%%T} : The time of day in decimal numbers using the format %H:%M:%S.

        B{%%D} : The date using the format %%m/%%d/%%y.

        B{%%F} : The date using the format %%Y-%%m-%%d.

        These specifiers can be arranged in any order and the widget will display the
        fields accordingly.

        Default format is taken as per the system locale settings.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_datetime_format_get(self.obj))
        def __set__(self, fmt):
            elm_datetime_format_set(self.obj, _cfruni(fmt))

    property value_max:
        """The upper boundary of a field.

        Year: years since 1900. Negative value represents year below 1900 (year
        value -30 represents 1870). Year default range is from 70 to 137.

        Month: default value range is from 0 to 11.

        Date: default value range is from 1 to 31 according to the month value.

        Hour: default value will be in terms of 24 hr format (0~23)

        Minute: default value range is from 0 to 59.

        @type: datetime.datetime

        """
        def __get__(self):
            cdef tm time
            elm_datetime_value_max_get(self.obj, &time)
            ret = datetime( time.tm_year + 1900,
                            time.tm_mon + 1,
                            time.tm_mday,
                            time.tm_hour,
                            time.tm_min,
                            time.tm_sec)
            return ret

        def __set__(self, newtime):
            cdef tm time
            tmtup = newtime.timetuple()
            time.tm_sec = tmtup.tm_sec
            time.tm_min = tmtup.tm_min
            time.tm_hour = tmtup.tm_hour
            time.tm_mday = tmtup.tm_mday
            time.tm_mon = tmtup.tm_mon - 1
            time.tm_year = tmtup.tm_year - 1900
            time.tm_wday = tmtup.tm_wday
            time.tm_yday = tmtup.tm_yday
            time.tm_isdst = tmtup.tm_isdst
            elm_datetime_value_max_set(self.obj, &time)

    property value_min:
        """The lower boundary of a field.

        Year: years since 1900. Negative value represents year below 1900 (year
        value -30 represents 1870). Year default range is from 70 to 137.

        Month: default value range is from 0 to 11.

        Date: default value range is from 1 to 31 according to the month value.

        Hour: default value will be in terms of 24 hr format (0~23)

        Minute: default value range is from 0 to 59.

        @type: datetime.datetime

        """
        def __get__(self):
            cdef tm time
            elm_datetime_value_min_get(self.obj, &time)
            ret = datetime( time.tm_year + 1900,
                            time.tm_mon + 1,
                            time.tm_mday,
                            time.tm_hour,
                            time.tm_min,
                            time.tm_sec)
            return ret

        def __set__(self, newtime):
            cdef tm time
            tmtup = newtime.timetuple()
            time.tm_sec = tmtup.tm_sec
            time.tm_min = tmtup.tm_min
            time.tm_hour = tmtup.tm_hour
            time.tm_mday = tmtup.tm_mday
            time.tm_mon = tmtup.tm_mon - 1
            time.tm_year = tmtup.tm_year - 1900
            time.tm_wday = tmtup.tm_wday
            time.tm_yday = tmtup.tm_yday
            time.tm_isdst = tmtup.tm_isdst
            elm_datetime_value_min_set(self.obj, &time)

    property field_limit:
        """The field limits of a field.

        Limits can be set to individual fields, independently, except for AM/PM field.
        Any field can display the values only in between these Minimum and Maximum limits unless
        the corresponding time value is restricted from MinTime to MaxTime.
        That is, Min/ Max field limits always works under the limitations of MinTime/ MaxTime.

        There is no provision to set the limits of AM/PM field.

        Type of the field. ELM_DATETIME_YEAR etc.
        Reference to field's minimum value
        Reference to field's maximum value

        type: (Elm_Datetime_Field_Type, int, int)

        """
        def __get__(self):
            cdef int min, max
            cdef Elm_Datetime_Field_Type fieldtype = ELM_DATETIME_YEAR
            elm_datetime_field_limit_get(self.obj, fieldtype, &min, &max)
            return (fieldtype, min, max)

        def __set__(self, value):
            cdef int min, max
            cdef Elm_Datetime_Field_Type fieldtype
            min, max, fieldtype = value
            elm_datetime_field_limit_set(self.obj, fieldtype, min, max)

    property value:
        """The current value of a field.

        Year: years since 1900. Negative value represents year below 1900 (year
        value -30 represents 1870). Year default range is from 70 to 137.

        Month: default value range is from 0 to 11.

        Date: default value range is from 1 to 31 according to the month value.

        Hour: default value will be in terms of 24 hr format (0~23)

        Minute: default value range is from 0 to 59.

        @type: tm

        """
        def __get__(self):
            cdef tm time
            elm_datetime_value_get(self.obj, &time)
            ret = datetime( time.tm_year + 1900,
                            time.tm_mon + 1,
                            time.tm_mday,
                            time.tm_hour,
                            time.tm_min,
                            time.tm_sec)
            return ret

        def __set__(self, newtime):
            cdef tm time
            tmtup = newtime.timetuple()
            time.tm_sec = tmtup.tm_sec
            time.tm_min = tmtup.tm_min
            time.tm_hour = tmtup.tm_hour
            time.tm_mday = tmtup.tm_mday
            time.tm_mon = tmtup.tm_mon - 1
            time.tm_year = tmtup.tm_year - 1900
            time.tm_wday = tmtup.tm_wday
            time.tm_yday = tmtup.tm_yday
            time.tm_isdst = tmtup.tm_isdst
            elm_datetime_value_set(self.obj, &time)

    def field_visible_get(self, fieldtype):
        """Whether a field can be visible/not

        @see: L{field_visible_set()}

        @param fieldtype: Type of the field. ELM_DATETIME_YEAR etc
        @type fieldtype: Elm_Datetime_Field_Type
        @return: C{True}, if field can be visible. C{False} otherwise.
        @rtype: bool

        """
        return bool(elm_datetime_field_visible_get(self.obj, fieldtype))

    def field_visible_set(self, fieldtype, visible):
        """Set a field to be visible or not.

        Setting this API True does not ensure that the field is visible, apart
        from this, the field's format must be present in Datetime overall format.
        If a field's visibility is set to False then it won't appear even though
        its format is present in overall format. So if and only if this API is
        set true and the corresponding field's format is present in Datetime
        format, the field is visible.

        By default the field visibility is set to True.

        @see: elm_datetime_field_visible_get()

        @param fieldtype: Type of the field. ELM_DATETIME_YEAR etc.
        @type fieldtype: Elm_Datetime_Field_Type
        @param visible: C{True} field can be visible, C{False} otherwise.
        @type visible: bool

        """
        elm_datetime_field_visible_set(self.obj, fieldtype, visible)

    def callback_changed_add(self, func, *args, **kwargs):
        """Whenever Datetime field value is changed, this signal is sent."""
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

    def callback_languge_changed_add(self, func, *args, **kwargs):
        """Whenever system locale changes, this signal is sent."""
        self._callback_add("language,changed", func, *args, **kwargs)

    def callback_language_changed_del(self, func):
        self._callback_del("language,changed", func)

_elm_widget_type_register("datetime", Datetime)
