/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"
#include "fw.h"
#include "db.h"

/* Hook functions */

static int nat;

static int 
check(dialogMenuItem * self)
{
	return ((int)self->data == nat);
}

static int 
spend(dialogMenuItem * self)
{
	nat = (int)self->data;
	return DITEM_SUCCESS | DITEM_REDRAW;
}

static void 
ask(dialogMenuItem * self, int is_selected)
{
	char str[512];


	if (is_selected) {
		if (!strcmp(self->prompt, "Enable"))
			snprintf(str, sizeof(str), "This option will enable Network Address Translation for your internal network interface(s) and route packets through your external interface.  Network Address Translation will activate once you enable this, and additionally every time you reboot (if you save your settings).  You must have an external device defined to use this option.");

		else
			snprintf(str, sizeof(str), "This option will stop Network Address Translation if it is currently running, and will disable it in future reboots. (if you save your settings)");
	} else
		snprintf(str, sizeof(str), "This option will enable Network Address Translation for your internal network interface(s) and route packets through your external interface.  Network address translation will activate once you enable this, and additionally every time you reboot (if you save your settings).  You must have an external device defined to use this option.  ");

	DialogX = 3;
	DialogY = 13;

	dialog_msgbox("ClosedBSD", str, 10, 74, 0);
}

static dialogMenuItem menu5[] = {
	{"Enable", "Enable network address translation", check, spend, ask, (void *)1},
	{"Disable", "Disable network address translation", check, spend, ask, (void *)2},
};

int 
disable_nat(void)
{
	char buf[256];

	unlink(NATSCRIPT);
	snprintf(buf, sizeof(buf), "/sbin/killall -9 natd");

	/* if NATSCRIPT does not exist, it won't process the divert. */
	ruleset();

	/* now remove it from the file */
	return 0;
}

int 
ruleset(void)
{
	FILE *stream = NULL;
	char buf[256];
	char nat[256];

	system("/sbin/ipfw -q flush");

	/* first add divert */
	stream = fopen(NATSCRIPT, "r");
	if (stream) {
		while (fgets(buf, sizeof(buf), stream) != NULL)
			snprintf(nat, sizeof(nat), "/sbin/ipfw -q add 85 divert 8668 ip from any to any via %s", buf);

		fclose(stream);
		system(nat);
	}
	/* then the ruleset */
	stream = fopen("/etc/rules", "r");
	if (stream == NULL)
		return -1;

	while (fgets(buf, sizeof(buf), stream) != NULL) {
		system(buf);
		bzero(buf, sizeof(buf));
	}

	return 0;
}

int 
enable_nat(char *device)
{
	FILE *stream = NULL;
	char buf[256];

	dialog_clear();

	unlink(NATSCRIPT);
	stream = fopen(NATSCRIPT, "a+");
	if (stream == NULL)
		return -1;

	fprintf(stream, "%s\n", device);
	fclose(stream);

	snprintf(buf, sizeof(buf), "/sbin/natd -n %s\n", device);
	system(buf);

	ruleset();
	return 0;
}

int 
nat_window(void)
{
	int retval;
	char *dev = NULL;

	dialog_clear();

	DialogX = 3;
	DialogY = 1;
	retval = dialog_menu("ClosedBSD: Network Address Translation", "Please select an option: ", 10, 74, 2, -2, menu5, NULL, NULL, NULL);

	if (retval == 0) {
		switch (nat) {
		case 1:
			if (ext_exist() >= 0) {
				dialog_clear();
				dev = dev_ext();
				enable_nat(dev);
				dialog_msgbox("ClosedBSD: Notification", "Network Address Translation is now enabled", -1, -1, -1);
			} else {
				dialog_clear();
				dialog_msgbox("ClosedBSD: Notification", "You don't have an external device defined!", -1, -1, -1);
				return -1;
			}
			break;

		case 2:
			disable_nat();
			dialog_clear();
			dialog_msgbox("ClosedBSD: Notification", "Network Address Translation is now disabled", -1, -1, -1);
			break;
		}
	}
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}
