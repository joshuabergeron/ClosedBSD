/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"

int 
dev_gauge()
{
	int i;

	init_dialog();

	for (i = 0; i <= 100; i++) {
		dialog_gauge("ClosedBSD", "Now determining what devices you have...", 10, 10, 7, 60, i);
		usleep(500);
	}
	return 0;
}
