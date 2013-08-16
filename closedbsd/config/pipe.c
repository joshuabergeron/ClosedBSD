/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"
#include "fw.h"

struct ruleset l;

int 
proto_pipe()
{
	WINDOW *w;

	dialog_clear();

	w = newwin(22, 76, 1, 1);
	_std_window(w, " ClosedBSD: Load Balancing Manager ");

	ip_list(w, "IP Address : Port", 2, 43);

	sleep(2);

	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

int 
proto_pipe_nav(WINDOW * w, struct ruleset r, int selected)
{
	return 0;
}
