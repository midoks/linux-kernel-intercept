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

### 参考
- https://zhuanlan.zhihu.com/p/549973896
- https://copyfuture.com/blogs-details/202211090546213912
- https://blog.51cto.com/u_15775105/5822083
- http://www.deansys.com/doc/ldd3/ch02s03.html

