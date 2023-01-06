#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>
#include <asm/unistd.h>    /* __NR_open */


#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <asm/current.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/file.h>
#include <asm/unistd.h>
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/path.h>


static unsigned long * sys_call_table;

asmlinkage long (*original_mkdir)(const char*, int);
asmlinkage long custom_mkdir(const char __user *pathname, umode_t mode);

/* make the page writable */
int make_rw(unsigned long address)
{
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);//查找虚拟地址所在的页表地址
    //设置页表读写属性
    pte->pte |=  _PAGE_RW;
    return 0;
}
/* make the page write protected */
int make_ro(unsigned long address)
{
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    pte->pte &= ~_PAGE_RW; //设置只读属性
    return 0;
}

int __init hello_module_init(void)
{
    sys_call_table = NULL;
    sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

    printk(KERN_ALERT "sys_call_table: 0x%p\n", sys_call_table);
    original_mkdir = (unsigned long*)sys_call_table[__NR_mkdir]; //获取原来的系统调用地址
    printk(KERN_ALERT "orig_mkdir: 0x%p\n", original_mkdir);

    make_rw((unsigned long)sys_call_table); //修改页属性
    sys_call_table[__NR_mkdir] = (unsigned long *)custom_mkdir;
    make_ro((unsigned long)sys_call_table);

    printk(KERN_INFO "Hello Kernel loaded successfully -- midoks.\n");
    return 0;//A non 0 return means init_module failed; module can't be loaded.
}
 
void __exit hello_module_exit(void)
{
    make_rw((unsigned long)sys_call_table);
    sys_call_table[__NR_mkdir] = (unsigned long *)original_mkdir;
    /*set mkdir syscall to the origal one*/
    make_ro((unsigned long)sys_call_table);

    printk(KERN_INFO "Bye Kernel -- midoks.\n");
}
 
//mkdir的函数原型,这个函数的原型要和系统的一致
asmlinkage long custom_mkdir(const char __user *pathname, umode_t mode)
{
    printk("mkdir pathname: %s\n", pathname);
    printk(KERN_INFO "mkdir do nothing!\n");
    return 0; /*everything is ok, but he new systemcall does nothing*/
}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("midoks");
MODULE_DESCRIPTION("hello sys mkdir");
MODULE_VERSION("v1.0");

module_init(hello_module_init);
module_exit(hello_module_exit);