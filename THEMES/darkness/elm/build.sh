#!/bin/sh

edje_cc -id . -id ../elm_default/themes -id ../etk -fd ../fonts  darkness-desktop.edc darkness-elm-desktop.edj
edje_cc -id . -id ../elm_default/themes -id ../etk -fd ../fonts  darkness.edc darkness-elm.edj
