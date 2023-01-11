#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>



int init_intercept(void){
	return 0;
}


int exit_intercept(void){
	return 0
}