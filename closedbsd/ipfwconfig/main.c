/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */
#include "main.h"
#include "fw.h"

dialogMenuItem afm[] = {
	{ "Insert", "insert a rule", NULL, _afi_menu },
	{ "Delete", "delete a rule", NULL, _afm_delete },
	{ "List", "list current rules", NULL, _afm_list },
	{ "Flush", "flush all rules", NULL, _afm_flush },
	{ "Save", "save all settings", NULL, _afm_save }, 
};

dialogMenuItem afi[] = {
	{ "Filter", "insert a protocol filter rule", NULL, _afi_filter },
	{ "Forward", "insert port forwarding rule", NULL, _afi_forward },
	{ "Manual", "manually insert a rule (raw ipfw input)", NULL, _afi_manual },
};

int _afm_save(dialogMenuItem *self)
{
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int _afm_list(dialogMenuItem *self)
{
	dialog_clear();
	dialog_prgbox("ClosedBSD: Listing IPFW rules", "/sbin/ipfw list", 22, 70, TRUE, FALSE);
	dialog_clear();
	
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int _afm_flush(dialogMenuItem *self)
{
	dialog_clear();
	system("/sbin/ipfw -q flush");
	unlink("/etc/rules");

   dialog_msgbox("ClosedBSD", "Ruleset has been cleared.", -1, -1, 1); 
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int _afm_delete(dialogMenuItem *self)
{
	delete_rule();	
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int _afm_menu()
{
	dialog_clear();
	dialog_menu("ClosedBSD: Firewall Configuration", "Please select the appropriate option:", -1, -1, 5, -5, afm, NULL, NULL, NULL);
	
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE; 
}

int _afi_menu(dialogMenuItem *self)
{
	dialog_clear();
	dialog_menu("ClosedBSD: Advanced Firewall Configuration", "Please select the appropriate option:", -1, -1, 3, -3, afi, NULL, NULL, NULL);
	
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
} 

int _afi_manual(dialogMenuItem *self)
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

int _afi_filter(dialogMenuItem *self)
{
	proto_filter();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int _afi_forward(dialogMenuItem *self)
{
	proto_forward();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int main(int argc, char *argv[])
{
	
	init_dialog(); 
	_afm_menu();

	end_dialog();

	return 0;		
}
