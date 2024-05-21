/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_CDEV_H
#define _LINUX_CDEV_H

#include <linux/kobject.h>
#include <linux/kdev_t.h>
#include <linux/list.h>
#include <linux/device.h>

struct file_operations;
struct inode;
struct module;

// 字符设备
struct cdev {
	struct kobject kobj; // kobject管理结构
	struct module *owner; // 字符设备所属的模块
	const struct file_operations *ops; // 文件操作函数
	struct list_head list; // 连接字符设备对应的所有设备文件inode
	dev_t dev; // 起始设备号
	unsigned int count; // 连续的字符设备号数量(从设备号数量)
} __randomize_layout;

void cdev_init(struct cdev *, const struct file_operations *);

struct cdev *cdev_alloc(void);

void cdev_put(struct cdev *p);

int cdev_add(struct cdev *, dev_t, unsigned);

void cdev_set_parent(struct cdev *p, struct kobject *kobj);
int cdev_device_add(struct cdev *cdev, struct device *dev);
void cdev_device_del(struct cdev *cdev, struct device *dev);

void cdev_del(struct cdev *);

void cd_forget(struct inode *);

#endif
