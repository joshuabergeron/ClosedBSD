/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dialog.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>

#define MAX_ITEMS 50

int main_menu();
int dev_list();
int dev_config(dialogMenuItem *self);
int dev_gauge();
void print_button(WINDOW *w, unsigned char *label, int y, int x, int selected);
int main_fw_menu();
int dhcp_choice(char *device);
int dhcp_start(char *device);
int fwcfg(char *device);
int ifs_cfg(char *title, char *prompt, int height, int width, int y, int x, char *dev);
int summary(void);
int nat_window(void);
int ruleset(void);
