#!bin/bash

if grep -Eqi "CentOS" /etc/issue || grep -Eq "CentOS" /etc/*-release; then
	OSNAME='rhel'
	yum install -y unzip

	yum install -y kernel-devel-$(uname -r)

	# yum search kernel-devel
	# yum install -y kernel-devel-matched.x86_64

	yum install -y rpm-build redhat-rpm-config asciidoc hmaccalc perl-ExtUtils-Embed pesign xmlto
	yum install -y audit-libs-devel binutils-devel elfutils-devel elfutils-libelf-devel java-devel
	yum install -y ncurses-devel newt-devel numactl-devel pciutils-devel python-devel zlib-devel

	yum install -y ncurses-devel
	yum install -y elfutils-libelf-devel
	
elif grep -Eqi "Fedora" /etc/issue || grep -Eq "Fedora" /etc/*-release; then
	OSNAME='fedora'
elif grep -Eqi "Rocky" /etc/issue || grep -Eq "Rocky" /etc/*-release; then
	OSNAME='rhel'
elif grep -Eqi "AlmaLinux" /etc/issue || grep -Eq "AlmaLinux" /etc/*-release; then
	OSNAME='rhel'
elif grep -Eqi "Amazon Linux" /etc/issue || grep -Eq "Amazon Linux" /etc/*-release; then
	OSNAME='amazon'
elif grep -Eqi "Debian" /etc/issue || grep -Eq "Debian" /etc/*-release; then
	OSNAME='debian'
	apt update -y
	apt install -y unzip

	apt-get install -y build-essential 
	apt-get install -y linux-headers-$(uname -r)

elif grep -Eqi "Ubuntu" /etc/issue || grep -Eq "Ubuntu" /etc/*-release; then
	OSNAME='ubuntu'
	apt update -y
	apt install -y unzip

	apt-get install -y build-essential 
	apt-get install -y linux-headers-$(uname -r)
else
	OSNAME='unknow'
fi

echo "use system version: ${OSNAME}"

mkdir -p /opt

curl -sSLo /tmp/main.zip https://codeload.github.com/midoks/linux-kernel-intercept/zip/main
cd /tmp && unzip /tmp/main.zip

CP_CMD=/usr/bin/cp
if [ -f /bin/cp ];then
		CP_CMD=/bin/cp
fi
mkdir -p /opt/linux-kernel-intercept
$CP_CMD -rf /tmp/linux-kernel-intercept-main/* /opt/linux-kernel-intercept


rm -rf /tmp/main.zip
rm -rf /tmp/linux-kernel-intercept-main




# cd /opt/linux-kernel-intercept/modules/base && make

# if [ "$?" == "0" ];then
# 	cd /opt/linux-kernel-intercept/modules/base && rmmod hello
# 	cd /opt/linux-kernel-intercept/modules/base && insmod hello.ko
# 	dmesg | tail -5 
# fi

cd /opt/linux-kernel-intercept/modules/base_args_kallsyms && make clean && make

if [ "$?" == "0" ];then
	find_mod=`lsmod |grep hello`
	if [ "$find_mod" != "" ];then
		cd /opt/linux-kernel-intercept/modules/base_args_kallsyms && rmmod hello
	fi
	cd /opt/linux-kernel-intercept/modules/base_args_kallsyms && insmod hello.ko
	dmesg | tail -5 
fi

