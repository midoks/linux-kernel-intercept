#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>


#include <linux/moduleparam.h>
#include <linux/kallsyms.h>

// MODULE_LICENSE("GPL");
MODULE_AUTHOR("midoks");
MODULE_DESCRIPTION("hello");
MODULE_VERSION("v1.0");

unsigned long (*n_kallsyms_lookup_name)(char *) = 0xffffffffb9b1ff50;
// module_param(n_kallsyms_lookup_name, charp, 0);
// module_param(sys_call_table_address, charp, 0);

// mkdir
// asmlinkage long *original_mkdir = NULL;
//拦截后,直接奔溃
asmlinkage long (*original_mkdir)(const char __user *pathname, umode_t mode);
// asmlinkage long custom_mkdir(const char __user *pathname, umode_t mode);

//mkdir的函数原型,这个函数的原型要和系统的一致
asmlinkage long custom_mkdir(const char __user *pathname, umode_t mode)
{
    printk("mkdir pathname: %s\n", pathname);
    printk(KERN_INFO "mkdir do nothing!\n");
    return 0; /*everything is ok, but he new systemcall does nothing*/
}

int __init hello_module_init(void)
{
    printk(KERN_INFO "n_kallsyms_lookup_name:%p\n", n_kallsyms_lookup_name);
    printk(KERN_INFO "Hello Kernel -- midoks .\n");
    return 0;//A non 0 return means init_module failed; module can't be loaded.
}
 
void __exit hello_module_exit(void)
{
    printk(KERN_INFO "Bye Kernel -- midoks .\n");
}
 
module_init(hello_module_init);
module_exit(hello_module_exit);