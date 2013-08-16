/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <db.h>

#define DEVICES "/etc/devices.db"

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

int ruleset(void);
int natset(void);
int config_devices(void);
