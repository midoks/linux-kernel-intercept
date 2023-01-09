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


unsigned long sys_call_table = (unsigned long ) SYS_CALL_TABLE;
 
asmlinkage long (*real_mkdir)(const char __user *pathname,
        umode_t mode);
 
asmlinkage long fake_mkdir(const char __user *pathname, umode_t mode)
{
  printk("mkdir-%s\n", pathname);
  if(true)
    {
        //return (*real_mkdir)(pathname, mode);
        return 0;
    }

}
 
 
//初始化函数
static int lkm_init(void)
{
    /*打开内核内存写保护*/ 
  write_cr0(read_cr0() & (~0x10000));
    //保存系统原有的系统调用接口函数
  real_mkdir = (void *)sys_call_table[__NR_mkdir];
    //替换原有系统调用地址
  sys_call_table[__NR_mkdir] = fake_mkdir;
    /*关闭内核内存写保护*/
  write_cr0(read_cr0() | 0x10000);
  printk("hook:module loaded\n");
 
  return 0;
}
 
static void lkm_exit(void)
{
  write_cr0(read_cr0() & (~0x10000));  
  sys_call_table[__NR_mkdir] = real_mkdir;
  write_cr0(read_cr0() | 0x10000);
  printk("hook:module removed\n");
}
 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("midoks");
MODULE_DESCRIPTION("hook mkdir");

module_init(lkm_init);
module_exit(lkm_exit);
