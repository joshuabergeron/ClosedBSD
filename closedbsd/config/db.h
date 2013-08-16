/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

/* XXX: nested includes very bad! */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <db.h>

#define DEVICES 	"/etc/devices.db"

enum {
	DHCP_DISABLED, DHCP_ENABLED
};

struct device {
	int id;
	char device[6];
	char ip[16];
	char netmask[16];
	char gateway[16];
	int ext;
	int dhcp;
};

int insert_device(struct device d);
int device_exist(char *device);	/* returns d.id if exists */
int next_device_id();
int ext_exist();
int unset_ext(int device_id);
char *dev_ext();
char *dtoi(char *device);
char *dev_ip(char *device);
char *dev_netmask(char *device);
char *dev_gateway(char *device);
