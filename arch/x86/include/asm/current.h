/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_CURRENT_H
#define _ASM_X86_CURRENT_H

#include <linux/compiler.h>
#include <asm/percpu.h>

#ifndef __ASSEMBLY__
struct task_struct;

DECLARE_PER_CPU(struct task_struct *, current_task);

static __always_inline struct task_struct *get_current(void)
{
	return this_cpu_read_stable(current_task);

	/* this_cpu_read_stable(current_task)展开如下
		({
		// 定义返回值变量，类型为 current_task 的类型
		typeof(current_task) pscr_ret__;

		// 验证变量类型的指针合法性，防止类型错误(确保current_task是一个变量)
		do {
			// &(current_task)能够取地址确保了current_task是一个变量，而不是常量
			const void *__vpp_verify = (typeof((&(current_task)) + 0))NULL;

			// 避免了未使用变量的编译警告
			(void)__vpp_verify;
		} while (0);

		// 根据 current_task 的大小进行不同的处理
		switch (sizeof(current_task)) {
			case 1: 
				// 处理 1 字节的情况
				pscr_ret__ = ({
					u8 pfo_val__;

					// P修饰符会将[var]的地址格式化为适合段寄存器引用的形式
					asm("movb %%gs:%P[var], %[val]" 
						: [val] "=q" (pfo_val__) 
						: [var] "p" (&(current_task)));
					(typeof(current_task))(unsigned long) pfo_val__;
				});
				break;

			case 2: 
				// 处理 2 字节的情况
				pscr_ret__ = ({
					u16 pfo_val__;
					asm("movw %%gs:%P[var], %[val]" 
						: [val] "=r" (pfo_val__) 
						: [var] "p" (&(current_task)));
					(typeof(current_task))(unsigned long) pfo_val__;
				});
				break;

			case 4: 
				// 处理 4 字节的情况
				pscr_ret__ = ({
					u32 pfo_val__;
					asm("movl %%gs:%P[var], %[val]" 
						: [val] "=r" (pfo_val__) 
						: [var] "p" (&(current_task)));
					(typeof(current_task))(unsigned long) pfo_val__;
				});
				break;

			case 8: 
				// 处理 8 字节的情况
				pscr_ret__ = ({
					u64 pfo_val__;
					asm("movq %%gs:%P[var], %[val]" 
						: [val] "=r" (pfo_val__) 
						: [var] "p" (&(current_task)));
					(typeof(current_task))(unsigned long) pfo_val__;
				});
				break;

			default: 
				// 处理不支持的大小
				__bad_size_call_parameter();
				break;
		}
		pscr_ret__;
	})
*/
}

// 获取当前进程task_struct
#define current get_current()

#endif /* __ASSEMBLY__ */

#endif /* _ASM_X86_CURRENT_H */
