# linux experiment

VMLINUZ := /boot/vmlinuz.old

run_debug:
# debug the kernel generated in this Work_Directory
	@qemu-system-x86_64 \
		-kernel ../arch/x86_64/boot/bzImage \
		-initrd ./initramfs.cpio.gz \
		-append "root=/dev/ram rdinit=/init nokaslr console=ttyS0" \
		-serial mon:stdio \
		-smp 2 \
		-m 4096M \
		-s -S

run_boot:
	@qemu-system-x86_64 \
		-kernel ../arch/x86_64/boot/bzImage \
		-initrd ./initramfs.cpio.gz \
		-append "debug=1 root=/dev/ram rdinit=/init initcall_debug console=ttyS0" \
		-serial file:boot.log \
		-smp 2 \
		-m 4096M \

run_init:
# run the smallest linux kernel
	@qemu-system-x86_64 \
		-kernel ./prog/boot/vmlinuz \
		-initrd ./initramfs.cpio.gz \
		-append "root=/dev/ram rdinit=/init console=ttyS0" \
		-smp 2 \
		-m 4096M \
		-serial mon:stdio \
		-nographic

hello_initramfs:
	@mkdir -p ./initramfs/bin ./initramfs/dev
	@sudo bash -c "cp $(VMLINUZ) ./prog/boot/vmlinuz && chmod 777 ./prog/boot/vmlinuz"

	@cd prog && make
	@cp ./prog/init ./initramfs/init
	@chmod 777 ./initramfs/init

# copy busybox
	@cp $(shell which busybox) ./initramfs/bin
	@ln -s ./busybox ./initramfs/bin/sh

# create the dev
#	sudo bash -c "mknod ./initramfs/dev/console c 5 1"
#	sudo bash -c "mknod ./initramfs/dev/ram b 1 0"

# compact the initramfs
	@cd initramfs && \
		find . -print0 \
		| cpio --null -ov --format=newc \
		| gzip -9 > ../initramfs.cpio.gz

sh_initramfs:
	@mkdir -p ./initramfs/bin ./initramfs/dev
	@sudo bash -c "cp $(VMLINUZ) ./prog/boot/vmlinuz && chmod 777 ./prog/boot/vmlinuz"

	@cp ./prog/sh_init ./initramfs/init
	@chmod 777 ./initramfs/init

	@ cp ./prog/test ./initramfs/test

# copy busybox
	@cp $(shell which busybox) ./initramfs/bin
	@./prog/busybox_generate.sh

# create the dev
#	sudo bash -c "mknod ./initramfs/dev/console c 5 1"
#	sudo bash -c "mknod ./initramfs/dev/ram b 1 0"

# compact the initramfs
	@cd initramfs && \
		find . -print0 \
		| cpio --null -ov --format=newc \
		| gzip -9 > ../initramfs.cpio.gz

bootsvg:
	@../scripts/bootgraph.pl boot.log > boot.svg
	@cp boot.svg /mnt/hgfs/share/
	@cp boot.log /mnt/hgfs/share/


clean:
	@bash -c "rm -rf initramfs initramfs.cpio.gz ./prog/boot/*"
	@bash -c "rm -rf boot.log boot.svg"