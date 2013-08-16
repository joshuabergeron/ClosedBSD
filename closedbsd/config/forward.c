/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"
#include "fw.h"

int 
proto_forward()
{
	WINDOW *w;
	int p = 1;
	int key;
	int done = 0;
	struct ruleset r;
	char src[128];
	char dst[128];
	char forw[128];
	char src_port[5];
	char dst_port[5];
	char forw_port[5];
	int ret;

	r.action = FWD;
	r.proto = TCP;

	/* where is the methlab */
	bzero(r.src, sizeof(r.src));
	bzero(r.src_port, sizeof(r.src_port));
	bzero(r.dst, sizeof(r.dst));
	bzero(r.dst_port, sizeof(r.dst_port));
	bzero(src, sizeof(src));
	bzero(dst, sizeof(dst));
	bzero(forw, sizeof(forw));
	bzero(src_port, sizeof(src_port));
	bzero(dst_port, sizeof(dst_port));
	bzero(forw_port, sizeof(forw_port));

	/* now initialize the layout */
	dialog_clear();
	w = newwin(22, 76, 1, 1);
	_std_window(w, "ClosedBSD: Insert a TCP/UDP port forward");
	wrefresh(w);

	draw_protocol_forward_box(w, 2, 13, 1, r);
	draw_host_inputbox(w, 7, 19);
	wattrset(w, dialog_attr);
	mvwprintw(w, 8, 54, ":");
	draw_port_inputbox(w, 7, 55);
	draw_host_inputbox(w, 10, 19);
	wattrset(w, dialog_attr);
	mvwprintw(w, 11, 54, ":");
	draw_port_inputbox(w, 10, 55);
	draw_host_inputbox(w, 13, 19);
	wattrset(w, dialog_attr);
	mvwprintw(w, 14, 54, ":");
	draw_port_inputbox(w, 13, 55);
	buttons(w, 0);

	mvwprintw(w, 8, 7, "packet src");
	mvwprintw(w, 11, 7, "packet dst");
	mvwprintw(w, 14, 7, "forward to");
	wmove(w, 0, 0);
	help(w, 31, r);
	wrefresh(w);

	keypad(w, TRUE);
	while (!done) {
		switch (key = wgetch(w)) {
		case KEY_DOWN:
	down:
			switch (p) {
			case 1:
				p++;
				help(w, 32, r);
				draw_protocol_forward_box(w, 2, 13, p, r);
				break;

			case 2:
				p++;
				help(w, 8, r);
				draw_protocol_forward_box(w, 2, 13, 0, r);
				ret = forw_input(w, p, src, dst, forw, src_port, dst_port, forw_port);
				if (ret < p) {
					p = 1;
					goto down;
				} else {
					p = 3;
					goto down;
				}
				break;

			case 3:
				p++;
				help(w, 9, r);
				ret = forw_input(w, p, src, dst, forw, src_port, dst_port, forw_port);
				if (ret < p) {
					p = 2;
					goto down;
				} else {
					p = 4;
					goto down;
				}
				break;

			case 4:
				p++;
				help(w, 10, r);
				ret = forw_input(w, p, src, dst, forw, src_port, dst_port, forw_port);
				if (ret < p) {
					p = 3;
					goto down;
				} else {
					p = 5;
					goto down;
				}
				break;

			case 5:
				p++;
				help(w, 9, r);
				ret = forw_input(w, p, src, dst, forw, src_port, dst_port, forw_port);
				if (ret < p) {
					p = 4;
					goto down;
				} else {
					p = 6;
					goto down;
				}
				break;

			case 6:
				p++;
				help(w, 12, r);
				ret = forw_input(w, p, src, dst, forw, src_port, dst_port, forw_port);
				if (ret < p) {
					p = 5;
					goto down;
				} else {
					p = 7;
					goto down;
				}
				break;

			case 7:
				p++;
				buttons(w, 0);
				help(w, 9, r);
				ret = forw_input(w, p, src, dst, forw, src_port, dst_port, forw_port);
				if (ret < p) {
					p = 6;
					goto down;
				} else {
					p = 8;
					goto down;
				}
				break;

			case 8:
				p++;
				help(w, 33, r);
				buttons(w, 1);
				break;

			case 9:
				p++;
				help(w, 34, r);
				buttons(w, 2);
				break;
			}
			break;

		case KEY_RIGHT:
			switch (p) {
			case 9:
				p++;
				buttons(w, 2);
				break;
			}
			break;

		case KEY_LEFT:
			switch (p) {
			case 10:
				p--;
				buttons(w, 1);
				break;
			}
			break;

		case KEY_UP:
			switch (p) {
			case 1:
				p = 1;
				help(w, 31, r);
				draw_protocol_forward_box(w, 2, 13, p, r);
				break;
			case 2:
				p--;
				help(w, 31, r);
				draw_protocol_forward_box(w, 2, 13, p, r);
				break;

			case 9:
				p = 7;
				buttons(w, 0);
				goto down;

			case 10:
				p = 8;
				buttons(w, 1);
				goto down;
			}
			break;

		case 10:
			switch (p) {
			case 9:
				snprintf(r.src, sizeof(r.src), "%s", src);
				snprintf(r.src_port, sizeof(r.src_port), "%s", src_port);
				snprintf(r.dst, sizeof(r.dst), "%s", dst);
				snprintf(r.dst_port, sizeof(r.dst_port), "%s", dst_port);
				snprintf(r.other_a, sizeof(r.other_a), "%s", forw);
				snprintf(r.other_b, sizeof(r.other_b), "%s", forw_port);

				ret = __insert_rule(r);
				if (ret == 0) {
					dialog_msgbox("ClosedBSD: Notice", "Rule has been added", -1, -1, 1);
					return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
				} else {
					dialog_msgbox("ClosedBSD: Notice", "There was an error adding the rule. Verify proper information!", -1, -1, 1);
					break;
				}

			case 10:
				return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
			}

			break;

		case 32:
			switch (p) {
			case 1:
				r.proto = TCP;
				draw_protocol_forward_box(w, 2, 13, p, r);
				break;
			case 2:
				r.proto = UDP;
				draw_protocol_forward_box(w, 2, 13, p, r);
				break;
			}
			break;
		}
	}
	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}
