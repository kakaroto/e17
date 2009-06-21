#!/usr/bin/env python
# Shellementary installation script
# import needed modules
import sys
import os
import re
import shutil
from optparse import OptionParser

# set destdir - for packaging
destdir = os.getenv("DESTDIR")
# translations list
translations = ["cs"]

# build shellementary from shellementary.in
def build(prefix):
	# make script
	input = open("shellementary.in", "r")
	content = re.sub("@prefix@", prefix, input.read())
	output = open("shellementary", "w")
	output.write(content)
	os.chmod("shellementary", 0755)
	# compile translations
	for translation in translations:
		os.system("msgfmt po/"+translation+".po -o po/"+translation+".mo")

# finally install
def install(destdir, prefix):
	# prepare paths
	if destdir:
		prefix = re.sub("^/", "", prefix)
	path = os.path.join(destdir, prefix)
	datapath = os.path.join(path, "share")
	binpath = os.path.join(path, "bin")
	# create dirs
	if not os.path.isdir(datapath):
		os.makedirs(datapath)
	if not os.path.isdir(binpath):
		os.makedirs(binpath)
	# set permissions to have everything right
	os.chmod("shellementary", 0755)
	for pngs in os.listdir("data/shellementary"):
		os.chmod("data/shellementary/"+pngs, 0644)
	# copy files
	shutil.copytree("data/shellementary", os.path.join(datapath, "shellementary"))
	shutil.copy("shellementary", binpath)

	# install translations
	for translation in translations:
		if not os.path.isdir(os.path.join(datapath, "locale", translation, "LC_MESSAGES")):
			os.makedirs(os.path.join(datapath, "locale", translation, "LC_MESSAGES"))
		shutil.copy("po/"+translation+".mo", os.path.join(datapath, "locale", translation, "LC_MESSAGES", "shellementary.mo"))

# check for deps
def check():
	try:
		import elementary
	except:
		print "Missing dependency: elementary"
		sys.exit(1)
	try:
		import evas
	except:
		print "Missing dependency: evas"
		sys.exit(1)
	try:
		import gettext
	except:
		print "Missing dependency: gettext"
		sys.exit(1)

# clean
def clean():
	# if exists, remove generated script
	if os.path.isfile('shellementary'):
		os.remove('shellementary')
	# remove translations
	for translation in translations:
		if os.path.isfile('po/'+translation+'.mo'):
			os.remove('po/'+translation+'.mo')

# parse options
usage = "usage: %prog [options] build|install|clean"
parser = OptionParser(usage=usage)
parser.add_option("-p", "--prefix", dest="prefix", help="Set installation prefix.", default="/usr")
options, args = parser.parse_args()

# run everything
if args[0] == "build":
	check()
	clean()
	build(options.prefix)
elif args[0] == "install":
	install(destdir, options.prefix)
elif args[0] == "clean":
	clean()
else:
	print "Unsupported argument: "+str(args[0])
	sys.exit(1)

sys.exit(0)

