#!/bin/bash
# e17builder - Script for compiling E17 for packages.enlightenment.org
# license: gpl v3
# author: quaker66
# features:
## download
## make chroots
## compile
## prepare
## autoupload

# user variables :)

username="USERNAME"
eurl="http://packages.enlightenment.org"
eserver="e2.enlightenment.org"
currentversion=$(wget -O- $eurl/CURRENTVERSION)
path="/var/www/packages/debian/pool/source/sources-$currentversion"
localpath="$HOME/repository_folder"
distros=(
## ubuntu
# hardy - x86*
"hardy#i386"
"hardy#amd64"
# intrepid - x86*
"intrepid#i386"
"intrepid#amd64"
# jaunty - x86*
"jaunty#i386"
"jaunty#amd64"
# hardy, intrepid, jaunty - armel
"hardy#armel"
"intrepid#armel"
"jaunty#armel"
# hardy, intrepid, jaunty - lpia
"hardy#lpia"
"intrepid#lpia"
"jaunty#lpia"
## debian
# lenny - x86*
"lenny#i386"
"lenny#amd64"
# squeeze - x86*
"squeeze#i386"
"squeeze#amd64"
# sid - x86*
"sid#i386"
"sid#amd64"
# lenny, squeeze, sid - armel
"lenny#armel"
"squeeze#armel"
"sid#armel"
)

compile_list=(
# going to release
"eina"
"eet"
"evas"
"ecore"
"embryo"
"edje"
"e_dbus"
"efreet"
"e"
# extra libs
"epsilon"
"esmart"
"emotion"
"exml"
"ewl"
"etk"
"enhance"
"exchange"
"elementary"
# extra apps
"entrance"
"ecomp"
"ecomorph-e17"
"emprint"
"exalt"
"edje_editor"
"elitaire"
"emphasis"
"estickies"
"exhibit"
"expedite"
# extra modules
"E-MODULES-EXTRA"
# python stuff
"cython" # ubuntu hardy only
"virtkey" # debian-only
# python bindings
"python-evas"
"python-ecore"
"python-e_dbus"
"python-edje"
"python-emotion"
"python-epsilon"
"python-etk"
"python-elementary"
"python-efl_utils"
# python apps
"keys"
# canola stuff - ubuntu only
"python-dispatcher"
"lightmediascanner"
"python-lightmediascanner"
"python-downloadmanager"
"python-terra"
"atabake"
"canola-thumbnailer"
"canola-daemon"
"canola2"
)

# functions

download() {
echo "DOWNLOADING PACKAGES WITH SCP..."
scp -r $username@$eserver:$path ./
mv sources-$currentversion/main/* .
mv sources-$currentversion/extras/* .
rm -r sources-$currentversion
}

makechroots() {
echo "UPDATING PBUILDERRC..."
sed -i 's/BINDMOUNTS=/#BINDMOUNTS=/g' $HOME/.pbuilderrc
sed -i 's/OTHERMIRROR=/#OTHERMIRROR=/g' $HOME/.pbuilderrc
echo "CREATING CHROOTS..."
for chroots in ${distros[@]}; do
	echo "Creating chroot: $(echo $chroots | sed 's/#.*//'):$(echo $chroots | sed 's/.*#//')"
	sudo DIST=$(echo $chroots | sed 's/#.*//') ARCH=$(echo $chroots | sed 's/.*#//') pbuilder create
	if [ "$?" -ge "1" ]; then
		echo "ERROR, exitting."
		exit 1
	fi
done
echo "REVERTING PBUILDERRC BACK..."
sed -i 's/#BINDMOUNTS=/BINDMOUNTS=/g' $HOME/.pbuilderrc
sed -i 's/#OTHERMIRROR=/OTHERMIRROR=/g' $HOME/.pbuilderrc
}

compile() {
for distrocomp in ${distros[@]}; do
	echo "Unpacking chroot $(echo $distrocomp | sed 's/#.*//'):$(echo $distrocomp | sed 's/.*#//')"
	if [ -d "/var/cache/pbuilder/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')" ]; then
		echo "Directory to unpack already exists. Remove or move it first, exitting."
		exit 1
	fi
	sudo mkdir /var/cache/pbuilder/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')
	sudo mkdir -p /var/cache/pbuilder/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/result
	sudo tar -C /var/cache/pbuilder/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//') -xf /var/cache/pbuilder/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')-base.tgz
	if [ "$?" -ge "1" ]; then
		echo "ERROR, exitting."
		exit 1
	fi
	basedir=$(pwd)
	echo "Compiling for: $(echo $distrocomp | sed 's/#.*//'):$(echo $distrocomp | sed 's/.*#//')"
	for comp in ${compile_list[@]}; do
		pbuildpath="/var/cache/pbuilder/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')"
		cd $comp
		echo "Compiling: $comp"
		sudo DIST="$(echo $distrocomp | sed 's/#.*//')" ARCH="$(echo $distrocomp | sed 's/.*#//')" pbuilder build --buildplace /var/cache/pbuilder/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//') --no-targz *.dsc
		if [ "$?" -ge "1" ]; then
			echo "ERROR, exitting."
			exit 1
		fi
		cd ..
		sudo rm -rf /var/cache/pbuilder/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/tmp/buildd/*
		sudo rm -rf /var/cache/pbuilder/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/tmp/satisfydepends*
		sudo mkdir -p $pbuildpath/result/binaries/$comp
		sudo mv $pbuildpath/result/*.deb $pbuildpath/result/binaries/$comp
		echo "Rebuilding packages database."
		cd $pbuildpath/result
		sudo dpkg-scanpackages . /dev/null | sudo tee Packages
		cd $basedir
		sudo DIST="$(echo $distrocomp | sed 's/#.*//')" ARCH="$(echo $distrocomp | sed 's/.*#//')" pbuilder update --override-config --buildplace /var/cache/pbuilder/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//') --no-targz
		if [ "$?" -ge "1" ]; then
			echo "ERROR, exitting."
			exit 1
		fi
	done
	echo "Updating base tarball for $(echo $distrocomp | sed 's/#.*//'):$(echo $distrocomp | sed 's/.*#//') again after compilation.."
	sudo DIST="$(echo $distrocomp | sed 's/#.*//')" ARCH="$(echo $distrocomp | sed 's/.*#//')" pbuilder update --override-config
	sudo rm -rf /var/cache/pbuilder/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')
done
}

prepare() {
for preparelist in ${distros[@]}; do
	preparepath="/var/cache/pbuilder/$(echo $preparelist | sed 's/#.*//')-$(echo $preparelist | sed 's/.*#//')/result/binaries"
	case $(echo $preparelist | sed 's/#.*//') in
		hardy|intrepid|jaunty) distro=ubuntu
		;;
		*) distro=debian
		;;
	esac
	# prepare pool tree and make dirs of dists
	echo "Creating dirs.."
	if [ ! -d "$distro/dists/$(echo $preparelist | sed 's/#.*//')/main/binary-$(echo $preparelist | sed 's/.*#//')" ]; then
		mkdir -p $distro/dists/$(echo $preparelist | sed 's/#.*//')/main/binary-$(echo $preparelist | sed 's/.*#//')
	fi
	if [ ! -d "$distro/dists/$(echo $preparelist | sed 's/#.*//')/extras/binary-$(echo $preparelist | sed 's/.*#//')" ]; then
		mkdir -p $distro/dists/$(echo $preparelist | sed 's/#.*//')/extras/binary-$(echo $preparelist | sed 's/.*#//')
	fi
	if [ ! -d "$distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/main" ]; then
		mkdir -p $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/main
	fi
	if [ ! -d "$distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/extras" ]; then
		mkdir -p $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/extras
	fi
	echo "Preparing pool dir..."
	cp -rf $preparepath/* $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/extras
	cp -rf $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/extras/{eina,eet,evas,ecore,embryo,edje,e_dbus,efreet,e} $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/main
	rm -rf $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/extras/{eina,eet,evas,ecore,embryo,edje,e_dbus,efreet,e}
	echo "Making databases."
	# make database for main section
	cd $distro
	dpkg-scanpackages --arch $(echo $preparelist | sed 's/.*#//') pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/main /dev/null | tee Packages
	cp Packages Pkgs
	gzip -f Packages
	mv Pkgs Packages
	mv {Packages,Packages.gz} dists/$(echo $preparelist | sed 's/#.*//')/main/binary-$(echo $preparelist | sed 's/.*#//')
	# make database for extras section
	dpkg-scanpackages --arch $(echo $preparelist | sed 's/.*#//') pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/extras /dev/null | tee Packages
	cp Packages Pkgs
	gzip -f Packages
	mv Pkgs Packages
	mv {Packages,Packages.gz} dists/$(echo $preparelist | sed 's/#.*//')/extras/binary-$(echo $preparelist | sed 's/.*#//')
	echo "Done."
	cd ..
done
}

upload() {
for up in ${distros[@]}; do
	case $(echo $up | sed 's/#.*//') in
		hardy|intrepid|jaunty) distro=ubuntu
		;;
		*) distro=debian
		;;
	esac
	echo "Uploading content."
	scp -r $distro $username@$eserver:/var/www/packages
done
}

print_help() {
cat << EOF
parameters:

--download - download things
--makechroots - make chroots
--compile - compile stuff
--prepare - make tree
--upload - upload things
--help - show this
EOF
}

# script body

# check for deps

for deps in sudo scp dpkg-scanpackages gzip; do
	which $deps
	if [ "$?" -ge "1" ]; then
		echo "Missing dep: $deps"
		exit 1
	fi
done

sudo apt-get install pbuilder debootstrap devscripts

if [ -z "$(dpkg -l | grep ii | grep ubuntu-keyring)" ]; then
	wget http://archive.ubuntu.com/ubuntu/pool/main/u/ubuntu-keyring/ubuntu-keyring_2008.03.04_all.deb
	sudo dpkg -i ubuntu-keyring_2008.03.04_all.deb
	if [ "$?" = "0" ]; then
		echo "Installed ubuntu keyring. Removing deb package."
		rm ubuntu-keyring_2008.03.04_all.deb
	else
		echo "error while installing, exitting."
		exit 1
	fi
fi

if [ -d $localpath ]; then
	echo "WARNING: local folder already exists, if it is created by script, it's OK but if not, it may cause problems."
	read
else
	mkdir -p $localpath
fi

cd $localpath

case $1 in
	--download) download
	;;
	--makechroots) makechroots
	;;
	--compile) compile
	;;
	--prepare) prepare
	;;
	--upload) upload
	;;
	--help) print_help
	;;
esac

exit 0
