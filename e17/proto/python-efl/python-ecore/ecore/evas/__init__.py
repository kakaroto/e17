#!/usr/bin/env python2

import c_ecore_evas

from c_ecore_evas import shutdown, engine_type_supported_get, \
     engine_type_from_name, engine_name_from_type, SoftwareX11, \
     GLX11, XRenderX11, FB, DirectFB, Buffer, SoftwareX11_16

c_ecore_evas.init()
