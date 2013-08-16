/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"
#include "db.h"

int 
ifs_cfg(char *title, char *prompt, int height, int width, int y, int x, char *dev)
{
	WINDOW *w;
	int i, n, k, j;
	char titlez[80];
	char devz[80];
	int foo, done, key, set;
	struct device d;
	int ret = -1;
	int dev_id;

	init_dialog();
	dialog_clear();

	draw_shadow(stdscr, y, x, height, width);
	w = newwin(height, width, y, x);

	draw_box(w, 0, 0, height, width, dialog_attr, border_attr);
	wattrset(w, border_attr);
	keypad(w, TRUE);
	/* draw pretty lines outside. */
	/* box(w, ACS_VLINE, ACS_HLINE); */

	wrefresh(w);
	snprintf(titlez, sizeof(titlez), " [ %s ] ", title);

	foo = ((width / 2) - strlen(title));
	wmove(w, 0, foo);
	wattrset(w, title_attr);
	waddstr(w, titlez);
	wattrset(w, border_attr);
	for (i = 1; i < (height - 1); i++) {
		for (n = 1; n < (width - 1); n++) {
			wmove(w, i, n);
			waddch(w, ' ');
		}
	}

	draw_box(w, 2, 3, 12, 44, dialog_attr, border_attr);
	wattrset(w, title_attr);
	wmove(w, 4, 4);
	waddstr(w, "        IP ->");
	draw_box(w, 3, 18, 3, 25, dialog_attr, border_attr);

	wattrset(w, title_attr);
	wmove(w, 7, 4);
	waddstr(w, "   Netmask ->");
	draw_box(w, 6, 18, 3, 25, dialog_attr, border_attr);

	wattrset(w, title_attr);
	wmove(w, 10, 4);
	waddstr(w, "   Gateway ->");
	draw_box(w, 9, 18, 3, 25, dialog_attr, border_attr);

	print_button(w, "Cancel", height - 2, (width / 2) + 3, FALSE);
	print_button(w, "  OK  ", height - 2, (width / 2) - 11, FALSE);

	d.dhcp = DHCP_DISABLED;

	if ((ret = device_exist(dev)) >= 0) {
		wattrset(w, dialog_attr);
		wrefresh(w);
		/* fill in the previously configured values */
		snprintf(d.ip, sizeof(d.ip), "%s", dev_ip(dev));
		if (strlen(d.ip) > 6)
			mvwprintw(w, 4, 19, "%s", d.ip);
		else
			bzero(d.ip, sizeof(d.ip));

		snprintf(d.netmask, sizeof(d.netmask), "%s", dev_netmask(dev));
		if (strlen(d.netmask) > 6)
			mvwprintw(w, 7, 19, "%s", d.netmask);
		else
			bzero(d.netmask, sizeof(d.netmask));

		snprintf(d.gateway, sizeof(d.gateway), "%s", dev_gateway(dev));
		if (strlen(d.gateway) > 6)
			mvwprintw(w, 10, 19, "%s", d.gateway);
		else
			bzero(d.gateway, sizeof(d.gateway));

		snprintf(devz, sizeof(devz), "Editing device %s", dev);
		wmove(w, 1, (width / 2) - (strlen(devz) / 2));
		wattrset(w, dialog_attr);
		waddstr(w, devz);
		wrefresh(w);
	} else {
		bzero(d.ip, sizeof(d.ip));
		bzero(d.netmask, sizeof(d.netmask));
		bzero(d.gateway, sizeof(d.gateway));
		snprintf(devz, sizeof(devz), "Configuring device %s", dev);
		wmove(w, 1, (width / 2) - (strlen(devz) / 2));
		wattrset(w, dialog_attr);
		waddstr(w, devz);
	}


	/* start off in first window. */
	done = key = set = 0;
	j = 1;

	while (!done) {
		wrefresh(w);
		switch (j) {
		case 1:
			wattrset(w, inputbox_attr);
			wrefresh(w);
			print_button(w, "  OK  ", height - 2, (width / 2) - 11, FALSE);
			print_button(w, "Cancel", height - 2, (width / 2) + 3, FALSE);

			if (strlen(d.ip) < 1)
				bzero(d.ip, sizeof(d.ip));

			k = line_edit(w, 4, 19, 23, 23, inputbox_attr, 1, d.ip, DialogInputAttrs);
			if (k == KEY_DOWN) {
				j++;
				break;
			} else if (k == 9) {
				j++;
				break;
			} else if (k == KEY_UP) {
				j = 1;
				break;
			} else if (k == 10) {
				j++;
				break;
			}
			break;

		case 2:
			wattrset(w, inputbox_attr);
			wrefresh(w);
			/*
			 * print_button(w, "  OK
			 * ",height-2,(width/2)-11,FALSE);
			 */

			if (strlen(d.netmask) < 1)
				bzero(d.netmask, sizeof(d.netmask));

			k = line_edit(w, 7, 19, 23, 23, inputbox_attr, 1, d.netmask, DialogInputAttrs);
			if (k == KEY_DOWN) {
				j++;
				break;
			} else if (k == 10) {
				j++;
				break;
			} else if (k == 9) {
				j++;
				break;
			} else if (k == KEY_UP) {
				j = 1;
				break;
			}
			break;

		case 3:
			wattrset(w, inputbox_attr);
			wrefresh(w);
			print_button(w, "  OK  ", height - 2, (width / 2) - 11, FALSE);
			if (strlen(d.gateway) < 1)
				bzero(d.gateway, sizeof(d.gateway));

			k = line_edit(w, 10, 19, 23, 23, inputbox_attr, 1, d.gateway, DialogInputAttrs);
			if (k == KEY_DOWN) {
				j++;
				break;
			} else if (k == 10) {
				j++;
				break;
			} else if (k == 9) {
				j++;
				break;
			} else if (k == KEY_UP) {
				j--;
				break;
			}
			break;

		case 4:
			wattrset(w, title_attr);
			wrefresh(w);
			curs_set(0);
			print_button(w, "  OK  ", height - 2, (width / 2) - 11, TRUE);
			print_button(w, "Cancel", height - 2, (width / 2) + 3, FALSE);
			wrefresh(w);
			key = wgetch(w);
			if (key == KEY_UP) {
				j--;
				break;
			} else if (key == 9) {
				j++;
				break;
			} else if (key == KEY_DOWN) {
				j++;
				break;
			} else if (key == 10) {
				done = 1;
				set = 1;
				break;
			}
			break;

		case 5:
			wattrset(w, title_attr);
			wrefresh(w);
			curs_set(0);
			print_button(w, "  OK  ", height - 2, (width / 2) - 11, FALSE);
			print_button(w, "Cancel", height - 2, (width / 2) + 3, TRUE);
			wrefresh(w);
			key = wgetch(w);
			if (key == KEY_UP) {
				j = 4;
				break;
			} else if (key == KEY_DOWN) {
				j = 4;
				break;
			} else if (key == 9) {
				j = 1;
				break;
			} else if (key == 10) {
				dialog_clear();
				return -1;
				break;
			}
			break;
		}
	}

	if (strlen(d.gateway) > 1) {
		ret = dialog_yesno("ClosedBSD", "Would you like this device to be the default gateway?", -1, -1);
		if (ret == 0) {
			if ((ext_exist()) == -1) {
				d.ext = 1;
			} else {
				dialog_clear();
				ret = dialog_yesno("ClosedBSD", "You already have a default gateway set.  Overwrite?", -1, -1);
				if (ret == 0) {
					unset_ext(d.id);
				} else {
					d.ext = 0;
				}
			}
		} else {
			d.ext = 0;
		}
	} else
		d.ext = 0;

	strcpy(d.device, dev);

	if (set == 1) {
		dialog_clear();
		dev_id = device_exist(d.device);

		switch (dev_id) {
		case -1:
			d.id = next_device_id();
			insert_device(d);
			break;
		default:
			ret = dialog_yesno("ClosedBSD", "This device is already configured. Overwrite?", -1, -1);
			if (ret == 0) {
				d.id = dev_id;
				insert_device(d);
				break;
			} else {
				break;
			}
		}
	}
	dialog_clear();
	wrefresh(w);
	end_dialog();
	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}
