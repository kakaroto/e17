#!/bin/sh
edje_cc $@ -id . -fd . emphasis.edc -o emphasis.eap
enlightenment_eapp \
emphasis.eap \
-set-name "Emphasis" \
-set-comment "MPD Client" \
-set-exe "emphasis" \
-set-win-name "emphasis" \
-set-win-class "Emphasis" \
-set-icon-class "Emphasis"
