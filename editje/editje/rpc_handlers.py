# Copyright (C) 2009 Samsung Electronics.
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

import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer, \
    SimpleXMLRPCRequestHandler

import edje
from edje.edit import EdjeEdit
import ecore

from misc import part_type_to_text


class QueriesHandler(object):
    def __init__(self, edit_grp, port, agent_register, agent_unregister):
        #FIXME: Change localhost to a host received as parameter.
        self._server = EditjeServer(("localhost", port), edit_grp,
                agent_register, agent_unregister)
        self._socket_no = self._server.fileno()

        self._input_fd = ecore.fd_handler_add(
            self._server.fileno(), ecore.ECORE_FD_READ | ecore.ECORE_FD_ERROR,
            self._command_handle)

    def _command_handle(self, fd_handler):
        self._server.handle_request()

        return True

    def delete(self):
        self._input_fd.delete()
        self._server.server_close()


class EditjeServer(SimpleXMLRPCServer):
    def __init__(self, addr, edit_grp, agent_register, agent_unregister,
                 requestHandler=SimpleXMLRPCRequestHandler,
                 logRequests=True, allow_none=True, encoding=None,
                 bind_and_activate=True):
        try:
            SimpleXMLRPCServer.__init__(self, addr=addr,
                                        requestHandler=requestHandler,
                                        logRequests=logRequests,
                                        allow_none=allow_none,
                                        encoding=encoding,
                                        bind_and_activate=bind_and_activate)
        except Exception, e:
            if type(e.args) is tuple and e.args[0] == 98:
                raise SystemExit("error: port %d already in use at localhost" %
                                 addr[1])
        self._edit_grp = edit_grp
        self._agent_register = agent_register
        self._agent_unregister = agent_unregister

    def _dispatch(self, method, params):
        try:
            # forcing the 'export_' prefix on methods that are
            # callable through XML-RPC
            func = getattr(self, 'export_' + method)
        except AttributeError:
            # TODO: inform that to calling process
            raise Exception('method "%s" is not supported' % method)
        else:
            return func(*params)

    def _create_dummy_edje(self, group):
        return EdjeEdit(
            # FIXME: we surely don't want to access the canvas
            # like that, just lack of time now
            self._edit_grp._canvas, file=self._edit_grp.workfile,
            group=group)

    def _edje_check_free(self, editable):
        if editable != self._edit_grp:
            del editable

    def _get_api_signals(self, editable):
        ret = {}

        sigs = [p for p in editable.programs if not p.startswith("@")]
        for s in sigs:
            sinfo = {}
            api = editable.program_get(s).api
            if api == (None, None):
                continue

            sinfo["type"] = "signal"
            sinfo["name"] = api[0]
            sinfo["description"] = api[1]

            ret[s] = sinfo

        return ret

    # exported methods
    def export_agent_register(self, host, port):
        self._agent_register(host, port)
        return True

    def export_agent_unregister(self, host, port):
        self._agent_unregister(host, port)
        return True

    def export_get_groups(self):
        return edje.file_collection_list(self._edit_grp.workfile)

    def export_get_parts(self, group):
        if not self._edit_grp.group:
            return {}

        if self._edit_grp.group != group:
            try:
                editable = self._create_dummy_edje(group)
            except Exception, e:
                return {}
        else:
            editable = self._edit_grp

        ret = {}
        for p in editable.parts:
            pinfo = {}
            part = editable.part_get(p)
            pinfo["type"] = part_type_to_text(part.type)
            pinfo["mouse_events"] = bool(part.mouse_events)

            ret[p] = pinfo

        self._edje_check_free(editable)

        return ret

    def export_get_api_objects(self, group):
        try:
            ret = self.export_get_parts(group)
        except Exception, e:
            return {}

        if self._edit_grp.group != group:
            try:
                editable = self._create_dummy_edje(group)
            except Exception, e:
                return {}
        else:
            editable = self._edit_grp

        for k in tuple(ret.iterkeys()):
            api = editable.part_get(k).api
            if api == (None, None):
                ret.pop(k)
            else:
                ret[k]["type"] = "part"
                ret[k]["name"] = api[0]
                ret[k]["description"] = api[1]
                ret[k].pop("mouse_events")

        ret.update(self._get_api_signals(editable))

        self._edje_check_free(editable)

        return ret

    def export_get_part(self, group, part):
        # TODO: make Java to accept the fscking nil value extension
        if not self._edit_grp.group:
            return {}

        if self._edit_grp.group != group:
            try:
                editable = self._create_dummy_edje(group)
            except Exception, e:
                return {}
        else:
            editable = self._edit_grp

        p = editable.part_get(part)
        if not p:
            self._edje_check_free(editable)
            return {}

        ret = {"type": p.type,
               "source": p.source,
               "states": p.states}

        self._edje_check_free(editable)

        return ret


class ReportsHandler(object):
    def __init__(self, edit_grp):
        self._clients = []
        self._edit_grp = edit_grp

        self._handlers = {self._edit_grp:
                              [("parts.changed",
                                self._parts_changed_event_handler)],
                          self._edit_grp.part:
                              [("part.api.changed",
                                self._parts_changed_event_handler)],
                          self._edit_grp.signal:
                              [("program.api.changed",
                                self._parts_changed_event_handler)]}

        self.server_prefix = "eclipse-efl-sdk."

        self._handlers_register()

    def agent_register(self, host, port):
        for c, (h, p) in self._clients:
            if (h, p) == (host, port):
                return

        c = xmlrpclib.ServerProxy("%s:%d" % (host, port))
        self._clients.append((c, (host, port)))
        getattr(c, self.server_prefix + "partsChanged")([])

    def agent_unregister(self, host, port):
        for c in self._clients:
            if c[1] == (host, port):
                self._clients.remove(c)
                del c
                break

    def _handlers_register(self):
        for obj, event_list in self._handlers.items():
            for event, handler in event_list:
                obj.callback_add(event, handler)

    def _clients_call(self, method_name, *args):
        for c in self._clients:
            try:
                getattr(c[0], self.server_prefix + method_name)(*args)
            except xmlrpclib.ProtocolError:
                self._clients.remove(c)
                del c

    def _event_handlers_get(self):
        return self._handlers

    event_handlers = property(fget=_event_handlers_get)

    # clients' api contract from now on
    def parts_changed(self, parts):
        self._clients_call("partsChanged", parts)

    def _parts_changed_event_handler(self, emissor, data):
        self.parts_changed([])
