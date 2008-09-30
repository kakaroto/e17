#!/bin/sh
DISTRO="openmoko"
MACHINE="gta02"
KERNEL="./tmp.$DISTRO.$MACHINE/deploy/glibc/images/uImage-om-gta02-latest.bin"
ROOTFS=`find ./tmp.$DISTRO.$MACHINE/deploy/glibc/images -name "*.jffs2*" | sort | tail -1`

echo "KERNEL: $KERNEL"
echo "ROOTFS: $ROOTFS"

sleep 5

DFU="tmp.openmoko.om-gta02/deploy/glibc/tools"

sudo $DFU -a kernel -D $KERNEL
sudo $DFU -a rootfs -D $ROOTFS
