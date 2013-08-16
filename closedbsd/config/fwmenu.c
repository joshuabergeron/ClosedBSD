/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */
#include "main.h"
#include "fw.h"
#include "db.h"

static int type;

dialogMenuItem afm[] = {
	{"Insert", "insert a rule", NULL, _afi_menu},
	{"Delete", "delete a rule", NULL, _afm_delete},
	{"List", "list current rules", NULL, _afm_list},
	{"Flush", "flush all rules", NULL, _afm_flush},
	{"Save", "save all settings", NULL, _afm_save},
};

dialogMenuItem afi[] = {
	{"Filter", "insert a protocol filter rule", NULL, _afi_filter},
	{"Forward", "insert port forwarding rule", NULL, _afi_forward},
	{"Manual", "manually insert a rule (raw ipfw input)", NULL, _afi_manual},
};

int 
_afm_save(dialogMenuItem * self)
{
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
_afm_list(dialogMenuItem * self)
{
	dialog_clear();
	dialog_prgbox("ClosedBSD: Listing IPFW rules", "/sbin/ipfw list", 22, 70, TRUE, FALSE);
	dialog_clear();

	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
_afm_flush(dialogMenuItem * self)
{
	dialog_clear();
	system("/sbin/ipfw -q flush");
	unlink("/etc/rules");

	dialog_msgbox("ClosedBSD", "Ruleset has been cleared.", -1, -1, 1);
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
_afm_delete(dialogMenuItem * self)
{
	delete_rule();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
_afm_menu()
{
	dialog_clear();
	dialog_menu("ClosedBSD: Firewall Configuration", "Please select the appropriate option:", -1, -1, 5, -5, afm, NULL, NULL, NULL);

	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
_afi_menu(dialogMenuItem * self)
{
	dialog_clear();
	dialog_menu("ClosedBSD: Advanced Firewall Configuration", "Please select the appropriate option:", -1, -1, 3, -3, afi, NULL, NULL, NULL);

	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
_afi_manual(dialogMenuItem * self)
{
	char result[256];
	int ret;

	dialog_clear();
	bzero(result, sizeof(result));

	ret = dialog_inputbox("ClosedBSD: Manual IPFW rule addition", "Type in the rule manually.  These are some sample rules that illustrate the syntax\n\n 'add allow all from any to any'\n 'add deny tcp from echelon.gov'\n 'add deny all from evilcrackers.org'\n 'add allow all from 192.168.202.0 to any out'\n\n", -1, -1, result);

	if (ret == 0)
		raw_add_rule(result);

	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
_afi_filter(dialogMenuItem * self)
{
	proto_filter();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
_afi_forward(dialogMenuItem * self)
{
	proto_forward();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

static int 
check(dialogMenuItem * self)
{
	return ((int)self->data == type);
}

static int 
spend(dialogMenuItem * self)
{
	type = (int)self->data;
	return DITEM_SUCCESS | DITEM_REDRAW;
}

static void 
ask(dialogMenuItem * self, int is_selected)
{
	char *str;

	if (is_selected) {
		if (!strcmp(self->prompt, "Basic"))
			str = "The 'basic' firewall is the type of firewall you would use\
					if you were into network gaming, or used a wide variety\
					of applications, such as ICQ (tm), AOL Instant Messanger (tm)\
					, or play games such\
					as quake or halflife.  This firewall type passes IP, TCP,\
					UDP, and ICMP in and out seamlessly, with no restriction.\
					If you want functionality for a lot of applications, this\
					is the option for you.\
					You can add additional rules to the 'Basic' ruleset later on\
					By specifying 'Advanced' at the main firewall menu.";
		else
			str = "Advanced configuration menu.  Add all types of rules\
					 manually to optimize network security and performance.\
					 The advanced configuration menu features interfaces \
					 to add protocol filters, port forwards, and redirects \
					 in a very simple and easy to understand method.";

		DialogY = 13;
		DialogX = 3;
		dialog_msgbox("ClosedBSD", str, 10, 74, 0);
	}
}

/* prompt	title			checked		fire		sel	data */
static dialogMenuItem list[] = {
	{"Basic", "Basic configuration; pass all packets in and out.", check, spend, ask, (void *)1},
	{"Advanced", "Create / Manage a custom ruleset", check, spend, ask, (void *)2},
};

int 
main_fw_menu()
{
	int retval;
	struct ruleset r;
	int ret;
	char *tmp = dev_ext();
	char buf[256];
	FILE *stream = NULL;

	init_dialog();
	DialogY = 1;
	DialogX = 3;

	retval = dialog_menu("ClosedBSD", "Select Your Firewall Configuration",
	    10, 74, 2, -2, list, NULL, NULL, NULL);

	dialog_clear();
	init_dialog();

	/* retval must be 0 to get value of type. */
	if (retval == 0) {
		switch (type) {
		case 1:
			ret = ext_exist();
			if (ret < 0) {
				dialog_clear();
				dialog_msgbox("ClosedBSD", "You must first configure an external device", -1, -1, 1);
				return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
			}
			unlink(RULESCRIPT);
			system("/sbin/ipfw -q flush");

			enable_nat(tmp);

			bzero(&r, sizeof(r));
			r.cmd = ADD;
			r.action = ALLOW;
			r.proto = IP;
			snprintf(r.src, sizeof(r.src), "any");
			snprintf(r.dst, sizeof(r.dst), "any");
			__insert_rule(r);

			bzero(&r, sizeof(r));
			r.cmd = ADD;
			r.action = ALLOW;
			r.proto = TCP;
			snprintf(r.src, sizeof(r.src), "any");
			snprintf(r.dst, sizeof(r.dst), "any");
			__insert_rule(r);

			bzero(&r, sizeof(r));
			r.cmd = ADD;
			r.action = ALLOW;
			r.proto = UDP;
			snprintf(r.src, sizeof(r.src), "any");
			snprintf(r.dst, sizeof(r.dst), "any");
			__insert_rule(r);

			bzero(&r, sizeof(r));
			r.cmd = ADD;
			r.action = ALLOW;
			r.proto = ICMP;
			snprintf(r.src, sizeof(r.src), "any");
			snprintf(r.dst, sizeof(r.dst), "any");
			__insert_rule(r);

			dialog_clear();

			snprintf(buf, sizeof(buf), "/sbin/natd -n %s", tmp);
			system(buf);

			unlink("/etc/nat");
			stream = fopen("/etc/nat", "a+");
			if (!stream)
				return -1;

			fprintf(stream, "%s", tmp);
			fclose(stream);

			if (dialog_yesno("ClosedBSD: 'Basic' Firewall Type Enabled", "Save Configuration?", -1, -1) == 0) {
				system("/sbin/writetcnp");
			}
			break;

		case 2:
			_afm_menu();
			break;
		}
	}
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}
