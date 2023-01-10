#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>


#include <linux/moduleparam.h>
#include <linux/kallsyms.h>

// MODULE_LICENSE("GPL");
MODULE_AUTHOR("midoks");
MODULE_DESCRIPTION("hello");
MODULE_VERSION("v1.0");

static char* sys_call_table_address = "";
module_param(sys_call_table_address, charp, 0);

// typedef void (*sys_call_ptr_t)(void);
// sys_call_ptr_t sys_call_table_address2;
// module_param(sys_call_table_address2, sys_call_ptr_t, 0);

int __init hello_module_init(void)
{
    printk(KERN_INFO "sys_call_table_address:%s\n", sys_call_table_address);
    // printk(KERN_INFO "sys_call_table_address2:%p\n", sys_call_table_address2);
    printk(KERN_INFO "Hello Kernel -- midoks .\n");
    return 0;//A non 0 return means init_module failed; module can't be loaded.
}
 
void __exit hello_module_exit(void)
{
    printk(KERN_INFO "Bye Kernel -- midoks .\n");
}
 
module_init(hello_module_init);
module_exit(hello_module_exit);