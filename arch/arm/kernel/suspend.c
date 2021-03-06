#include <linux/init.h>
#include <linux/slab.h>

#include <asm/cacheflush.h>
#include <asm/idmap.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/memory.h>
#include <asm/smp_plat.h>
#include <asm/suspend.h>
#include <asm/tlbflush.h>
#include <asm/cache.h>

extern int __cpu_suspend(unsigned long, int (*)(unsigned long), u32 cpuid);
extern void cpu_resume_mmu(void);

#ifdef CONFIG_MMU
/*
 * Hide the first two arguments to __cpu_suspend - these are an implementation
 * detail which platform code shouldn't have to know about.
 *
 * On SMP systems, the value of cpu_logical_map(smp_processor_id()) must be
 * the MPIDR of the physical CPU the suspending logical CPU will resume on.
 * Consequently, if doing a physical CPU migration, cpu_logical_map() must be
 * updated appropriately somewhere between cpu_pm_enter() and cpu_suspend().
 */
int cpu_suspend(unsigned long arg, int (*fn)(unsigned long))
{
	struct mm_struct *mm = current->active_mm;
	u32 __mpidr = cpu_logical_map(smp_processor_id());
	int ret;

	if (!idmap_pgd)
		return -EINVAL;

	/*
	 * Provide a temporary page table with an identity mapping for
	 * the MMU-enable code, required for resuming.  On successful
	 * resume (indicated by a zero return code), we need to switch
	 * back to the correct page tables.
	 */
	ret = __cpu_suspend(arg, fn, __mpidr);
	if (ret == 0) {
		cpu_switch_mm(mm->pgd, mm);
		local_flush_tlb_all();
	}

	return ret;
}
#else
int cpu_suspend(unsigned long arg, int (*fn)(unsigned long))
{
	u32 __mpidr = cpu_logical_map(smp_processor_id());
	return __cpu_suspend(arg, fn, __mpidr);
}
#define	idmap_pgd	NULL
#endif

/*
 * This is called by __cpu_suspend() to save the state, and do whatever
 * flushing is required to ensure that when the CPU goes to sleep we have
 * the necessary data available when the caches are not searched.
 */
void __cpu_suspend_save(u32 *ptr, u32 ptrsz, u32 sp, u32 *save_ptr)
{
	*save_ptr = virt_to_phys(ptr);

	/* This must correspond to the LDM in cpu_resume() assembly */
	*ptr++ = virt_to_phys(idmap_pgd);
	*ptr++ = sp;
	*ptr++ = virt_to_phys(cpu_do_resume);

	cpu_do_suspend(ptr);

	flush_cache_all();
	outer_clean_range(*save_ptr, *save_ptr + ptrsz);
	outer_clean_range(virt_to_phys(save_ptr),
			  virt_to_phys(save_ptr) + sizeof(*save_ptr));
}

extern struct sleep_save_sp sleep_save_sp;

static int cpu_suspend_alloc_sp(void)
{
	void *ctx_ptr;
	/* ctx_ptr is an array of physical addresses */
	ctx_ptr = kcalloc(mpidr_hash_size(), L1_CACHE_BYTES, GFP_KERNEL);

	if (WARN_ON(!ctx_ptr))
		return -ENOMEM;
	sleep_save_sp.save_ptr_stash = ctx_ptr;
	sleep_save_sp.save_ptr_stash_phys = virt_to_phys(ctx_ptr);
	sync_cache_w(&sleep_save_sp);
	return 0;
}
early_initcall(cpu_suspend_alloc_sp);
