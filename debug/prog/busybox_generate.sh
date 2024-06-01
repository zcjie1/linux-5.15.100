#! /bin/sh
for i in $(./initramfs/bin/busybox --list); do ln -s ./busybox ./initramfs/bin/$i; done