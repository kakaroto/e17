#!/usr/bin/python

import sys, re, os

try:
    infile = open(sys.argv[1], "rb")
except IndexError, e:
    raise SystemExit("missing input file name")
except OSError, e:
    raise SystemExit("could not open file '%s': %s" % (argv[1], e))

rec = re.compile

err = 1
wrn = 2

codenames = {
    err: "ERROR",
    wrn: "WARNING",
}

model = (
    # regexp, type, message
    (r"\s+$", err, "trailing whitespaces."),
    (r"^\S+[{]", err, "braces should be in their own lines."),
    (r"^[ ]{8}", err, "8 spaces must be converted to one TAB."),
    (r" \t", err, "TAB should not be preceded by space."),
    (r"^\s.{80,}", wrn, "line exceeds 80 characters and is not a function declaration."),
    )

comp = []
for i, (exp, code, msg) in enumerate(model):
    comp.append((i, re.compile(exp), code, msg))
comp = tuple(comp)

use_color = os.environ.get("COLOR", "1")
# based on codenames!
if int(use_color):
    color = {
        "ERROR": "\033[1;31m",
        "WARNING": "\033[1;33m",
        "HILIGHT": "\033[1m",
        "ADD": "\033[1;36m",
        "DEL": "\033[1;35m",
        "CLEAR": "\033[0m",
        }
else:
    color = {
        "ERROR": "",
        "WARNING": "",
        "HILIGHT": "",
        "ADD": "",
        "DEL": "",
        "CLEAR": "",
        }

def parse_hunk(line):
    line = line.strip()
    try:
        a, b = line.split(" ")
    except ValueError:
        raise SystemExit("invalid hunk format '%s'" % line)
    if a.startswith("-"):
        prev = a[1:]
        cur = b[1:]
    else:
        prev = b[1:]
        cur = a[1:]

    try:
        prev_start, prev_size = prev.split(",")
    except ValueError:
        raise SystemExit("invalid hunk region format '%s'" % prev)
    try:
        cur_start, cur_size = cur.split(",")
    except ValueError:
        raise SystemExit("invalid hunk region format '%s'" % cur)

    return ((int(prev_start), int(prev_size)),
            (int(cur_start), int(cur_size)))

issues = {
    err: 0,
    wrn: 0,
}

cur_file = None
cur_hunk = None
cur_hunk_add = 0
cur_hunk_del = 0
for i, line in enumerate(infile):
    if line.startswith("@@ "):
        idx = line.index(" @@")
        cur_hunk = parse_hunk(line[3:idx])
        cur_hunk_add = 0
        cur_hunk_del = 0
    elif line.startswith("Index: "):
        cur_file = line[7:]
    elif line.startswith("--- ") or line.startswith("+++ "):
        pass
    else:
        try:
            op = line[0]
        except IndexError, e:
            continue
        line = line[1:-1] # remove op and trailing \n

        if cur_file is None:
            continue

        if op == "-":
            cur_hunk_del += 1
            continue
        elif op == ' ':
            cur_hunk_add += 1
            cur_hunk_del += 1
        if op == "+":
            for j, exp, code, msg in comp:
                m = exp.search(line)
                if not m:
                    continue
                codename = codenames[code]
                pref = "%s%s%s" % (color[codename], codename, color["CLEAR"])

                print "%s:%s%d%s: %s" % (
                    pref, color["HILIGHT"], i, color["CLEAR"], msg)
                print "%d:'%s%s%s%s'" % (
                    i, color["HILIGHT"], op, line, color["CLEAR"])
                print "context:"
                print "   hunk: %s-%d,%d %s+%d,%d%s" % (
                    color["DEL"],
                    cur_hunk[0][0], cur_hunk[0][1],
                    color["ADD"],
                    cur_hunk[1][0], cur_hunk[1][1],
                    color["CLEAR"])

                add_line = cur_hunk[1][0] + cur_hunk_add
                del_line = cur_hunk[0][0] + cur_hunk_del

                print "   file: %s-%d %s+%d%s: %s%s%s" % (
                    color["DEL"], del_line,
                    color["ADD"], add_line,
                    color["CLEAR"],
                    color["HILIGHT"], cur_file,
                    color["CLEAR"])

                issue = issues[code]
                issues[code] = issue + 1

            cur_hunk_add += 1

infile.close()

sep_line = None
for code, count in issues.iteritems():
    if count == 0:
        continue
    if sep_line is None:
        sep_line = 1
        print "-" * 72
    codename = codenames[code]
    print "%s%s%s: %d issues" % (
        color[codename], codename, color["CLEAR"], count)

