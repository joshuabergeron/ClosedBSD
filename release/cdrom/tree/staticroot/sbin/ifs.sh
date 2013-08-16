#!/bin/sh

for x in `ifconfig -l`
do
	echo $x >> /tmp/devices.tmp
done
