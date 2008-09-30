#!/bin/sh
DISTRO="openmoko"
MACHINE="gta02"
KERNEL="./tmp.$DISTRO.$MACHINE/deploy/glibc/images/uImage-om-gta02-latest.bin"
ROOTFS=`find ./tmp.$DISTRO.$MACHINE/deploy/glibc/images -name "*.jffs2*" | sort | tail -1`

echo "KERNEL: $KERNEL"
echo "ROOTFS: $ROOTFS"

sleep 5

sudo dfu-util -a kernel -D $KERNEL
sudo duf-util -a rootfs -D $ROOTFS
