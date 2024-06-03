/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PATH_H
#define _LINUX_PATH_H

struct dentry;
struct vfsmount;

/**
 * 当前文件系统挂载信息 和 欲挂载新文件系统的目录
 * 
 * 用于通过dir_name查找对应的挂载点
 * 
 * 比如对于ext4文件系统下的test文件夹
 * mnt就对应ext4的挂载信息
 * dentry就对应test文件夹
*/
struct path {
	struct vfsmount *mnt; // 挂载信息
	struct dentry *dentry; // path对应的目录项
} __randomize_layout;

extern void path_get(const struct path *);
extern void path_put(const struct path *);

static inline int path_equal(const struct path *path1, const struct path *path2)
{
	return path1->mnt == path2->mnt && path1->dentry == path2->dentry;
}

static inline void path_put_init(struct path *path)
{
	path_put(path);
	*path = (struct path) { };
}

#endif  /* _LINUX_PATH_H */
