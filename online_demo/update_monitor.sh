#!/bin/bash

pkg_name=online_update
process_num=1

dir=/home/privateCloud
bin=${dir}/bin/${pkg_name}
conf=${dir}/conf/${pkg_name}".conf"

num=`ps -ef |grep ${bin} | grep -v "grep" | wc -l`

if [ $num -lt ${process_num} ]; then
	killall -9 $pkg_name
	${bin} ${conf}
	ip=`/sbin/ifconfig eth0 | grep "inet addr" | awk '{print $2}' | sed 's/addr\://'`
    	date=`date +'%Y-%m-%d %H:%M:%S'`
    	echo "$date $bin restart in $ip ! Check it!" >> monitor.log
fi

