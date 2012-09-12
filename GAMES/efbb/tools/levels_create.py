#!/usr/bin/env python
# -*- coding: utf-8 -*-

# AUTHOR: Bruno Dilly <bdilly@profusion.mobi>
# COPYRIGHT: ProFUSION Embedded Systems

"""
Script to create levels EET file from config files.
"""

import sys
import os
import os.path
import subprocess
import tempfile
import ConfigParser


def perror(msg):
    print "Error:", msg


def usage():
    print "Usage:"
    print "\t", sys.argv[0], "full_path_to_levels full_path_to_eet_file"


def eet_prop_write(f, key, value):
    if type(value) is str:
        f.write('value "%s" string: "%s";\n' % (key, value));
    elif type(value) is int:
        f.write('value "%s" int: %s;\n' % (key, value));
    else:
        perror("Property %s: %s of not handled type: %s" %
                (key, value, type(value)))
        return False
    return True


def eet_struct_write(f, struct):
    name = struct.pop("name", None)
    if not name:
        perror("Can't add struct, no name was provided")
        return False
    f.write('group "%s" struct {\n' % name)

    for key, value in struct.items():
        if type(value) is list:
            f.write('group "%s" list {\n' % key)
            for item in value:
                if not eet_struct_write(f, item):
                    return False
            f.write('}\n')
        else:
            if not eet_prop_write(f, key, value):
                return False

    f.write('}\n')
    return True


def eet_desc_write(levels):
    fd, tmp_filename = tempfile.mkstemp()
    f = os.fdopen(fd, 'w')
    r = eet_struct_write(f, levels)
    f.close()
    if r:
        return tmp_filename
    return None


def eet_create(eet_desc_filename, eet_filename):
    subprocess.check_output(
        "eet -e %s world %s 0; exit 0" %
        (eet_filename, eet_desc_filename),
        stderr = subprocess.STDOUT,
        shell = True)


def eval_value(value, consts):
    fvalue = ""
    parts = value.split()
    for part in parts:
        fvalue = fvalue + consts.get(part.lower(), part)
    try:
        fvalue = eval(fvalue)
    except NameError:
        return value
    return fvalue


def block_add(rblock, consts):
    block = {}
    block["name"] = "Block"
    int_props = ["w", "h", "x", "y", "mass", "relto"]

    btype = rblock[0].split()[0]
    block["type"] = consts.get(btype.lower(), btype)

    for i in range(len(int_props)):
        value = eval_value(rblock[i + 1], consts)
        block[int_props[i]] = value

    return block


def target_add(rtarget, consts):
    target = {}
    target["name"] = "Target"
    int_props = ["w", "h", "x", "y"]

    ttype = rtarget[0].split()[0]
    target["type"] = consts.get(ttype.lower(), ttype)

    for i in range(len(int_props)):
        value = eval_value(rtarget[i + 1], consts)
        target[int_props[i]] = value

    return target

def object_add(robject, consts):
    object = {}
    object["name"] = "Object"
    int_props = ["w", "h", "x", "y", "above"]

    ttype = robject[0].split()[0]
    object["type"] = consts.get(ttype.lower(), ttype)

    for i in range(len(int_props)):
        value = eval_value(robject[i + 1], consts)
        object[int_props[i]] = value

    return object

def level_add(filename):
    consts = {}
    level = {}
    level["name"] = "Level"
    level["targets"] = []
    level["blocks"] = []
    level["objects"] = []

    print "Creating level %s..." % filename

    config = ConfigParser.RawConfigParser()
    config.read(filename)

    if not config.has_section("General") or not config.has_section("Elements"):
        perror("Missing obligatory sections: General and Elements")
        return None
    if not config.has_option("Elements", "targets") or \
        not config.has_option("Elements", "targets"):
        perror("Missing obligatory lists: targets and blocks")
        return None

    if config.has_section("Consts"):
        for key, value in config.items("Consts"):
            consts[key] = value

    for key, value in config.items("General"):
        value = eval_value(value, consts)
        level[key] = value

    raw_blocks = config.get("Elements", "blocks").split("/")
    for raw_block in raw_blocks:
        block = raw_block.split(",")
        if len(block) < 7:
            perror("Few arguments to build block")
            print block
            return None
        level["blocks"].append(block_add(block, consts))

    raw_targets = config.get("Elements", "targets").split("/")
    for raw_target in raw_targets:
        target = raw_target.split(",")
        if len(target) < 5:
            perror("Few arguments to build target")
            print target
            return None
        level["targets"].append(target_add(target, consts))

    if not config.has_option("Elements", "objects"):
        return level

    raw_objects = config.get("Elements", "objects").split("/")
    for raw_object in raw_objects:
        object = raw_object.split(",")
        if len(object) < 6:
            perror("Few arguments to build object")
            print object
            return None
        level["objects"].append(object_add(object, consts))

    return level


def main(root, eet_filename):
    levels = {}
    levels["name"] = "World"
    levels["levels"] = []
    tmp_lvls = []

    print "Looking for levels at:", root

    if not os.path.isdir(root):
        perror("%s isn't a valid path. Wrong arguments order ?" % root)
        return

    if os.path.isdir(eet_filename):
       perror("%s is a directory." % root)
       return

    if not os.path.isfile(os.path.join(root, "levels.list")):
       perror("File levels.list not found")
       return

    for dirname, dirnames, filenames in os.walk(root):
        for filename in filenames:
            name, ext = os.path.splitext(filename)
            if ext == '.cfg':
                level = level_add(os.path.join(dirname, filename))
                if level:
                    tmp_lvls.append(level)

    f = open(os.path.join(root, 'levels.list'), 'r')
    for ordered_level in f:
        added = False
        for level in tmp_lvls:
            if level["level_id"] == ordered_level.strip():
                levels["levels"].append(level)
                tmp_lvls.remove(level)
                added = True
        if not added:
            perror("No such level: %s" % ordered_level.strip())
            sys.exit(-1)
    if tmp_lvls:
        perror("Warning: Levels not used:")
        for level in tmp_lvls:
            print "%s" %level["level_id"]
    f.close()

    desc_filename = eet_desc_write(levels)
    if desc_filename:
        eet_create(desc_filename, eet_filename)
        os.unlink(desc_filename)
        print "Done! File %s created with success." % eet_filename


if __name__ == "__main__":
    if len(sys.argv) != 3:
        usage()
        sys.exit(1)

    main(sys.argv[1], sys.argv[2])
