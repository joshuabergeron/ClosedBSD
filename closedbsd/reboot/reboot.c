/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/reboot.h>

int main(void)
{
	reboot(RB_DUMP);
}

