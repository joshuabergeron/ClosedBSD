/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"
#include "mon.h"

static int 
_menu1_sa_action(dialogMenuItem * self)
{
	int ret;

	dialog_clear();
	dialog_msgbox("ClosedBSD", "Please insert a floppy diskette and press a key.", -1, -1, 1);

	dialog_clear();
	if ((ret = system("/sbin/mount /dev/fd0 /mnt")) != 0) {
		dialog_clear();
		dialog_msgbox("ClosedBSD", "This floppy is not formatted.  Press a key to format. (This may take a few minutes)", -1, -1, 1);

		dialog_clear();
		dialog_gauge("ClosedBSD", "Formatting /dev/fd0", 10, 1, 7, 70, 10);
		if ((ret = system("/sbin/fdformat -y /dev/fd0 >/dev/null")) != 0) {
			dialog_clear();
			dialog_msgbox("ClosedBSD", "Some error occured during 'fdformat'.  Contact bugs@closedbsd.org.", -1, -1, 1);
			return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
		}
		dialog_gauge("ClosedBSD", "Formatting /dev/fd0", 10, 1, 7, 70, 30);
		if ((ret = system("/sbin/disklabel -w -r /dev/fd0 fd1440 >/dev/null")) != 0) {
			dialog_clear();
			dialog_msgbox("ClosedBSD", "Some error occured during 'disklabel'.  Contact bugs@closedbsd.org.", -1, -1, 1);
			return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
		}
		dialog_gauge("ClosedBSD", "Formatting /dev/fd0", 10, 1, 7, 70, 60);
		if ((ret = system("/sbin/newfs -T fd1440 /dev/fd0 fd1440 >/dev/null")) != 0) {
			dialog_clear();
			dialog_msgbox("ClosedBSD", "Some error occured during 'newfs'.  Contact bugs@closedbsd.org.", -1, -1, 1);
			return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
		}
		dialog_gauge("ClosedBSD", "Formatting /dev/fd0", 10, 1, 7, 70, 100);
		if ((ret = system("/sbin/mount /dev/fd0 /mnt >/dev/null")) != 0) {
			dialog_clear();
			dialog_msgbox("ClosedBSD", "Unable to mount floppy.  Contact bugs@closedbsd.org.", -1, -1, 1);
			return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
		}
		system("/sbin/writetcnp");
	} else
		system("/sbin/umount /dev/fd0");
	system("/sbin/writetcnp");

	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

static int 
_menu1_mo_action(dialogMenuItem * self)
{
	dialog_clear();
	mon();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

static int 
_menu1_sh_action(dialogMenuItem * self)
{
	dialog_clear();
	end_dialog();
	exit(0);
}

static int 
_menu1_in_action(dialogMenuItem * self)
{
	dialog_clear();
	dev_gauge();
	dialog_clear();
	dev_list();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

static int 
_menu1_fw_action(dialogMenuItem * self)
{
	dialog_clear();
	init_dialog();
	main_fw_menu();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

static int 
_menu1_su_action(dialogMenuItem * self)
{
	summary();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

static int 
_menu1_re_action(dialogMenuItem * self)
{
	dialog_clear();
	end_dialog();
	endwin();
	system("/sbin/reboot");
	exit(0);
}

static int 
_menu1_na_action(dialogMenuItem * self)
{
	nat_window();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

dialogMenuItem menu1[] = {
	{"Interfaces", "configure network interfaces", NULL, _menu1_in_action},
	{"Firewall", "configure firewall rules", NULL, _menu1_fw_action},
	{"NAT", "configure network address translation", NULL, _menu1_na_action},
	{"Monitor", "monitor connections and protocol statistics ", NULL, _menu1_mo_action},
	{"Save", "save current settings", NULL, _menu1_sa_action},
	{"Summary", "summary of current configuration", NULL, _menu1_su_action},
	{"Exit", "abort configuration and exit", NULL, _menu1_sh_action},
	{"Reboot", "reboot the system", NULL, _menu1_re_action},
};

int 
main_menu()
{
	int retval;

	retval = dialog_menu("ClosedBSD Configuration Menu", "Please select the appropriate option:", -1, -1, 8, -8, menu1, NULL, NULL, NULL);

	dialog_clear();

	return 0;
}
