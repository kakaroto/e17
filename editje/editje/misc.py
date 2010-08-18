# Copyright (C) 2010 Samsung Electronics.
#
# This file is part of Editje.
#
# Editje is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Editje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje. If not, see <http://www.gnu.org/licenses/>.

import re


def part_type_to_text(type_):
    parttypes = ['NONE', 'RECTANGLE', 'TEXT', 'IMAGE', 'SWALLOW',
                 'TEXTBLOCK', 'GRADIENT', 'GROUP', 'BOX', 'TABLE',
                 'EXTERNAL']
    return parttypes[type_]


def name_generate(suggest, list=[]):
    # Split the name and the number in end
    spliter = re.compile("((?:(?:\d+\D)*|\D)*)(\d*$)")
    name, number = spliter.match(suggest).groups()

    # to int
    if not number:
        number = 0
    else:
        number = int(number)

    has = False
    for item in list:
        iname, inumber = spliter.match(item).groups()
        if iname != name:
            continue

        if item == suggest:
            has = True

        # to int
        if not inumber:
            inumber = 0
        else:
            inumber = int(inumber)

        # max
        if inumber > number:
            number = inumber

    if has:
        number += 1
        name = name + str(number)
    else:
        name = suggest

    return name

accepted_filetype = re.compile("(.*\.(?:edc|edj))").match

validator_str = re.compile("^(.*)$").match
validator_int = re.compile("^\s*(-?\d+)\s*$").match
validator_int_pos = re.compile("^\s*(\d+)\s*$").match
validator_float = re.compile("^\s*(-?(?:\d+\.?)|(?:\d*\.\d+))\s*$").match
validator_float_pos = re.compile("^\s*((?:\d+\.?)|(?:\d*\.\d+))\s*$").match
validator_geometry = re.compile("^\s*(\d+)\s*\D\s*(\d+)\s*$").match
validator_rgba = re.compile(
    "^\s*(?:(?:(\d+)\s+(\d+)\s+(\d+)(?:\s+(\d+))?)|(?:#(?:[a-f]|[A-F]|[0-9])" \
        "{6}(?:(?:[a-f]||[A-F]|[0-9]){2})?))\s*$").match
