#!/usr/bin/env python2

import c_ecore_imf

from c_ecore_imf import shutdown, available_ids, available_ids_by_canvas_type, \
    default_id, default_id_by_canvas_type, info_by_id

c_ecore_imf.init()
