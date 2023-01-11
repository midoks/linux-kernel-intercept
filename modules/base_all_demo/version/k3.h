#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>

static char writebuf[100];
static unsigned long * sys_call_table;

//定义
asmlinkage int (*original_open)(const char*, int, int);
asmlinkage int custom_open(const char* __user file_name, int flags, int mode);

/** 
 * @desc 自定义拦截文件打开时间
 * @param file_name 文件
 */
asmlinkage int custom_open(const char* __user file_name, int flags, int mode)
{
    copy_from_user(writebuf, file_name, 80);
    printk(KERN_INFO "hello %s\n", writebuf);
    printk(KERN_INFO "hello custom_open successfully %d- %d \n",flags, mode);

    //获取当前进程的id
    int pid = current->pid;

    //获取当前进程的父进程id
    int ppid = current->real_parent->real_parent->pid;

    //获取当前进程的根目录
    const char *ppwd = (current->fs->root).dentry->d_name.name;
    struct path pwd;

    //获取当前目录
    get_fs_pwd(current->fs,&pwd);
    printk(KERN_WARNING "hello op PID=%d,parent=%d attempts to open!\n",pid,ppid);
    printk(KERN_WARNING "hello op ROOT:%s!\n",ppwd);
    printk(KERN_WARNING "hello op PWD:%s!\n",pwd.dentry->d_name.name);
    return original_open(file_name, flags, mode);
}

int init_intercept(void){
	sys_call_table = NULL;
    sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");
    write_cr0 (read_cr0 () & (~ 0x10000));
    original_open = (void *)sys_call_table[__NR_open];
    sys_call_table[__NR_open] = custom_open;
    write_cr0 (read_cr0 () | 0x10000);

    printk(KERN_INFO "Hello Kernel Loaded Successfully.\n");
	return 0;
}


int exit_intercept(void){
	write_cr0 (read_cr0 () & (~ 0x10000));
	sys_call_table[__NR_open] = original_open;
    write_cr0 (read_cr0 () | 0x10000);

    printk(KERN_INFO "Bye Kernel.\n");
	return 0;
}