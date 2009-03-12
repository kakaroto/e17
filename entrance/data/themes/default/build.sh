#!/bin/sh
# for building a theme without recompiling entrance
edje_cc $@ -id img -fd . default.edc -o default.edj
