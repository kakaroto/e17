#!/bin/sh

if [ $# != 5 ]; then
  echo "Usage: build_icon.sh app_name app_exe window_class window_name icon_name";
  exit;
fi

cp $5 /tmp/icon_source.png

echo "data { \
item: \"app/name\" \"$1\";\
item: \"app/exe\" \"$2\";\
item: \"app/window/class\" \"$3\";\
item: \"app/window/name\" \"$4\";\
}\
images { image: \"/tmp/icon_source.png\" COMP; }\
collections { group {\
name: \"icon\";\
max: 48 48;\
parts { part {\
name: \"image\";\
mouse_events: 0;\
description {\
state: \"default\" 0.0;\
aspect: 1.0 1.0;\
image.normal: \"/tmp/icon_source.png\";
} } } } }" > /tmp/icon_sources.edc

edje_cc /tmp/icon_sources.edc $1.eet
