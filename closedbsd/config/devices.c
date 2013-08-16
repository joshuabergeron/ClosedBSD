/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"
#include "db.h"

dialogMenuItem menu2[MAX_ITEMS];

int 
dev_list()
{
	int i;
	FILE *stream = NULL;
	char buf[256];

	unlink("/tmp/devices.tmp");

	system("/sbin/ifs.sh");
	stream = fopen("/tmp/devices.tmp", "r");
	if (stream == NULL) {
		return -1;
	}
	i = 0;

	dialog_clear();

	while (fgets(buf, sizeof(buf), stream) != NULL) {
		buf[strlen(buf) - 1] = '\0';
		menu2[i].prompt = strdup(buf);

		if (strncmp(buf, "ppp", 3) == 0) {
			menu2[i].title = "PPP Device";
		} else if (strncmp(buf, "sl", 2) == 0) {
			menu2[i].title = "Slip Device";
		} else if (strncmp(buf, "gif", 3) == 0) {
			menu2[i].title = "IPv6 Device";
		} else if (strncmp(buf, "lo", 2) == 0) {
			menu2[i].title = "Loopback Device: [configured]";
		} else {
			if (device_exist(menu2[i].prompt) >= 0)
				menu2[i].title = "Ethernet Device: [configured]";
			else
				menu2[i].title = "Ethernet Device";
		}

		menu2[i].checked = NULL;
		menu2[i].fire = dev_config;
		i += 1;
		bzero(buf, sizeof(buf));
	}
	i += 1;

	fclose(stream);
	unlink("/tmp/devices.tmp");

	dialog_menu("ClosedBSD", "Please select the device to configure", -1, -1, (i - 1), -(i - 1), menu2, NULL, NULL, NULL);

	dialog_clear();
	return 0;
}

int 
write_dhcp_conf(char *hostname, char *device)
{
	FILE *stream = NULL;

	unlink("/etc/dhclient.conf");

	stream = fopen("/etc/dhclient.conf", "a+");
	if (stream)
		fprintf(stream, "interface \"%s\" {\n\tsend host-name \"%s\";\n}\n", device, hostname);

	fclose(stream);
	return 0;
}

int 
dev_config(dialogMenuItem * self)
{
	char *device = self->prompt;
	int ret = 1, host;
	struct device d;
	unsigned char dhcp_host[212];

	dialog_clear();

	ret = dialog_yesno("ClosedBSD", "Should this device use DHCP?", -1, -1);

	switch (ret) {
	case 0:
		d.id = next_device_id();
		strcpy(d.device, device);

		/* knock the rest out. its dhcp. */
		bzero(d.ip, sizeof(d.ip));
		bzero(d.netmask, sizeof(d.netmask));
		bzero(d.gateway, sizeof(d.gateway));

		dialog_clear();

		d.ext = 1;
		dialog_clear();
		d.dhcp = DHCP_ENABLED;

		bzero(dhcp_host, sizeof(dhcp_host));
		host = dialog_inputbox("ClosedBSD", "If your ISP requires you to send a hostname for DHCP requests, enter it here.    \n\nOtherwise, just hit OK.", 11, -1, dhcp_host);

		if (host == 0 && strlen(dhcp_host) > 1)
			write_dhcp_conf(dhcp_host, d.device);

		/* now insert the device */
		insert_device(d);
		dialog_clear();
		break;

	case 1:
		if (ifs_cfg("ClosedBSD", "foo", 16, 50, 1, 13, device) < 0) {
			dialog_clear();
			return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
		}
		break;
	}

	dialog_clear();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}
