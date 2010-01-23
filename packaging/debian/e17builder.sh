#!/bin/bash
# e17builder - Script for compiling E17 for packages.enlightenment.org
# license: gpl v3
# author: quaker <quaker66@gmail.com>
# features:
## download
## make chroots
## compile
## prepare
## autoupload

# user variables :)

# here enter your esvn username.
username="USERNAME"
# url of packages, it shouldn't change
eurl="http://packages.enlightenment.org"
# enlightenment server to connect
eserver="e2.enlightenment.org"
# current source version
currentversion=$(wget -O- $eurl/CURRENTVERSION)
# path of sources on the server
path="/var/www/packages/debian/pool/source/sources-$currentversion"
# path of md5sum file on the server
md5path="$eurl/SOURCES-MD5"
# local folder for binaries and sources
localpath="$HOME/repository_folder"
# this is where chroot(packed/unpacked) and final result before tree preparation will be, change if you haven't got much space in /var
pbuilderplace="/var/cache/pbuilder"
# set to 1 if pbuilderplace is in place where you have write permissions.
pbuilderplaceperms="0"
# do apt-get update before installing deps?
doupdate="1"
# install dependencies on --setup?
installdeps="1"
# distro list
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
# karmic - x86*
"karmic#i386"
"karmic#amd64"
# lucid - x86*
"lucid#i386"
"lucid#amd64"
# armel
"hardy#armel"
"intrepid#armel"
"jaunty#armel"
"karmic#armel"
"lucid#armel"
# lpia
"hardy#lpia"
"intrepid#lpia"
"jaunty#lpia"
"karmic#lpia"
"lucid#lpia"
# powerpc
"hardy#powerpc"
"intrepid#powerpc"
"jaunty#powerpc"
"karmic#powerpc"
"lucid#powerpc"
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
# lenny, squeeze, sid - powerpc
"lenny#powerpc"
"squeeze#powerpc"
"sid#powerpc"
)

# list of distros to build database for.
dtb=(
## ubuntu
"hardy"
"intrepid"
"jaunty"
"karmic"
"lucid"
## debian
"lenny"
"squeeze"
"sid"
)

# architectures to make database for
archs=(
"i386"
"amd64"
"armel"
"lpia"
"powerpc"
)

# list of things to compile, comment out things which you don't want to compile, or which are not made for your distro
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
"elementary"
# extra libs
"epsilon"
"esmart"
"emotion"
"epdf"
"ethumb"
"exml"
"etk"
"exchange"
"libeweather"
# extra apps
"ecomp"
"emprint"
"exalt"
"edje_viewer"
"edje_player"
"empower"
"estickies"
"entrance"
"shellementary"
"expedite"
"image-viewer"
"eyesight"
"ephoto"
"eyelight"
"eyelight_edit"
# extra modules
"E-MODULES-EXTRA"
# python stuff
"cython" # ubuntu hardy only
# python bindings
"python-evas"
"python-ecore"
"python-e_dbus"
"python-edje"
"python-emotion"
"python-epsilon"
"python-ethumb"
"python-etk"
"python-elementary"
"python-efl_utils"
# python apps
"editje"
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
echo "DOWNLOADING PACKAGES WITH RSYNC..."
for down in ${compile_list[@]}; do
	case $down in
		eina|eet|evas|ecore|embryo|edje|e_dbus|efreet|e|elementary) rsync --partial --progress --recursive --rsh=ssh $username@$eserver:$path/main/$down ./
		;;
		*) rsync --partial --progress --recursive --rsh=ssh $username@$eserver:$path/extras/$down ./
		;;
	esac
done
echo "DOWNLOADING MD5SUMS..."
wget $md5path
echo "CHECKING MD5SUMS..."
for md5 in ${compile_list[@]}; do
	case $md5 in
		e_dbus) check="edbus"
		;;
		e) check="e17"
		;;
		edje_player) check="edje-player"
		;;
		edje_viewer) check="edje-viewer"
		;;
		E-MODULES-EXTRA) check="emodules"
		;;
		python-e_dbus) check="python-edbus"
		;;
		python-efl_utils) check="python-efl-utils"
		;;
		*) check="$md5"
		;;
	esac
	check()
	{
	if [ "$(md5sum $md5/$check*.$1 | sed 's/  .*//')" = "$(cat SOURCES-MD5 | grep "$md5/$check.*.$1" | sed 's/  .*//')" ]; then
		echo "$md5 $1 file: MD5 OK"
	else
		badchecksums="1"
		while badchecksums="1"; do
			filename=$(ls $md5 | grep "$check.*.$1")
			rm $md5/$filename
			if test $(echo $check | egrep "(^eina$|^eet$|^evas$|^ecore$|^embryo$|^edje$|^edbus$|^efreet$|^e17$)"); then
				rsync --partial --progress --recursive --rsh=ssh $username@$eserver:$path/main/$md5/$filename $md5/$filename
			else
				rsync --partial --progress --recursive --rsh=ssh $username@$eserver:$path/extras/$md5/$filename $md5/$filename
			fi
			if [ "$(md5sum $md5/$check*.$1 | sed 's/  .*//')" = "$(cat SOURCES-MD5 | grep "$md5/$check.*.$1" | sed 's/  .*//')" ]; then
				echo "MD5 OK now."
				badchecksums="0"
				break
			else
				echo "MD5 bad again. How this is possible?"
			fi
		done
	fi
	}
	check dsc
	check diff.gz
	check orig.tar.gz
done
rm SOURCES-MD5
echo "DONE DOWNLOADING..."
}

makechroots() {
echo "UPDATING PBUILDERRC..."
sed -i 's/BINDMOUNTS=/#BINDMOUNTS=/g' $HOME/.pbuilderrc
sed -i 's/OTHERMIRROR=\"deb file/#OTHERMIRROR=\"deb file/g' $HOME/.pbuilderrc
echo "CREATING CHROOTS..."
for chroots in ${distros[@]}; do
	echo "Creating chroot: $(echo $chroots | sed 's/#.*//'):$(echo $chroots | sed 's/.*#//')"
	sudo mkdir -p $pbuilderplace/build/$(echo $chroots | sed 's/#.*//')-$(echo $chroots | sed 's/.*#//')
	sudo DIST=$(echo $chroots | sed 's/#.*//') ARCH=$(echo $chroots | sed 's/.*#//') PBUILDERPLACE="$pbuilderplace" pbuilder create --basetgz $pbuilderplace/$(echo $chroots | sed 's/#.*//')-$(echo $chroots | sed 's/.*#//')-base.tgz --buildplace $pbuilderplace/build --debootstrapopts --include=sysv-rc
	if [ "$?" -ge "1" ]; then
		echo "ERROR, exitting."
		sudo rm -rf $pbuilderplace/build/$(echo $chroots | sed 's/#.*//')-$(echo $chroots | sed 's/.*#//')
		exit 1
	fi
	sudo rm -rf $pbuilderplace/build/$(echo $chroots | sed 's/#.*//')-$(echo $chroots | sed 's/.*#//')
done
echo "REVERTING PBUILDERRC BACK..."
sed -i 's/#*BINDMOUNTS=/BINDMOUNTS=/g' $HOME/.pbuilderrc
sed -i 's/#*OTHERMIRROR=\"deb file/OTHERMIRROR=\"deb file/g' $HOME/.pbuilderrc
}

updatechroots() {
echo "UPDATING CHROOTS..."
for uchroots in ${distros[@]}; do
	echo "Updating chroot: $(echo $uchroots | sed 's/#.*//'):$(echo $uchroots | sed 's/.*#//')"
	sudo mkdir -p $pbuilderplace/build/$(echo $uchroots | sed 's/#.*//')-$(echo $uchroots | sed 's/.*#//')
	sudo DIST=$(echo $uchroots | sed 's/#.*//') ARCH=$(echo $uchroots | sed 's/.*#//') PBUILDERPLACE="$pbuilderplace" pbuilder update --override-config --basetgz $pbuilderplace/$(echo $uchroots | sed 's/#.*//')-$(echo $uchroots | sed 's/.*#//')-base.tgz
	if [ "$?" -ge "1" ]; then
		echo "ERROR, exitting."
		exit 1
	fi
	sudo rmdir $pbuilderplace/build/$(echo $uchroots | sed 's/#.*//')-$(echo $uchroots | sed 's/.*#//')
done
}

setup() {
echo "INSTALLING PBUILDER..."
[ "$doupdate" = "1" ] && sudo apt-get update
[ "$installdeps" = "1" ] && sudo apt-get --assume-yes --force-yes install pbuilder cdebootstrap devscripts ccache rsync openssh-client
echo "INSTALLING UBUNTU KEYRING..."
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
echo "CREATING LOCAL FOLDER..."
if [ -d $localpath ]; then
	echo "WARNING: local folder already exists, delete it or move it or something."
	exit 1
else
	mkdir -p $localpath
fi

echo "CREATING $pbuilderplace IF NOT EXISTS..."
if [ ! -d "$pbuilderplace" ]; then
	sudo mkdir -p $pbuilderplace
fi

if [ "$pbuilderplace" != "/var/cache/pbuilder" ]; then
	sudo mkdir -p $pbuilderplace/{build,ccache,aptcache,pbuildd,pbuilder-mnt,pbuilder-umlresult,result}
	sudo chmod -R 777 $pbuilderplace
fi
}

compile() {
for distrocomp in ${distros[@]}; do
	echo "Unpacking chroot $(echo $distrocomp | sed 's/#.*//'):$(echo $distrocomp | sed 's/.*#//')"
	if [ -d "$pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')" ]; then
		echo "Directory to unpack already exists. Remove or move it first, exitting."
		exit 1
	fi
	sudo mkdir $pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')
	sudo mkdir -p $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/result
	sudo tar -C $pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//') -xf $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')-base.tgz
	if [ "$?" -ge "1" ]; then
		echo "ERROR, exitting."
		exit 1
	fi
	basedir=$(pwd)
	echo "Compiling for: $(echo $distrocomp | sed 's/#.*//'):$(echo $distrocomp | sed 's/.*#//')"
	for comp in ${compile_list[@]}; do
		pbuildpath="$pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')"
		cd $comp
		echo "Compiling: $comp"
		sudo DIST="$(echo $distrocomp | sed 's/#.*//')" ARCH="$(echo $distrocomp | sed 's/.*#//')" PBUILDERPLACE="$pbuilderplace" pbuilder build --debbuildopts "-b" --buildresult $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/result --buildplace $pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//') --no-targz *.dsc
		if [ "$?" -ge "1" ]; then
			echo "ERROR, exitting."
			exit 1
		fi
		cd ..
		sudo rm -rf $pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/tmp/buildd/*
		sudo rm -rf $pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/tmp/satisfydepends*
		sudo mkdir -p $pbuildpath/result/binaries/$comp
		sudo mv $pbuildpath/result/*.deb $pbuildpath/result/binaries/$comp
		echo "Rebuilding packages database."
		cd $pbuildpath/result
		sudo dpkg-scanpackages . /dev/null | sudo tee Packages
		cd $basedir
		sudo DIST="$(echo $distrocomp | sed 's/#.*//')" ARCH="$(echo $distrocomp | sed 's/.*#//')" PBUILDERPLACE="$pbuilderplace" pbuilder update --override-config --buildplace $pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//') --no-targz
		if [ "$?" -ge "1" ]; then
			echo "ERROR, deleting chroot and updating packed, then exitting."
			echo "Deleting chroot."
			sudo rm -rf $pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')
			echo "Updating tarball."
			sudo DIST="$(echo $distrocomp | sed 's/#.*//')" ARCH="$(echo $distrocomp | sed 's/.*#//')" PBUILDERPLACE="$pbuilderplace" pbuilder update --override-config --basetgz $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')-base.tgz
			echo "Done, EXIT."
			exit 1
		fi
	done
	echo "Updating base tarball for $(echo $distrocomp | sed 's/#.*//'):$(echo $distrocomp | sed 's/.*#//') again after compilation.."
	sudo DIST="$(echo $distrocomp | sed 's/#.*//')" ARCH="$(echo $distrocomp | sed 's/.*#//')" PBUILDERPLACE="$pbuilderplace" pbuilder update --override-config --basetgz $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')-base.tgz
	sudo rm -rf $pbuilderplace/build/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')
	sudo rm -rf $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/result/*.orig.tar.gz
	sudo rm -rf $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/result/*.diff.gz
	sudo rm -rf $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/result/*.dsc
	sudo rm -rf $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/result/*.changes
	sudo rm $pbuilderplace/$(echo $distrocomp | sed 's/#.*//')-$(echo $distrocomp | sed 's/.*#//')/result/Packages
done
}

prepare() {
for preparelist in ${distros[@]}; do
	preparepath="$pbuilderplace/$(echo $preparelist | sed 's/#.*//')-$(echo $preparelist | sed 's/.*#//')/result/binaries"
	case $(echo $preparelist | sed 's/#.*//') in
		hardy|intrepid|jaunty|karmic) distro=ubuntu
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
	cp -rf $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/extras/{eina,eet,evas,ecore,embryo,edje,e_dbus,efreet,e,elementary} $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/main
	rm -rf $distro/pool/$(echo $preparelist | sed 's/#.*//')/binaries-$currentversion/extras/{eina,eet,evas,ecore,embryo,edje,e_dbus,efreet,e,elementary}
	echo "Done. If you have binaries for all architectures in the right place, run script with --database. Don't make databases if you have only one arch in the folder, because there are also some files with architecture all, they will be rewritten when you will compile another arch and they will have different checksums."
done
}

database() {
for databases in ${dtb[@]}; do
	case $databases in
		hardy|intrepid|jaunty|karmic) distr=ubuntu
		;;
		*) distr=debian
		;;
	esac
	echo "Making databases."
	# make database for main section
	cd $distr
	for dtbarch in ${archs[@]}; do 
		dpkg-scanpackages --arch $dtbarch pool/$databases/binaries-$currentversion/main /dev/null > Packages
		cp Packages Pkgs
		gzip -f Packages
		mv Pkgs Packages
		mv {Packages,Packages.gz} dists/$databases/main/binary-$dtbarch
		# make database for extras section
		dpkg-scanpackages --arch $dtbarch pool/$databases/binaries-$currentversion/extras /dev/null > Packages
		cp Packages Pkgs
		gzip -f Packages
		mv Pkgs Packages
		mv {Packages,Packages.gz} dists/$databases/extras/binary-$dtbarch
	done
	cd ..
	echo "Done."
done
}

upload() {
up_ubuntu=$(echo "${distros[@]}" | egrep "(hardy|intrepid|jaunty|karmic)")
up_debian=$(echo "${distros[@]}" | egrep "(lenny|squeeze|sid)")
if test "$up_ubuntu"; then
	echo "Uploading ubuntu dir."
	chmod -R 775 ubuntu
	rsync --partial --progress --recursive --rsh=ssh ubuntu $username@$eserver:/var/www/packages
	ssh $username@$eserver "chgrp -R www-data /var/www/packages/ubuntu"
fi
if test "$up_debian"; then
	echo "Uploading debian dir."
	chmod -R 775 debian
	rsync --partial --progress --recursive --rsh=ssh debian $username@$eserver:/var/www/packages
	ssh $username@$eserver "chgrp -R www-data /var/www/packages/debian"
fi
echo "Done uploading."
}

clean() {
for clean in ${distros[@]}; do
	echo "Cleaning local repo for: $(echo $chroots | sed 's/#.*//'):$(echo $chroots | sed 's/.*#//')"
	sudo rm -rf $pbuilderplace/$(echo $chroots | sed 's/#.*//')-$(echo $chroots | sed 's/.*#//')/result/*
	if [ "$?" -ge "1" ]; then
		echo "ERROR, exitting."
		exit 1
	fi
done
}

print_help() {
cat << EOF
parameters:

--setup|-s - configure host system for build
--download|-d - download things
--makechroots|-m - make chroots
--update|-U - update base tarballs
--compile|-c - compile stuff
--prepare|-p - make tree
--database|-D - make databases
--upload|-u - upload things
--clean|-C - removes local repo - run it when you don't need it anymore
--help|-h - show this
EOF
}

# script body

# check for deps, but first try if you don't want to install dependencies and create folder

case $1 in
	--setup|-s) 
		setup
		exit 0
	;;
esac

for deps in sudo rsync ssh dpkg-scanpackages gzip /usr/sbin/pbuilder ccache; do
	which $deps
	if [ "$?" -ge "1" ]; then
		echo "Missing dep: $deps"
		exit 1
	fi
done

cd $localpath
if [ "$?" -ge "1" ]; then
	echo "Failed to change directory to $localpath. It looks that folder does not exist, run script with --setup."
	exit 1
fi

if test "$pbuilderplaceperms" = "1"; then
	mkdir -p $pbuilderplace/aptcache
	mkdir -p $pbuilderplace/build
	mkdir -p $pbuilderplace/pbuildd
	mkdir -p $pbuilderplace/pbuilder-mnt
	mkdir -p $pbuilderplace/pbuilder-umlresult
	mkdir -p $pbuilderplace/result
else
	sudo mkdir -p $pbuilderplace/aptcache
	sudo mkdir -p $pbuilderplace/build
	sudo mkdir -p $pbuilderplace/pbuildd
	sudo mkdir -p $pbuilderplace/pbuilder-mnt
	sudo mkdir -p $pbuilderplace/pbuilder-umlresult
	sudo mkdir -p $pbuilderplace/result
fi

while [ "$1" != "" ]; do
	case $1 in
		--download|-d) download
		;;
		--makechroots|-m) makechroots
		;;
		--update|-U) updatechroots
		;;
		--compile|-c) compile
		;;
		--prepare|-p) prepare
		;;
		--database|-D) database
		;;
		--upload|-u) upload
		;;
		--clean|-C) clean
		;;
		--help|-h) print_help
		;;
		*) 
		echo "Bad argument, run it with --help to see what it can do."
		exit 1
		;;
	esac
	shift
done

exit 0
