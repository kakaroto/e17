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

import os
import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer, \
    SimpleXMLRPCRequestHandler, Fault

import edje
import ecore

from misc import part_type_to_text


class QueriesHandler(object):
    def __init__(self, edit_grp, port):
        self._server = EditjeServer(("localhost", port), edit_grp)
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
    def __init__(self, addr, edit_grp,
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

    # exported methods
    def export_get_groups(self):
        return edje.file_collection_list(self._edit_grp.workfile)

    def export_get_parts(self):
        if not self._edit_grp.group:
            return None

        ret = {}
        for p in self._edit_grp.parts:
            pinfo = {}
            part = self._edit_grp.part_get(p)
            pinfo["type"] = part_type_to_text(part.type)
            pinfo["mouse_events"] = bool(part.mouse_events)

            ret[p] = pinfo

        return ret

    def export_get_api_objects(self):
        return "to be implemented"

    def export_get_part(self, part):
        # TODO: make Java to accept the fscking nil value extension
        if not self._edit_grp.group:
            return None

        p = self._edit_grp.part_get(part)
        if not p:
            return None

        ret = {"type": p.type,
               "source": p.source,
               "states": p.states}

        return ret


class ReportsHandler(object):
    def __init__(self, edit_grp, ports):
        if not ports:
            raise TypeError("ReportsHandler must be created with one or " +
                            "more writing ports")

        self._clients = []
        self._edit_grp = edit_grp
        for p in ports:
            self._clients.append(xmlrpclib.ServerProxy(
                    "http://localhost:" + str(p)))

        self._handlers = {self._edit_grp:
                              [("parts.changed",
                                self._parts_changed_event_handler)]}

        self._handlers_register()

    def _handlers_register(self):
        for obj, event_list in self._handlers.items():
            for event, handler in event_list:
                obj.callback_add(event, handler)

    def _clients_call(self, method_name, *args):
        prefix = "eclipse-efl-sdk."

        for c in self._clients:
            if args:
                getattr(c, prefix + method_name)(*args)
            else:
                getattr(c, prefix + method_name)()

    def _event_handlers_get(self):
        return self._handlers

    event_handlers = property(fget=_event_handlers_get)

    # clients' api contract from now on
    def parts_changed(self, parts):
        self._clients_call("partsChanged", parts)

    def _parts_changed_event_handler(self, emissor, data):
        self.parts_changed(data)


