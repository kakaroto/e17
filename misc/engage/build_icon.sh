#!/bin/sh

if [ $# != 5 ]; then
  echo "Usage: build_icon.sh app_name app_exe window_class window_name icon_name";
  exit;
fi

cp $5 /tmp/engage_icon.png

echo "data { \
item: \"app/name\" \"$1\";\
item: \"app/exe\" \"$2\";\
item: \"app/window/class\" \"$3\";\
item: \"app/window/name\" \"$4\";\
}\
images { image: \"/tmp/engage_icon.png\" COMP; }\
collections { group {\
name: \"icon\";\
max: 48 48;\
parts { part {\
name: \"image\";\
mouse_events: 0;\
description {\
state: \"default\" 0.0;\
aspect: 1.0 1.0;\
image.normal: \"/tmp/engage_icon.png\";
} } } } }" > /tmp/engage_icon.edc

edje_cc /tmp/engage_icon.edc
cp /tmp/engage_icon.eet "$HOME/.e/apps/engage/mapping/$1.eet"
cp /tmp/engage_icon.eet "$HOME/.e/apps/engage/launcher/$1.eet"

echo "$1.eet installed to your engage bar, it should appear now - to remove it
from the launch area remove ~/.e/apps/engage/launcher/$1.eet"
