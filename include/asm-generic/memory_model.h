/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_MEMORY_MODEL_H
#define __ASM_MEMORY_MODEL_H

#include <linux/pfn.h>

#ifndef __ASSEMBLY__

/**
 * supports 3 memory models. DISCONTIGMEM内存模型已被移除(2021)
 * 
 * FLAT内存模型:
 *  1. page结构体所处的虚拟内存连续
 *  2. page结构体对应的物理页的物理内存页连续
 *  3. page结构体对应的PFN号连续
 *  4. page结构体处于线性映射区
 * 
 * DISCONTIGMEM内存模型: 死翘翘了不谈
 * 
 * SPARSEMEM内存模型:
 *  1. 同一个section内，page结构体所处的虚拟内存连续
 *  2. 不同section间(即使是相邻section)，page结构体所处的虚拟内存不连续
 *  3. 同一个section，或连续section间，page结构体对应的PFN连续
 *  4. PFN对应的物理内存连续(但该PFN对应的虚拟内存可能未分配, page结构体也未分配，自然产生了不连续的物理内存)
 * 
 * SPARSEMEM_VMEMMAP内存模型:
 *  1. 所有section管理的page结构体都处于一个连续的虚拟内存区域(即加强了SPARSEMEM内存模型的第二点)
 * 
 *  2. 相比起FLAT内存模型，只是多了section的管理结构，使得部分struct page可以动态分配(FLAT内存模型中，由于是数组管理内存，必须全部分配)
 * 
 *  3. 尚未被动态分配的虚拟内存(线性映射的物理内存-用于存放page结构体，不是page结构体指向的物理页)可由其他进程暂时使用，等到设备热插的时候，再将数据迁移到其他页上，让出线性映射的物理内存，用于存放page结构体
 */
#if defined(CONFIG_FLATMEM) /* 平坦内存模型 */

#ifndef ARCH_PFN_OFFSET
#define ARCH_PFN_OFFSET		(0UL)
#endif

#define __pfn_to_page(pfn)	(mem_map + ((pfn) - ARCH_PFN_OFFSET))
#define __page_to_pfn(page)	((unsigned long)((page) - mem_map) + \
				 ARCH_PFN_OFFSET)

#elif defined(CONFIG_SPARSEMEM_VMEMMAP)

/* memmap is virtually contiguous.  */
#define __pfn_to_page(pfn)	(vmemmap + (pfn))
#define __page_to_pfn(page)	(unsigned long)((page) - vmemmap)

#elif defined(CONFIG_SPARSEMEM) /* SPARSEMEM 稀疏内存模型 */
/*
 * Note: section's mem_map is encoded to reflect its start_pfn.
 * section[i].section_mem_map == mem_map's address - start_pfn;
 */
#define __page_to_pfn(pg)					\
({	const struct page *__pg = (pg);				\
	int __sec = page_to_section(__pg);			\
	(unsigned long)(__pg - __section_mem_map_addr(__nr_to_section(__sec)));	\
})

#define __pfn_to_page(pfn)				\
({	unsigned long __pfn = (pfn);			\
	struct mem_section *__sec = __pfn_to_section(__pfn);	\
	__section_mem_map_addr(__sec) + __pfn;		\
})
#endif /* CONFIG_FLATMEM/SPARSEMEM */

/**
 * Convert a physical address to a Page Frame Number and back
 * PFN与物理地址的转换仅仅是PAGE_SHIFT的左移或右移
 */
#define	__phys_to_pfn(paddr)	PHYS_PFN(paddr)
#define	__pfn_to_phys(pfn)	PFN_PHYS(pfn)

#define page_to_pfn __page_to_pfn
#define pfn_to_page __pfn_to_page

#endif /* __ASSEMBLY__ */

#endif
