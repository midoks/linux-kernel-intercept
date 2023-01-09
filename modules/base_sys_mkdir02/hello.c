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


//module constructor/destructor
typedef unsigned long (*sys_call_ptr_t)(void);
sys_call_ptr_t *_sys_call_table = NULL;

typedef asmlinkage long (*old_mkdir_t)(const char __user *pathname, umode_t mode);
old_mkdir_t old_mkdir = NULL;

// hooked mkdir function
asmlinkage long hooked_mkdir(const char __user *pathname, umode_t mode) {

        printk("hooked sys_mkdir(), mkdir name: ");
        printk(pathname);
        old_mkdir(pathname, mode);
}

// memory protection shinanigans
unsigned int level;
pte_t *pte;

//obtain sys_call_table
static int get_sys_call_table(void){
    unsigned long tmp_sys_call_table = 0;
    int ans = 0;

    tmp_sys_call_table = kallsyms_lookup_name("sys_call_table");
    if(tmp_sys_call_table != 0)
    {
        ans = 1;
        _sys_call_table = tmp_sys_call_table;
        printk("[+] find sys_call_table: 0x%lx\n", tmp_sys_call_table);
    }
    return ans;
}
// initialize the module
static int hooked_init(void) {
    printk("+ Loading hook_mkdir module\n");
    if(!get_sys_call_table()){
        return 0;
    }
    // now we can hook syscalls ...such as uname
    // first, save the old gate (fptr)
    old_mkdir = (old_mkdir_t) _sys_call_table[__NR_mkdir];

    // unprotect sys_call_table memory page
    pte = lookup_address((unsigned long) _sys_call_table, &level);

    // change PTE to allow writing
    set_pte_atomic(pte, pte_mkwrite(*pte));

    printk("+ unprotected kernel memory page containing sys_call_table\n");

    // now overwrite the __NR_uname entry with address to our uname
    _sys_call_table[__NR_mkdir] = (sys_call_ptr_t) hooked_mkdir;

    printk("+ sys_mkdir hooked!\n");

    return 0;
}

static void hooked_exit(void) {
    if(old_mkdir != NULL) {
        // restore sys_call_table to original state
        _sys_call_table[__NR_mkdir] = (sys_call_ptr_t) old_mkdir;

        // reprotect page
        set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    }

    printk("+ Unloading hook_mkdir module\n");
}

/*entry/exit macros*/
module_init(hooked_init);
module_exit(hooked_exit);
 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("midoks");
MODULE_DESCRIPTION("hook mkdir");
