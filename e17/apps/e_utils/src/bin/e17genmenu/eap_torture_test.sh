#!/bin/bash

# This script will create the data used for the Eap Torture Test (TM).
# For worst results, run it on a system with thousands of applications 
# installed.

find / -xdev -type f -name '*.desktop' -printf '%h/\n' | uniq | xargs -L 1 -n 1 -P 1 --replace e17genmenu -o -m -d="{}"
