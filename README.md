# linux-kernel-intercept

linux内核拦截事例

### 简单说明

- 下载到/opt目录下,进行测试。


### 一键安装测试

```
curl -fsSL  https://raw.githubusercontent.com/midoks/linux-kernel-intercept/main/scripts/install.sh | bash
```


### 常用命令

- 从 System.map 文件中读取(System.map 是一份内核符号表，包含了内核中的变量名和函数名地址，在每次编译内核时，自动生成。获取 sys_call_table 数组的虚拟地址使用如下命令)： 

```
sudo cat /boot/System.map-`uname -r` | grep sys_call_table
```

- demsg调试命令

```
sudo dmesg 						#列出所有 dmesg 消息
sudo dmesg | less 				#更少的消息分页
sudo dmesg -L 					#着色 dmesg 消息
sudo dmesg -H 					#dmesg 人性化格式
sudo dmesg -T					#dmesg 人类可读格式
sudo dmesg --follow				#dmesg 实时监控
sudo dmesg | head -10			#前 10 条 dmesg 消息
sudo dmesg | tail -10			#最近 10 条 dmesg 消息
sudo dmesg | grep -i "admin" 	#过滤特定字符串
sudo dmesg -l warn 				#过滤特定的消息级别
sudo dmesg -l warn,err 			#过滤多个消息级别

dmesg 消息级别
内核或 dmesg 消息设置有不同的级别来指定消息的重要性。
emerg： 系统无法使用。
alert： 必须立即采取行动。
crit： 临界条件。
err： 错误条件。
warn： 警告条件。
notice： 正常但重要的情况。
info： 信息。
debug： 调试级消息。

cat /var/log/dmesg.old 

#内核崩溃日志
/var/crash

```

### 查看加载模块

- ls /lib/modules/`uname -r`/extra

```
lsmod
lsmod | grep hello
```

### 系统函数头文件
```
include/linux/syscalls.h

例子,具体源码中去查找

asmlinkage long sys_getcwd(char __user *buf, unsigned long size);
asmlinkage long sys_mkdir(const char __user *pathname, umode_t mode);
asmlinkage long sys_chdir(const char __user *filename);
asmlinkage long sys_fchdir(unsigned int fd);
asmlinkage long sys_rmdir(const char __user *pathname);
asmlinkage long sys_lookup_dcookie(u64 cookie64, char __user *buf, size_t len);
```


### 参考
- https://zhuanlan.zhihu.com/p/549973896
- https://copyfuture.com/blogs-details/202211090546213912
- https://blog.51cto.com/u_15775105/5822083
- http://www.deansys.com/doc/ldd3/ch02s03.html
- https://github.com/torvalds/linux/search?q=__NR_open
- https://github.com/DaveGamble/cJSON
