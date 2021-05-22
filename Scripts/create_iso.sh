#!/bin/bash

if [[ $# -ne 1 ]]; then
    1>&2 echo "Usage : sudo ./Scripts/create_iso.sh PATH_DISK"
    exit 1
fi

DEVICE=$1
# SIZE_BYTES="3619836K"
# ROOTFS_KB=3619836
ROOTFS_KB=3564544
BOOT_KB=262144
TOTAL_KB=$(( $ROOTFS_KB + $BOOT_KB ))

# Check the filesystem
umount "$1"*
e2fsck -f "${DEVICE}2"
if [[ $? -ne 0 ]]; then
    echo "File system seems corrupted, aborting" >&2
    exit 1
fi

# Resize the filesystem
resize2fs "${DEVICE}2" "${ROOTFS_KB}K"
if [[ $? -ne 0 ]]; then
    echo "Filesystem resizing failed, aborted" >&2
    exit 1
fi


# Resize the partition
RET=0
echo "d\np\nw" | fdisk ${DEVICE}
RET=$(( ${RET} + $? ))
echo "n\np\nw" | fdisk ${DEVICE}
RET=$(( ${RET} + $? ))
if [[ $RET -ne 0 ]]; then
    echo "Partition resizing failed, aborted" >&2
    exit 1
fi

# Dump the partition
dd status=progress bs=4M if=${DEVICE} of=raciderry.img
if [[ $RET -ne 0 ]]; then
    echo "Image dump failed, aborted" >&2
    exit 1
fi

# Reduce the img size
qemu-img resize raciderry.img "${TOTAL_KB}K"
if [[ $? -ne 0 ]]; then
    echo "Image resize failed, aborted" >&2
    rm -f raciderry.img
    exit 1
fi

# Compress the image size
gzip raciderry.img -k --best > raciderry.img.gz

