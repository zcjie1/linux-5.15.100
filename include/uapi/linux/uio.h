/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */
/*
 *	Berkeley style UIO structures	-	Alan Cox 1994.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#ifndef _UAPI__LINUX_UIO_H
#define _UAPI__LINUX_UIO_H

#include <linux/compiler.h>
#include <linux/types.h>


struct iovec
{
	/*用户缓冲区基地址 —— BSD uses caddr_t (1003.1g requires void *) */
	void __user *iov_base;

	/*用户缓冲区长度(读取字节数) —— Must be size_t (1003.1g) */
	__kernel_size_t iov_len; 
};

/*
 *	UIO_MAXIOV shall be at least 16 1003.1g (5.4.1.1)
 */
 
#define UIO_FASTIOV	8
#define UIO_MAXIOV	1024


#endif /* _UAPI__LINUX_UIO_H */
