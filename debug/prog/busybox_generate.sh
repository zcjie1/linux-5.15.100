#!/bin/bash
for i in $(./initramfs/bin/busybox --list); do
    # 检查当前命令是否为 'busybox'
    if [ "$i" != "busybox" ]; then
        # 如果不是 'busybox'，则创建软链接
        ln -s ./busybox ./initramfs/bin/$i
    fi
done