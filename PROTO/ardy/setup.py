#!/usr/bin/env python
# ez_setup part(automatic download of setuptools)
from ez_setup import use_setuptools
use_setuptools('0.6c6')
# import setuptools modules
from setuptools import setup
from distutils.command.install import install as _install
from setuptools.command.egg_info import egg_info as _egg_info
# import other modules
import os
import sys
import shutil

### CHECK IF EDJE_CC IS PRESENT IN PATH
edjecc = 0
for check in os.getenv('PATH').split(':'):
   if os.path.isfile(os.path.join(check, 'edje_cc')):
      edjecc = 1

if edjecc != 1:
   print "edje_cc was not found in your PATH. Exitting."
   sys.exit(1)

### CUSTOM CLASSES FOR SETUPTOOLS TO GET RID OF EGG-INFO
class install(_install):
    sub_commands = [('install_lib',     lambda self:False),
                    ('install_headers', lambda self:False),
                    ('install_scripts', lambda self:True),
                    ('install_data',    lambda self:True),
                    ('install_egg_info', lambda self:False)
                   ]

class egg_info(_egg_info):
    def run(self):
        print "Not creating egg_info."

### FUNCTIONS TO RUN BEFORE SETUPTOOLS PART
## Cleanup the tree
def clean():
   # set permissions
   def setperms(fileto, valueof):
      if os.path.isfile(fileto):
         os.chmod(fileto, valueof)

   # remove file
   def removefile(rmfile):
      if os.path.isfile(rmfile):
         os.remove(rmfile)

   # set the permissions
   setperms("data/ardy/ardy", 0755)
   setperms("data/applications/ardy.desktop", 0644)
   setperms("data/pixmaps/ardy.png", 0644)
   setperms("data/pixmaps/ardy_svg.svg", 0644)

   # remove the edje file
   removefile("data/ardy/Arduino.edj")

## Build a new edje file
def build():
   # make new edj file
   os.system("edje_cc -v -id edj_data -fd edj_data edj_data/Arduino.edc -o data/ardy/Arduino.edj")
   os.chmod("data/ardy/Arduino.edj", 0644)

### RUN THOSE FUNCTIONS
if sys.argv[1] == "build":
   clean()
   build()
elif sys.argv[1] == "clean":
   clean()

### SETUPTOOLS PART
setup(name = 'ardy',
      version = '1.0.0',
      author = 'Toma',
      author_email = 'tomhaste@gmail.com',
      description = 'Simple python-efl program to send/receive serial data to an Arduino',
      keywords = 'arduino efl enlightenment ecore edje python',
      scripts = ["bin/ardy"],
      cmdclass = {"install" : install,
                  "egg_info" : egg_info},
      setup_requires = ["python_evas>=0.2.1", "python_ecore>=0.2.1", "python_edje>=0.2.1"],
      install_requires = ["python_evas>=0.2.1", "python_ecore>=0.2.1", "python_edje>=0.2.1"],
      data_files = [('share/ardy', ["data/ardy/ardy", "data/ardy/Arduino.edj"]), 
                    ('share/pixmaps', ["data/pixmaps/ardy.png", "data/pixmaps/ardy_svg.svg"]), 
                    ('share/applications', ["data/applications/ardy.desktop"]),]
      )
