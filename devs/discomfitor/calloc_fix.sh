#!/bin/bash

sed -i 's/calloc(sizeof(\([^,]*\)),\? \([^)]*\))/calloc(\2, sizeof(\1))/g' $@
