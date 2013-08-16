/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */
#include "main.h"
#include "fw.h"

/*
   the idea is to build our ruleset structure through the interface.
   once it is built, we call _insert_rule() to work with the
   supplied data; it will add the rule with ipfw, and also add it to
   our file
*/

int proto_filter()
{
	WINDOW *w;
	int p = 1, key;
	int done = 0;	
	unsigned char src[128];
	unsigned char dst[128];
	unsigned char src_port[5];
	unsigned char dst_port[5];
	struct ruleset rule;
	int ch;

	// a few defaults.
	rule.action = ALLOW;
	rule.proto  = TCP;
	rule.ifs 	= ANY;	
	rule.option	= NONE;

	bzero(src, sizeof(src));
	bzero(dst, sizeof(dst));
	bzero(src_port, sizeof(src_port));
	bzero(dst_port, sizeof(dst_port));
	
	bzero(rule.src_port, sizeof(rule.src_port));
	bzero(rule.dst_port, sizeof(rule.dst_port));
	bzero(rule.src, sizeof(rule.src));
	bzero(rule.dst, sizeof(rule.dst));

	initscr();
	init_dialog();
	dialog_clear();
	w = newwin(22, 76, 1, 1);
	_std_window(w, "ClosedBSD: Insert Protocol Filter");

	// initialize the radio buttons 
	draw_action_box(w, 2, 1, 1, rule);
	draw_protocol_box(w, 7, 1, 0, rule);
	draw_iface(w, 2, 51, 0, rule);
	draw_options(w, 7, 51, 0, rule);
	buttons(w, 0);
	
	// initialize input boxes
	draw_host_inputbox(w, 13, 16);
   wattrset(w, dialog_attr);
   mvwprintw(w, 14, 9, "Source:"); 
   draw_port_inputbox(w, 13, 51);

   draw_host_inputbox(w, 16, 16);
   wattrset(w, dialog_attr);
	mvwprintw(w, 17, 4, "Destination:"); 
   draw_port_inputbox(w, 16, 51);
	wmove(w, 0, 0);
	wrefresh(w);
	
	keypad(w, TRUE);

	while (!done) {
#ifdef DEBUG
		mvwprintw(w, 1, 1, "  ");
		mvwprintw(w, 1, 1, "%d", p);
		wmove(w, 0, 0);
		wrefresh(w); 
#endif
		help(w, p, rule);
		switch(key = wgetch(w)) {
		case KEY_DOWN:
			down:	
			//help(w, p, rule);
#ifdef DEBUG
			mvwprintw(w, 1, 1, "  ");
			mvwprintw(w, 1, 1, "%d", p);
      	wmove(w, 0, 0);
			wrefresh(w);
#endif
			switch(p) { 
			case 1:
				p++;
				draw_action_box(w, 2, 1, 2, rule); 
				break;
			case 2:
				p++;
				draw_action_box(w, 2, 1, 3, rule);
				break;	
			case 3:
				p++;
				draw_action_box(w, 2, 1, 0, rule);
				draw_protocol_box(w, 7, 1, 1, rule);
				break;
			case 4:
				p++;
				draw_protocol_box(w, 7, 1, 2, rule);
				break;
			case 5:
				p++;
				draw_protocol_box(w, 7, 1, 3, rule);
				break;
			case 6:
				p++;
				draw_protocol_box(w, 7, 1, 4, rule);
				break; 
			case 7:
				draw_protocol_box(w, 7, 1, 0, rule);
				p = 8;	
				help(w, p, rule);
				ch = inputs(w, p, src, dst, src_port, dst_port);
				if (ch < 8) {
					p = 6; 
					goto down;
				}  
			 	else if (ch > 8) {
					p = 8;
					goto down;
				}
				break;
			case 8:
				// srcport
				p = 9;
				help(w, p, rule);
				ch = inputs(w, p, src, dst, src_port, dst_port);
				if (ch < 9) {
					p = 7;
					goto down;
				}
				else if (ch > 8) {
					p = 9;
					goto down;
				} 
				break;
			case 9:
				// dst
				p = 10;
				help(w, p, rule);
				ch = inputs(w, p, src, dst, src_port, dst_port);
				if (ch < 10) {
					p = 8;
					goto down;
				}
				else if (ch > 10) {
					p = 10;
					goto down;
				}
				break;
			case 10:
				// dstport
				buttons(w, 0);
				p = 11;
				help(w, p, rule);
				ch = inputs(w, p, src, dst, src_port, dst_port);
				if (ch < 11) {
					p = 9;
					goto down;
				}
				else if (ch > 11) {
					p = 11;
					goto down;
				}
				break;
			case 11:
				p = 12;
				buttons(w, 1);
				break;
			case 12:
				p++;
				buttons(w, 2);	
				break;
			case 21:
				p++;
				help(w, 41, rule);
				draw_iface(w, 2, 51, 2, rule);
				break;
			case 22:
				p++;
				help(w, 42, rule);
				draw_iface(w, 2, 51, 3, rule);
				break; 
			case 23:
				p++;
				draw_iface(w, 2, 51, 0, rule);
				draw_options(w, 7, 51, 1, rule);
				break;
			case 24:
				p++;
				draw_options(w, 7, 51, 2, rule);
				break;
			case 25:
				p++;
				draw_options(w, 7, 51, 3, rule);
				break;
			case 26:
				p++;
				draw_options(w, 7, 51, 4, rule);
				break; 	
			case 27:
				p = 7;
				draw_options(w, 7, 51, 0, rule);
				goto down;	
			}	
			break;

		case KEY_LEFT:
			switch(p) {
			case 21:
				p = 1;
				draw_action_box(w, 2, 1, 1, rule);
				draw_iface(w, 2, 51, 0, rule);
				break;
			
			case 22:
				p = 2;
				draw_action_box(w, 2, 1, 2, rule);
				draw_iface(w, 2, 51, 0, rule);
				break;
			
			case 23:
				p = 3;
				draw_action_box(w, 2, 1, 3, rule);
				draw_iface(w, 2, 51, 0, rule);
				break;
			
			case 24:
				p = 4;
				draw_protocol_box(w, 7, 1, 1, rule);
				draw_options(w, 7, 51, 0, rule); 
				break;

			case 25:
				p = 5;
				draw_protocol_box(w, 7, 1, 2, rule);
				draw_options(w, 7, 51, 0, rule);
				break; 
			
			case 26:
				p = 6;
				draw_protocol_box(w, 7, 1, 3, rule);
				draw_options(w, 7, 51, 0, rule);
				break;
			
			case 27:
				p = 7;
				draw_protocol_box(w, 7, 1, 4, rule);
				draw_options(w, 7, 51, 0, rule);
				break;	
			}
			break;
	
		case KEY_RIGHT:
			switch(p) {
			case 1:	
				p = 21;
				draw_action_box(w, 2, 1, 0, rule);
				help(w, 40, rule);	
				draw_iface(w, 2, 51, 1, rule);
				break;
			
			case 2:
				p = 22;
				draw_action_box(w, 2, 1, 0, rule);
				help(w, 41, rule);
				draw_iface(w, 2, 51, 2, rule);
				break;

			case 3:
				p = 23;
				draw_action_box(w, 2, 1, 0, rule);
				help(w, 42, rule);
				draw_iface(w, 2, 51, 3, rule);
				break;
			
			case 4:
				p = 24;
				draw_protocol_box(w, 7, 1, 0, rule);
				draw_options(w, 7, 51, 1, rule);
				break;
			
			case 5:
				p = 25;
				draw_protocol_box(w, 7, 1, 0, rule);
				draw_options(w, 7, 51, 2, rule);
				break;
			
			case 6:
				p = 26;
				draw_protocol_box(w, 7, 1, 0, rule);
				draw_options(w, 7, 51, 3, rule);
				break;
			
			case 7:
				p = 27;
				draw_protocol_box(w, 7, 1, 0, rule);
				draw_options(w, 7, 51, 4, rule);
				break;
			}
			break;
		
		case KEY_UP:
			switch(p) {
			case 1:
				p = 1;
				draw_action_box(w, 2, 1, 1, rule);
				break;
			case 2:
				p--;
				draw_action_box(w, 2, 1, 1, rule);
				break;
			case 3:
				p--;
				draw_action_box(w, 2, 1, 2, rule);
				break;
			case 4:
				p--;
				draw_protocol_box(w, 7, 1, 0, rule);
				draw_action_box(w, 2, 1, 3, rule);
				break;	
			case 5:
				p--;
				draw_protocol_box(w, 7, 1, 1, rule);
				break;
			case 6:
				p--;
				draw_protocol_box(w, 7, 1, 2, rule);
				break;
			case 7:
				p--;
				draw_protocol_box(w, 7, 1, 3, rule);
				break; 
			case 8:
				buttons(w, 0);
				p--;
				draw_protocol_box(w, 7, 1, 4, rule);
				break;
			
			case 12:
				p = 10;
				goto down;
				break;

			case 13:
				p = 11;
				goto down;
				break;

			case 21:
				p = 7;
				draw_iface(w, 2, 51, 0, rule); 
				draw_protocol_box(w, 7, 1, 4, rule);
				break; 
			case 22:
				p--;
				help(w, 40, rule);
				draw_iface(w, 2, 51, 1, rule);
				break;
			case 23:
				p--;
				help(w, 41, rule);
				draw_iface(w, 2, 51, 2, rule);
				break;
			case 24:
				p--;
				draw_options(w, 7, 51, 0, rule);
				help(w, 42, rule);
				draw_iface(w, 2, 51, 3, rule);
				break;
			case 25:
				p--;
				draw_options(w, 7, 51, 1, rule);
				break;
			case 26:
				p--;
				draw_options(w, 7, 51, 2, rule);
				break;
			case 27:
				p--;
				draw_options(w, 7, 51, 3, rule); 
				break;
			}
			break;
		
		// space bar
		case 32:
			switch(p) {
			case 1:
				rule.action = ALLOW;
				draw_action_box(w, 2, 1, 1, rule);	
				break;
			case 2:
				rule.action = DENY;	
				draw_action_box(w, 2, 1, 2, rule); 	
				break;	
			case 3:
				rule.action = RESET;
				draw_action_box(w, 2, 1, 3, rule);
				break;
			case 4:
				rule.proto = TCP;
				rule.option = NONE;
				draw_protocol_box(w, 7, 1, 1, rule);
				draw_options(w, 7, 51, 0, rule);
				break;
			case 5:
				rule.proto = IP;
				rule.option = IP_NONE;
				draw_protocol_box(w, 7, 1, 2, rule);
				draw_options(w, 7, 51, 0, rule);
				break;
			case 6:
				rule.proto = UDP;
				draw_protocol_box(w, 7, 1, 3, rule);
				draw_options(w, 7, 51, 0, rule);
				break;
			case 7:
				rule.proto = ICMP;
				rule.option = ICMP_NONE;
				draw_protocol_box(w, 7, 1, 4, rule);
				draw_options(w, 7, 51, 0, rule);
				break; 
			case 21:
				rule.ifs = INCOMING;
				draw_iface(w, 2, 51, 1, rule);
				break;
			case 22:
				rule.ifs = OUTGOING;
				draw_iface(w, 2, 51, 2, rule);
				break;
			case 23:
				rule.ifs = ANY;
				draw_iface(w, 2, 51, 3, rule);
				break; 
			case 24:
				switch(rule.proto) {
				case TCP:
					rule.option = ESTABLISHED;
					draw_options(w, 7, 51, 1, rule);
					break;
			
				case IP:
					rule.option = SSRR;
					draw_options(w, 7, 51, 1, rule);
					break;
				
				case UDP:
					break;
				
				case ICMP:
					rule.option = ECHO_REQUEST;
					draw_options(w, 7, 51, 1, rule);
					break;
				}
				break;

			case 25:
				switch(rule.proto) {
				case TCP:
					rule.option = SETUP;
					draw_options(w, 7, 51, 2, rule);
					break;
				
				case IP:
					rule.option = LSRR;
					draw_options(w, 7, 51, 2, rule);
					break;
				
				case UDP:
					break;
				
				case ICMP:
					rule.option = DEST_UNREACH;
					draw_options(w, 7, 51, 2, rule);
					break;
				}
				break;
 
			case 26:
				switch(rule.proto) {
				case TCP:
					rule.option = FRAG;
					draw_options(w, 7, 51, 3, rule);
					break;
				
				case IP:
					rule.option = TS;
					draw_options(w, 7, 51, 3, rule);
					break;
				
				case UDP:
					break;
				
				case ICMP:
					rule.option = ECHO_REPLY;
					draw_options(w, 7, 51, 3, rule);
					break;
				}
				break;	
			
			case 27:
				switch(rule.proto) {
				case TCP:
					rule.option = NONE;
					draw_options(w, 7, 51, 4, rule);
					break; 
				
				case IP:
					rule.option = IP_NONE;
					draw_options(w, 7, 51, 4, rule);
					break;
				
				case UDP:
					break;

				case ICMP:
					rule.option = ICMP_NONE;
					draw_options(w, 7, 51, 4, rule);
					break;
				}
				break;
	
			}
			break;

		case 10:
			switch(p) {
			case 12:
#ifdef DEBUG
				mvwprintw(w, 1, 5, "     ");
				wmove(w, 0, 0);
				wrefresh(w);
#endif
				snprintf(rule.src, sizeof(rule.src), "%s", src);
				snprintf(rule.dst, sizeof(rule.dst), "%s", dst);

				if (strlen(rule.src) < 1)
					snprintf(rule.src, sizeof(rule.src), "any");
				
				if (strlen(rule.dst) < 1)
					snprintf(rule.dst, sizeof(rule.dst), "any");
	
				if (strncmp(src_port, "any", 3) == 0)
					bzero(rule.src_port, sizeof(rule.src_port));
				else 	
					snprintf(rule.src_port, sizeof(rule.src_port), "%s", src_port);
				
				if (strncmp(dst_port, "any", 3) == 0)
					bzero(rule.dst_port, sizeof(rule.dst_port));
				else
					snprintf(rule.dst_port, sizeof(rule.dst_port), "%s", dst_port);
 		
				__insert_rule(rule);
				dialog_clear();
				dialog_msgbox("ClosedBSD: Notice", "Rule has been added.", -1, -1, 1);
				return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;

			case 13:
#ifdef DEBUG
				mvwprintw(w, 1, 5, "     ");
				mvwprintw(w, 1, 5, "CANC ");
				wmove(w, 0, 0);
				wrefresh(w);
#endif
				bzero(&rule, sizeof(rule));

				return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
			}
			break;
				 
		// tab
		case 9:
			switch(p) {
			case 1:
				p = 21;
				draw_action_box(w, 2, 1, 0, rule);
				draw_iface(w, 2, 51, 1, rule);
				break;
			case 2:
				p = 21;
				draw_action_box(w, 2, 1, 0, rule);
            draw_iface(w, 2, 51, 1, rule);
            break;
			case 3:
				p = 21;
				draw_action_box(w, 2, 1, 0, rule);
            draw_iface(w, 2, 51, 1, rule);
            break;
			case 4:
				p = 24;
				draw_protocol_box(w, 7, 1, 0, rule);
            draw_options(w, 7, 51, 1, rule);
            break;
			case 5:
				p = 24;
				draw_protocol_box(w, 7, 1, 0, rule);
				draw_options(w, 7, 51, 1, rule);	
				break;
			case 6:
				p = 24;
            draw_protocol_box(w, 7, 1, 0, rule);
            draw_options(w, 7, 51, 1, rule); 
            break;
			case 7:
				p = 24;
            draw_protocol_box(w, 7, 1, 0, rule);
            draw_options(w, 7, 51, 1, rule); 
            break;
			case 21:
				p = 5;
				draw_iface(w, 2, 51, 0, rule);
				draw_protocol_box(w, 7, 1, 1, rule);
				break;
			case 22:
		 		p = 5;
            draw_iface(w, 2, 51, 0, rule);
            draw_protocol_box(w, 7, 1, 1, rule);
            break;
			case 23:
	 			p = 5;
            draw_iface(w, 2, 51, 0, rule);
            draw_protocol_box(w, 7, 1, 1, rule);
            break;
			}
			break;

		}
	}

	//draw_action_box(w, 2, 1, 1); 	
	//draw_protocol_box(w, 7, 1, 2);

	return 0;
}

int proto_filter_nav(WINDOW *w, struct ruleset r, int selected)
{
	return 0;
}

 
