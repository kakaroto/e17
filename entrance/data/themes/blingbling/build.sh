#!/bin/sh
# for building a theme without recompiling entrance
edje_cc $@ -id img -fd . blingbling.edc -o blingbling.edj
