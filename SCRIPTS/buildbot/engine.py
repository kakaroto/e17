#!/usr/bin/python

__all__ = (
    "Package", "packages_get", "package_get_by_name",
    "platforms_set", "platforms_get",
    "env_default_set", "env_default_get",
    )

class Node(object):
    def __init__(self, name):
        self.name = name
        self.incoming = 0
        self.outgoing = []


class Graph(object):
    def __init__(self):
        self.nodes = {}

    def node_add(self, name):
        self.nodes[name] = Node(name)

    def relation_add(self, src, dst):
        s = self.nodes[src]
        d = self.nodes[dst]
        s.outgoing.append(d)
        d.incoming += 1

    def roots_get(self):
        r = []
        for n in self.nodes.itervalues():
            if n.incoming == 0:
                r.append(n)
        return r

    def topo_sorted(self):
        roots = self.roots_get()
        sorted = []
        while roots:
            r = roots.pop()
            sorted.append(r.name)
            for c in self.nodes[r.name].outgoing:
                c.incoming -= 1
                if c.incoming == 0:
                    roots.append(c)
            del self.nodes[r.name]
        if self.nodes:
            print "DEPENDENCY LOOP:"
            for n in self.nodes.itervalues():
                print "%s: in=%d" % (n.name, n.incoming),
                for c in n.outgoing:
                    print c.name,
                print
        return sorted

_platforms = []
_env_default = {}
_pkgs = {}
class Package(object):
    def __init__(self, name, dependencies=None, configure_flags=None,
                 env=None, doc_target=None, test_target=None,
                 srcdir=None, exclusive_platforms=None):
        self.name = name
        self.dependencies = dependencies or {}
        self.configure_flags = configure_flags or {}
        self.env = env or {}
        self.doc_target = doc_target
        self.test_target = test_target
        self.srcdir = srcdir or name
        self.exclusive_platforms = exclusive_platforms
        _pkgs[name] = self

    def valid_check(self, platform):
        if self.exclusive_platforms and \
                platform not in self.exclusive_platforms:
            return False
        return True

    def env_get(self, platform):
        r = {}
        def merge(key, value):
            old = r.get(key)
            if old:
                r[key] = "%s %s" % (old, value)
            else:
                r[key] = value

        for k, v in _env_default.get("common", {}).iteritems():
            merge(k, v)
        for k, v in _env_default.get(platform, {}).iteritems():
            merge(k, v)
        for k, v in self.env.get("common", {}).iteritems():
            merge(k, v)
        for k, v in self.env.get(platform, {}).iteritems():
            merge(k, v)
        return r

    def _platform_get(self, varname, platform):
        v = self.__dict__[varname]
        r_common = v.get("common")
        r_platform = v.get(platform)
        if r_common and r_platform:
            return r_common + r_platform
        else:
            return r_common or r_platform or []

    def configure_flags_get(self, platform):
        return self._platform_get("configure_flags", platform)

    def dependencies_get(self, platform):
        return set(self._platform_get("dependencies", platform))

    def all_dependencies_get(self, platform):
        r = set()
        for depname in self.dependencies_get(platform):
            n = _pkgs[depname]
            if n == self or not n.valid_check(platform):
                continue
            r.add(depname)
            n_deps = n.dependencies_get(platform)
            for x in n.all_dependencies_get(platform):
                r.add(x)
        return r

    def users_get(self, platform):
        r = set()
        for p in _pkgs.itervalues():
            if p == self or not p.valid_check(platform):
                continue
            if self.name in p.dependencies_get(platform):
                r.add(p.name)
        return r

    def all_users_get(self, platform):
        r = set()
        for uname in self.users_get(platform):
            n = _pkgs[uname]
            if n == self or not n.valid_check(platform):
                continue
            if self.name in n.dependencies_get(platform):
                r.add(n.name)
                for x in n.all_users_get(platform):
                    r.add(x)
        return r

    def all_dependencies_sorted_get(self, platform):
        deps = self.all_dependencies_get(platform)
        g = Graph()
        g.node_add(self.name)
        for dep in deps:
            g.node_add(dep)

        for src in deps:
            for dst in _pkgs[src].dependencies_get(platform):
                g.relation_add(src, dst)

        sorted = g.topo_sorted()
        sorted.remove(self.name)
        return sorted

    def all_users_sorted_get(self, platform):
        users = self.all_users_get(platform)
        g = Graph()
        g.node_add(self.name)
        for user in users:
            g.node_add(user)

        for src in users:
            for dst in _pkgs[src].users_get(platform):
                g.relation_add(src, dst)

        sorted = g.topo_sorted()
        sorted.remove(self.name)
        return sorted


def packages_get(platform):
    g = Graph()
    plat_pkgs = [p for p in _pkgs.itervalues() if p.valid_check(platform)]

    for p in plat_pkgs:
        g.node_add(p.name)

    for p in plat_pkgs:
        for u in p.users_get(platform):
            g.relation_add(p.name, u)

    sorted = g.topo_sorted()
    return [_pkgs[pname] for pname in sorted]


def package_get_by_name(name, platform):
    p = _pkgs.get(name)
    if p and p.valid_check(platform):
        return p
    return None

def platforms_set(value):
    global _platforms
    lst = list(value)
    lst.sort()
    _platforms = lst

def platforms_get():
    return _platforms

def env_default_set(value):
    global _env_default
    _env_default = value

def env_default_get(value):
    return _env_default
