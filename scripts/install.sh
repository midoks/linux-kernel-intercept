#!bin/bash

mkdir -p /opt

if [ ! -d /opt/linux-kernel-intercept ];then
	cd /opt && git clone https://github.com/midoks/linux-kernel-intercept
else
	cd /opt/linux-kernel-intercept && git pull
fi


cd /opt/linux-kernel-intercept/modules/base && make