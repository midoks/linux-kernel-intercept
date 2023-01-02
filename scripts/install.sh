#!bin/bash

mkdir -p /opt

if [ ! -d /opt/linux-kernel-intercept ];then
	cd /opt && git clone https://github.com/midoks/linux-kernel-intercept
else
	cd /opt/linux-kernel-intercept && git pull
fi

if grep -Eqi "CentOS" /etc/issue || grep -Eq "CentOS" /etc/*-release; then
	OSNAME='rhel'
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
	apt-get install -y build-essential 
	apt-get install -y linux-headers-$(uname -r)

elif grep -Eqi "Ubuntu" /etc/issue || grep -Eq "Ubuntu" /etc/*-release; then
	OSNAME='ubuntu'
else
	OSNAME='unknow'
fi

echo "use system version: ${OSNAME}"

# cd /opt/linux-kernel-intercept/modules/base && make

# if [ "$?" == "0" ];then
# 	cd /opt/linux-kernel-intercept/modules/base && rmmod hello
# 	cd /opt/linux-kernel-intercept/modules/base && insmod hello.ko
# 	dmesg | tail -5 
# fi

cd /opt/linux-kernel-intercept/modules/base_sys_call && make

if [ "$?" == "0" ];then
	cd /opt/linux-kernel-intercept/modules/base_sys_call && rmmod hello
	cd /opt/linux-kernel-intercept/modules/base_sys_call && insmod hello.ko
	dmesg | tail -5 
fi

