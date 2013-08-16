#!/bin/sh

sysctl -w 'net.inet.ip.fw.enable=1' 2>&1 >/dev/null
sysctl -w 'net.inet.ip.forwarding=1' 2>&1 >/dev/null
