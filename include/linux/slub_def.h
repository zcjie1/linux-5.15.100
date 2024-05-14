/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SLUB_DEF_H
#define _LINUX_SLUB_DEF_H

/*
 * SLUB : A Slab allocator without object queues.
 *
 * (C) 2007 SGI, Christoph Lameter
 */
#include <linux/kfence.h>
#include <linux/kobject.h>
#include <linux/reciprocal_div.h>
#include <linux/local_lock.h>

enum stat_item {
	ALLOC_FASTPATH,		/* Allocation from cpu slab */
	ALLOC_SLOWPATH,		/* Allocation by getting a new cpu slab */
	FREE_FASTPATH,		/* Free to cpu slab */
	FREE_SLOWPATH,		/* Freeing not to cpu slab */
	FREE_FROZEN,		/* Freeing to frozen slab */
	FREE_ADD_PARTIAL,	/* Freeing moves slab to partial list */
	FREE_REMOVE_PARTIAL,	/* Freeing removes last object */
	ALLOC_FROM_PARTIAL,	/* Cpu slab acquired from node partial list */
	ALLOC_SLAB,		/* Cpu slab acquired from page allocator */
	ALLOC_REFILL,		/* Refill cpu slab from slab freelist */
	ALLOC_NODE_MISMATCH,	/* Switching cpu slab */
	FREE_SLAB,		/* Slab freed to the page allocator */
	CPUSLAB_FLUSH,		/* Abandoning of the cpu slab */
	DEACTIVATE_FULL,	/* Cpu slab was full when deactivated */
	DEACTIVATE_EMPTY,	/* Cpu slab was empty when deactivated */
	DEACTIVATE_TO_HEAD,	/* Cpu slab was moved to the head of partials */
	DEACTIVATE_TO_TAIL,	/* Cpu slab was moved to the tail of partials */
	DEACTIVATE_REMOTE_FREES,/* Slab contained remotely freed objects */
	DEACTIVATE_BYPASS,	/* Implicit deactivation */
	ORDER_FALLBACK,		/* Number of times fallback was necessary */
	CMPXCHG_DOUBLE_CPU_FAIL,/* Failure of this_cpu_cmpxchg_double */
	CMPXCHG_DOUBLE_FAIL,	/* Number of times that cmpxchg double did not match */
	CPU_PARTIAL_ALLOC,	/* Used cpu partial on alloc */
	CPU_PARTIAL_FREE,	/* Refill cpu partial on free */
	CPU_PARTIAL_NODE,	/* Refill cpu partial from node partial */
	CPU_PARTIAL_DRAIN,	/* Drain cpu partial to node partial */
	NR_SLUB_STAT_ITEMS };

/* Slab Cache CPU本地缓存
 * When changing the layout, make sure freelist and tid are still compatible
 * with this_cpu_cmpxchg_double() alignment requirements.
 */
struct kmem_cache_cpu {
	/** 
	 * 指向被 CPU 本地缓存的 slab 中第一个空闲的对象
	 * Pointer to next available object 
	 */
	void **freelist;	

	/**
	 * 确保kmem_cache_cpu 与当前执行进程的 cpu 是一致的
	 * Globally unique transaction id
	*/
	unsigned long tid;

	struct page *page;	/* The slab from which we are allocating */
#ifdef CONFIG_SLUB_CPU_PARTIAL
	struct page *partial;	/* 备用slab列表——Partially allocated frozen slabs */
#endif
	local_lock_t lock;	/* Protects the fields above */
#ifdef CONFIG_SLUB_STATS
	unsigned stat[NR_SLUB_STAT_ITEMS];
#endif
};

#ifdef CONFIG_SLUB_CPU_PARTIAL
#define slub_percpu_partial(c)		((c)->partial)

#define slub_set_percpu_partial(c, p)		\
({						\
	slub_percpu_partial(c) = (p)->next;	\
})

#define slub_percpu_partial_read_once(c)     READ_ONCE(slub_percpu_partial(c))
#else
#define slub_percpu_partial(c)			NULL

#define slub_set_percpu_partial(c, p)

#define slub_percpu_partial_read_once(c)	NULL
#endif // CONFIG_SLUB_CPU_PARTIAL

/*
 * Word size structure that can be atomically updated or read and that
 * contains both the order and the number of objects that a slab of the
 * given order would contain.
 */
struct kmem_cache_order_objects {
	unsigned int x;
};

/*
 * Slab cache management.
 */
struct kmem_cache {

	// 每个 cpu 拥有一个本地缓存，用于无锁化快速分配释放对象
	struct kmem_cache_cpu __percpu *cpu_slab;

	/* Used for retrieving partial slabs, etc. */

	/**
	 * slab cache 的管理标志位，用于设置 slab 的一些特性
	 * 比如：slab 中的对象按照什么方式对齐，对象是否需要POISON毒化,
	 * 是否插入 red zone 在对象内存周围，是否追踪对象的分配和释放信息 等等
	*/
	slab_flags_t flags;

	// kmem_cache_node中partial列表上slab数目限制(超过则回收empty slab)
	unsigned long min_partial;

	/*
	 * The size of an object including metadata
	 * slab 对象在内存中的真实占用，包括内存对齐填充的字节数，red zone 等等
	*/
	unsigned int size;

	// slab 中对象的实际大小，不包含填充的字节数
	unsigned int object_size;

	// size的乘法逆元
	struct reciprocal_value reciprocal_size;

	// 下一个空闲对象指针(Free pointer)的位置距离对象首地址的偏移(用于poison的情况)
	unsigned int offset;
#ifdef CONFIG_SLUB_CPU_PARTIAL
	/* Number of per cpu partial objects to keep around */
	unsigned int cpu_partial;
#endif

	/**
	 * cache 中的 slab 大小，包括 slab 所需要申请的页面个数，以及所包含的对象个数
	 * 低 16 位表示一个 slab 中所包含的对象总数
	 * 高 16 位表示一个 slab 所占有的内存页个数
	*/
	struct kmem_cache_order_objects oo;

	/* Allocation and freeing of slabs */

	// slab 中所能包含对象以及内存页个数的最大值
	struct kmem_cache_order_objects max;

	 // 内存紧张时，采用 min 的尺寸为 slab 申请内存(而不是oo)
	struct kmem_cache_order_objects min;


	gfp_t allocflags;	/* 伙伴系统申请内存分配标识——gfp flags to use on each alloc */

	// slab cache 的引用计数，为 0 时就可以销毁并释放内存回伙伴系统重
	int refcount;

	// 池化对象的构造函数，用于创建 slab 对象池中的对象
	void (*ctor)(void *);
	unsigned int inuse;		/* Offset to metadata */
	unsigned int align;		/* Alignment */
	unsigned int red_left_pad;	/* Left redzone padding size */
	const char *name;	/* Name (only for display!) */
	struct list_head list;	/* List of slab caches */
#ifdef CONFIG_SYSFS
	struct kobject kobj;	/* For sysfs */
#endif
#ifdef CONFIG_SLAB_FREELIST_HARDENED
	unsigned long random;
#endif

#ifdef CONFIG_NUMA
	/*
	 * Defragmentation by allocating from a remote node.
	 * 初始化为1000
	 */
	unsigned int remote_node_defrag_ratio;
#endif

#ifdef CONFIG_SLAB_FREELIST_RANDOM
	unsigned int *random_seq;
#endif

#ifdef CONFIG_KASAN
	struct kasan_cache kasan_info;
#endif

	unsigned int useroffset;	/* Usercopy region offset */
	unsigned int usersize;		/* Usercopy region size */

	struct kmem_cache_node *node[MAX_NUMNODES];
};

#ifdef CONFIG_SLUB_CPU_PARTIAL
#define slub_cpu_partial(s)		((s)->cpu_partial)
#define slub_set_cpu_partial(s, n)		\
({						\
	slub_cpu_partial(s) = (n);		\
})
#else
#define slub_cpu_partial(s)		(0)
#define slub_set_cpu_partial(s, n)
#endif /* CONFIG_SLUB_CPU_PARTIAL */

#ifdef CONFIG_SYSFS
#define SLAB_SUPPORTS_SYSFS
void sysfs_slab_unlink(struct kmem_cache *);
void sysfs_slab_release(struct kmem_cache *);
#else
static inline void sysfs_slab_unlink(struct kmem_cache *s)
{
}
static inline void sysfs_slab_release(struct kmem_cache *s)
{
}
#endif

void object_err(struct kmem_cache *s, struct page *page,
		u8 *object, char *reason);

void *fixup_red_left(struct kmem_cache *s, void *p);

static inline void *nearest_obj(struct kmem_cache *cache, struct page *page,
				void *x) {
	void *object = x - (x - page_address(page)) % cache->size;
	void *last_object = page_address(page) +
		(page->objects - 1) * cache->size;
	void *result = (unlikely(object > last_object)) ? last_object : object;

	result = fixup_red_left(cache, result);
	return result;
}

/* Determine object index from a given position */
static inline unsigned int __obj_to_index(const struct kmem_cache *cache,
					  void *addr, void *obj)
{
	return reciprocal_divide(kasan_reset_tag(obj) - addr,
				 cache->reciprocal_size);
}

static inline unsigned int obj_to_index(const struct kmem_cache *cache,
					const struct page *page, void *obj)
{
	if (is_kfence_address(obj))
		return 0;
	return __obj_to_index(cache, page_address(page), obj);
}

static inline int objs_per_slab_page(const struct kmem_cache *cache,
				     const struct page *page)
{
	return page->objects;
}
#endif /* _LINUX_SLUB_DEF_H */
