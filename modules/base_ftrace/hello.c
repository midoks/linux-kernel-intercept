#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/version.h>

#include "ftrace_helper.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("blue");
MODULE_DESCRIPTION("demo");
MODULE_VERSION("0.01");


#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
#define PTREGS_SYSCALL_STUBS 1
#endif

#ifdef PTREGS_SYSCALL_STUBS
static asmlinkage long (*orig_kill)(const struct pt_regs *);


asmlinkage int hook_kill(const struct pt_regs *regs)
{
    // void set_root(void);

    pid_t pid = regs->di;
    int sig = regs->si;

    printk(KERN_INFO "sig is %d...\npid is %llu",sig,pid);


    return orig_kill(regs);

}
#else
static asmlinkage long (*orig_kill)(pid_t pid, int sig);

static asmlinkage int hook_kill(pid_t pid, int sig)
{

    printk(KERN_INFO "sig is %d...\npid is %llu",sig,pid);

    return orig_kill(pid, sig);
}
#endif

static struct ftrace_hook hooks[] = {
    HOOK("sys_kill", hook_kill, &orig_kill),
};

static int __init rootkit_init(void)
{
    int err;
    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if(err)
        return err;

    printk(KERN_INFO "rootkit: Loaded >:-)\n");

    return 0;
}

static void __exit rootkit_exit(void)
{
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
    printk(KERN_INFO "rootkit: Unloaded :-(\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);