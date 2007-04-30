#!/usr/bin/env python2

import c_ecore_evas

init = c_ecore_evas.init
shutdown = c_ecore_evas.shutdown

engine_type_supported_get = c_ecore_evas.engine_type_supported_get
engine_type_from_name = c_ecore_evas.engine_type_from_name
engine_name_from_type = c_ecore_evas.engine_name_from_type

SoftwareX11 = c_ecore_evas.SoftwareX11
GLX11 = c_ecore_evas.GLX11
XRenderX11 = c_ecore_evas.XRenderX11
FB = c_ecore_evas.FB
DirectFB = c_ecore_evas.DirectFB
Buffer = c_ecore_evas.Buffer

