/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"

int 
main(int argc, char **argv)
{
	int rc;

	init_dialog();
	rc = dialog_textbox("BSD License", "/COPYRIGHT", 20, 76);
	dialog_clear();

	main_menu();
	end_dialog();

	exit(EXIT_SUCCESS);
}
