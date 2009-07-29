#!/bin/sh
#DISTRO="openmoko"
DISTRO="angstrom"
MACHINE="om-gta02"
# 2.6.28 - bug bug bugs! panic!
KERNEL="./t-$MACHINE.$DISTRO/deploy/glibc/images/om-gta02/uImage-om-gta02-latest.bin"
ROOTFS=`find ./t-$MACHINE.$DISTRO/deploy/glibc/images -name "*.jffs2" | sort | tail -1`

echo "KERNEL: $KERNEL"
echo "ROOTFS: $ROOTFS"

sleep 5

DFU="./t-$MACHINE.$DISTRO/deploy/glibc/tools/dfu-util"

sudo $DFU -d 1d50:5119 -a kernel -D $KERNEL
sudo $DFU -d 1d50:5119 -a rootfs -D $ROOTFS
