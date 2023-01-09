obj-m += hello.o
hello-y := 
EXTRA_CFLAGS= -O0
CONFIG_MODULE_SIG=n
CONFIG_MODULE_SIG_FORCE=n
CONFIG_MODULE_SIG_AL=n
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean