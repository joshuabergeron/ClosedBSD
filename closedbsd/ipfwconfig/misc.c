/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"
#include "fw.h"

int _std_window(WINDOW *w, char *title)
{
	draw_box(w, 0, 0, 22, 76, dialog_attr, border_attr);
	wattrset(w, title_attr);
	mvwprintw(w, 0, ((76/2) - (strlen(title)/ 2)), "%s", title);  
	draw_shadow(stdscr, 1, 1, 22, 76);
	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}

int ip_list(WINDOW *w, char *title, int y, int x)
{
	draw_box(w, y, x, 10, 30, dialog_attr, border_attr);
	wattrset(w, title_attr);
   mvwprintw(w, 0, ((y/2) - (strlen(title) / 2)), "%s", title);
   wrefresh(w);
	return 0;
}
	
int help(WINDOW *w, int selected, struct ruleset r)
{
	wattrset(w, dialog_attr);
	mvwprintw(w, 1, 1, "                                                                         "); 
	wrefresh(w);

	switch(selected) {
	case 1:
		mvwprintw(w, 1, 1, "Allow all matching packets");
		break;

	case 2:
		mvwprintw(w, 1, 1, "Deny all matching packets");		
		break;

	case 3:
		mvwprintw(w, 1, 1, "Discard packet and send RST (TCP OPTION ONLY)");
		break;
	
	case 4:
		mvwprintw(w, 1, 1, "Match TCP Packets");
		break;
	
	case 5:
		mvwprintw(w, 1, 1, "Match IP Packets");
		break;
		
	case 6:
		mvwprintw(w, 1, 1, "Match UDP Packets");
		break;

	case 7:
		mvwprintw(w, 1, 1, "Match ICMP Packets");
		break;
	
	case 8:
		mvwprintw(w, 1, 1, "Enter the source (address, address:netmask, address/CIDR, me, or any)");
		break;

	case 9:
		mvwprintw(w, 1, 1, "Enter the port (the number, or \"any\")");
		break;
	
	case 10:
		mvwprintw(w, 1, 1, "Enter the dest (address, address:netmask, address/CIDR, me, or any)");
		break;
	
	case 11:
		mvwprintw(w, 1, 1, "Enter the port (the number, or \"any\")");
		break;

	case 12:
		mvwprintw(w, 1, 1, "Enter the address to forward to (addr, addr:mask, addr/CIDR, me, or any)");
		break;
 
	case 24:
		switch(r.proto) {
		case UDP:
			mvwprintw(w, 1, 1, "There are no UDP header options");
			break;
 		case ICMP:
			mvwprintw(w, 1, 1, "Select the ICMP header option for ICMP echo requests");
			break;
		case TCP:
			mvwprintw(w, 1, 1, "Select the TCP header option for established TCP connections");
			break;
		case IP:
			mvwprintw(w, 1, 1, "Select the IP header option for strict source routes");
			break;
		}
		break;
	
	case 25:
		switch(r.proto) {
		case UDP:
			mvwprintw(w, 1, 1, "There are no UDP header options");
			break;
		case ICMP:
			mvwprintw(w, 1, 1, "Select the ICMP header option for ICMP destination unreachable");
			break;
		case TCP:
			mvwprintw(w, 1, 1, "Select the TCP header option for SYN flags set");
			break;
		case IP:
			mvwprintw(w, 1, 1, "Select the IP header option for loose source routes");
			break;	
		}
		break;

	case 26:
		switch(r.proto) {
		case UDP:
			mvwprintw(w, 1, 1, "There are no UDP header options");
			break;
		case ICMP:
			mvwprintw(w, 1, 1, "Select the ICMP header option for ICMP echo replys");
			break;
		case TCP:
			mvwprintw(w, 1, 1, "Select the TCP header option for fragmented packets");
			break;
		case IP:
			mvwprintw(w, 1, 1, "Select the IP header option for packets with timestamps");
			break;
		}
		break;

	case 31:
		mvwprintw(w, 1, 1, "Forward matching TCP packets");
		break;

	case 32:
		mvwprintw(w, 1, 1, "Forward matching UDP packets");
		break;

	case 33:
		mvwprintw(w, 1, 1, "Insert forwarding rule");
		break;

	case 34:
		mvwprintw(w, 1, 1, "Cancel rule");
		break;

	case 27:
		switch(r.proto) {
		case UDP:
			mvwprintw(w, 1, 1, "There are no UDP header options");
			break;
		case ICMP:
			mvwprintw(w, 1, 1, "Select no additional ICMP header options");
			break;
		case TCP:
			mvwprintw(w, 1, 1, "Select no additional TCP header options");
			break;
		case IP:
			mvwprintw(w, 1, 1, "Select no additional IP header options");
			break;	
		}
		break;

	case 40:
		mvwprintw(w, 1, 1, "Rule will only apply towards incoming packets");
		break;
	case 41:
		mvwprintw(w, 1, 1, "Rule will only apply towards outgoing packets");
		break;
	case 42:
		mvwprintw(w, 1, 1, "Rule will apply to all packets (incoming and outgoing)");
		break;
	
	default:
		break;	
	}

	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}

int buttons(WINDOW *w, int selected)
{
	switch(selected) {
	case 0:
		print_button(w, "   OK   ", 20, 20, FALSE);
		print_button(w, " Cancel ", 20, 46, FALSE);
		break; 
	case 1:
		print_button(w, "   OK   ", 20, 20, TRUE);
		print_button(w, "	Cancel ", 20, 46, FALSE);
		break;
	case 2:
		print_button(w, "   OK   ", 20, 20, FALSE);  
		print_button(w, " Cancel ", 20, 46, TRUE);
		break;
	}
	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}

int forw_input(WINDOW *w, int selected, char src[128], char dst[128], char forw[128], char src_port[5], char dst_port[5], char forw_port[5])
{
	int key;
	int first = 1;
	
	switch(selected) {
	case 3:
		key = line_edit(w, 8, 20, 33, 33, inputbox_attr, first, src, DialogInputAttrs);
		switch(key) { 
		case KEY_DOWN:
			first = 0;
			return selected+1;
		case KEY_UP:
			first = 0;
			return selected-1;
		}
		break; 
	
	case 4:
	 	key = line_edit(w, 8, 56, 5, 5, inputbox_attr, first, src_port, DialogInputAttrs);
      switch(key) { 
      case KEY_DOWN:
         first = 0;
         return selected+1;
      case KEY_UP:
         first = 0;
         return selected-1;
      }
      break;

	case 5:
		key = line_edit(w, 11, 20, 33, 33, inputbox_attr, first, dst, DialogInputAttrs);
      switch(key) { 
      case KEY_DOWN:
         first = 0;
         return selected+1;
      case KEY_UP:
         first = 0;
         return selected-1;
      }
      break;
	
	case 6:
		key = line_edit(w, 11, 56, 5, 5, inputbox_attr, first, dst_port, DialogInputAttrs);
      switch(key) { 
      case KEY_DOWN:
         first = 0;
         return selected+1;
      case KEY_UP:
         first = 0;
         return selected-1;
      }
      break;
	
	case 7:
		key = line_edit(w, 14, 20, 33, 33, inputbox_attr, first, forw, DialogInputAttrs);
		switch(key) {
		case KEY_DOWN:
			first = 0;
			return selected+1;
		case KEY_UP:
			first = 0;	
			return selected-1;	
		}
		break;
	
	case 8:
		key = line_edit(w, 14, 56, 5, 5, inputbox_attr, first, forw_port, DialogInputAttrs);
		switch(key) {
		case KEY_DOWN:
			first = 0;
			return selected+1;
		case KEY_UP:
			first = 0;
			return selected-1;
		}
		break; 
	}
	
	wrefresh(w);
	return selected;	
}

int inputs(WINDOW *w, int selected, char src[128], char dst[128], char src_port[5], char dst_port[5])
{
	int key; 
	int first = 1;

	switch(selected) {
	case 8:
		key = line_edit(w, 14, 17, 33, 33, inputbox_attr, first, src, DialogInputAttrs);
		switch(key) {
		case KEY_DOWN:
			first = 0;
			return selected + 1;
		case KEY_UP:
			first = 0;
			return selected - 1;	
		}
		break;

	case 9:
		key = line_edit(w, 14, 52, 5, 5, inputbox_attr, first, src_port, DialogInputAttrs); 
		switch(key) {
		case KEY_DOWN:
			first = 0;
			return selected + 1;
		case KEY_UP:
			first = 0;
			return selected - 1;
		}
		break;	
	
	case 10:
		key = line_edit(w, 17, 17, 33, 33, inputbox_attr, first, dst, DialogInputAttrs);
		switch(key) {
		case KEY_DOWN:
			first = 0;
			return selected + 1;	
		case KEY_UP:
			first = 0;
			return selected - 1;
		}
		break;
	
	case 11:
		key = line_edit(w, 17, 52, 5, 5, inputbox_attr, first, dst_port, DialogInputAttrs);
		switch(key) {
		case KEY_DOWN:
			first = 0;
			return selected + 1;
		case KEY_UP:
			first = 0;
			return selected - 1;
		}
		break;
	}

	return selected + 1;
	
   wrefresh(w);
	return 0;
}

int draw_options(WINDOW *w, int y, int x, int selected, struct ruleset rule)
{
	draw_box(w, y, x, 6, 24, dialog_attr, border_attr);
	wattrset(w, title_attr);

	switch(rule.proto) {
	case UDP:
		mvwprintw(w, y, (x + 12) - 6, "UDP options:");
		wrefresh(w);
		switch(selected) {
		case 0:
			wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1, " No UDP options       ");
      	mvwprintw(w,y+2,x+1, " No UDP options       ");
			mvwprintw(w,y+3,x+1, " No UDP options       ");
			mvwprintw(w,y+4,x+1, " No UDP options       ");
			break;
  
		case 1:
         wattrset(w, check_selected_attr);
			mvwprintw(w,y+1,x+1, "[No UDP options      ]");
         wattrset(w, dialog_attr);
			mvwprintw(w,y+2,x+1, " No UDP options       ");
         mvwprintw(w,y+3,x+1, " No UDP options       ");
         mvwprintw(w,y+4,x+1, " No UDP options       ");
         break;

		case 2:
	 		wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1, " No UDP options       ");
         wattrset(w, check_selected_attr);
			mvwprintw(w,y+2,x+1, "[No UDP options      ]");
         wattrset(w, dialog_attr);
			mvwprintw(w,y+3,x+1, " No UDP options       ");
         mvwprintw(w,y+4,x+1, " No UDP options       ");
         break;

		case 3:
	 		wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1, " No UDP options       ");
         mvwprintw(w,y+2,x+1, " No UDP options       ");
         wattrset(w, check_selected_attr);
			mvwprintw(w,y+3,x+1, "[No UDP options      ]");
         wattrset(w, dialog_attr);
			mvwprintw(w,y+4,x+1, " No UDP options       ");
         break;

		case 4:
	 		wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1, " No UDP options       ");
         mvwprintw(w,y+2,x+1, " No UDP options       ");
         mvwprintw(w,y+3,x+1, " No UDP options       ");
         wattrset(w, check_selected_attr);
			mvwprintw(w,y+4,x+1, "[No UDP options      ]");
         break;
		}
		break;

	case ICMP:
		mvwprintw(w, y, (x + 12) - 6, "ICMP options:");
		wrefresh(w);
		switch(selected) {
		case 0:
		   switch(rule.option) {
         case ICMP_NONE:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " (*)   None           ");
            break;

         case ECHO_REQUEST:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " (*)   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;

         case DEST_UNREACH:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " (*)   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;

         case ECHO_REPLY:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " (*)   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
         }
			break;

		case 1:
		 switch(rule.option) {
         case ICMP_NONE:
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+1,x+1, "[( )   Echo Request  ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " (*)   None           ");
            break;

         case ECHO_REQUEST:
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+1,x+1, "[(*)   Echo Request  ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;

         case DEST_UNREACH:
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+1,x+1, "[( )   Echo Request  ]");
           	wattrset(w, dialog_attr); 
				mvwprintw(w,y+2,x+1, " (*)   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;

         case ECHO_REPLY:
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+1,x+1, "[( )   Echo Request  ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " (*)   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
         }
			break;

		case 2:
			switch(rule.option) {
         case ICMP_NONE:
            wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+2,x+1, "[( )   Dest. Unreach ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " (*)   None           ");
            break;
            
         case ECHO_REQUEST:
            wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " (*)   Echo Request   ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[( )   Dest. Unreach ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
            
         case DEST_UNREACH:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+2,x+1, "[(*)   Dest. Unreach ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
            
         case ECHO_REPLY:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+2,x+1, "[( )   Dest. Unreach ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " (*)   Echo Reply     ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
         }
			break;	
		
		case 3:
		   switch(rule.option) {
         case ICMP_NONE:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+3,x+1, "[( )   Echo Reply    ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " (*)   None           ");
            break;
            
         case ECHO_REQUEST:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " (*)   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+3,x+1, "[( )   Echo Reply    ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
         
         case DEST_UNREACH:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " (*)   Dest. Unreach  ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+3,x+1, "[( )   Echo Reply    ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
            
         case ECHO_REPLY:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+3,x+1, "[(*)   Echo Reply    ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
         }
         break;

		case 4:
		   switch(rule.option) {
         case ICMP_NONE:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+4,x+1, "[(*)   None          ]");
            break;
            
         case ECHO_REQUEST:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " (*)   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+4,x+1, "[( )   None          ]");
            break;
         
         case DEST_UNREACH:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " (*)   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " ( )   Echo Reply     ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+4,x+1, "[( )   None          ]");
            break;
            
         case ECHO_REPLY:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   Echo Request   ");
            mvwprintw(w,y+2,x+1, " ( )   Dest. Unreach  ");
            mvwprintw(w,y+3,x+1, " (*)   Echo Reply     ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+4,x+1, "[( )   None          ]");
            break;
         }
         break;
		}
		break;
 
	case IP:
		mvwprintw(w, y, (x + 12) - 6, "IP options:");
		wrefresh(w);
		switch(selected) {
		case 0:
			switch(rule.option) {
			case IP_NONE:
				wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " (*)   None           ");
            break;
	
			case SSRR:
				wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " (*)   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
		
			case LSRR:
				wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " (*)   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
	
			case TS:		
				wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " (*)   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
			}
			break;
		
		case 1:
         switch(rule.option) {
         case IP_NONE:
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+1,x+1, "[( )   SSRR          ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " (*)   None           ");
            break;
  
         case SSRR:
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+1,x+1, "[(*)   SSRR          ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;

         case LSRR:
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+1,x+1, "[( )   SSRR          ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " (*)   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
  
         case TS:
            wattrset(w, check_selected_attr); 
				mvwprintw(w,y+1,x+1, "[( )   SSRR          ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " (*)   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
         }
         break;
	
		case 2:
         switch(rule.option) {
         case IP_NONE:
				wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[( )   LSRR          ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " (*)   None           ");
            break;
  
         case SSRR:
            wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " (*)   SSRR           ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[( )   LSRR          ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;

         case LSRR:
            wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[(*)   LSRR          ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
  
         case TS:
				wattrset(w, dialog_attr); 
				mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[( )   LSRR          ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " (*)   TS             ");
            mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
         }
         break;
	
		case 3:
         switch(rule.option) {
         case IP_NONE:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+3,x+1, "[( )   TS            ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " (*)   None           ");
            break;
  
         case SSRR:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " (*)   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+3,x+1, "[( )   TS            ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;

         case LSRR:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " (*)   LSRR           ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+3,x+1, "[( )   TS            ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
  
         case TS:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            wattrset(w, check_selected_attr);
				mvwprintw(w,y+3,x+1, "[(*)   TS            ]");
            wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
            break;
         }
         break;

	 	case 4:
         switch(rule.option) {
         case IP_NONE:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+4,x+1, "[(*)   None          ]");
            break;

         case SSRR:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " (*)   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+4,x+1, "[( )   None          ]");
            break;
          
         case LSRR:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " (*)   LSRR           ");
            mvwprintw(w,y+3,x+1, " ( )   TS             ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+4,x+1, "[( )   None          ]");
            break;
            
         case TS:
            wattrset(w, dialog_attr);
            mvwprintw(w,y+1,x+1, " ( )   SSRR           ");
            mvwprintw(w,y+2,x+1, " ( )   LSRR           ");
            mvwprintw(w,y+3,x+1, " (*)   TS             ");
            wattrset(w, check_selected_attr);
            mvwprintw(w,y+4,x+1, "[( )   None          ]");
            break;
         }
         break;
		}	
		break;
	
	case TCP:
		mvwprintw(w, y, (x + 12) - 6, "TCP options:");
		wrefresh(w);
		switch(selected) {
		case 0:
			switch(rule.option) {
			case NONE:
				wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
				mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    "); 
				mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
				mvwprintw(w,y+4,x+1, " (*)   None           ");
				break;
		
			case SETUP:
				wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	mvwprintw(w,y+2,x+1, " (*)   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
				break;
		
			case ESTABLISHED:
	  			wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " (*)   Established    ");
         	mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;
	
			case FRAG:
				wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " (*)   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;
			}
			break;
	
		case 1:
			switch(rule.option) {
			case NONE:
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+1,x+1, "[( )   Established   ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " (*)   None           ");
         	break;

      	case SETUP:
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+1,x+1, "[( )   Established   ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " (*)   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;

      	case ESTABLISHED:
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+1,x+1, "[(*)   Established   ]");
         	wattrset(w, dialog_attr); 
				mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;
  
      	case FRAG:
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+1,x+1, "[( )   Established   ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " (*)   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;
      	}
      	break;
	
	
		case 2:
			switch(rule.option) {
	   	case NONE:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[( )   Setup (SYN)   ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " (*)   None           ");
         	break;

      	case SETUP:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[(*)   Setup (SYN)   ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;

      	case ESTABLISHED:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " (*)   Established    ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[( )   Setup (SYN)   ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;
  
      	case FRAG:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+2,x+1, "[( )   Setup (SYN)   ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+3,x+1, " (*)   Fragmented     ");
         	mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;
      	}
      	break;
	
		case 3:
			switch(rule.option) {
			case NONE:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+3,x+1, "[( )   Fragmented    ]");
        		wattrset(w, dialog_attr); 
				mvwprintw(w,y+4,x+1, " (*)   None           ");
         	break;

      	case SETUP:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	mvwprintw(w,y+2,x+1, " (*)   Setup (SYN)    ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+3,x+1, "[( )   Fragmented    ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;

      	case ESTABLISHED:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " (*)   Established    ");
         	mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+3,x+1, "[( )   Fragmented    ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;
  
      	case FRAG:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+3,x+1, "[(*)   Fragmented    ]");
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+4,x+1, " ( )   None           ");
         	break;
      	}
      	break;

		case 4:
			switch(rule.option) {
     		case NONE:
        		wattrset(w, dialog_attr); 
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+4,x+1, "[(*)   None          ]");
         	break;

      	case SETUP:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	mvwprintw(w,y+2,x+1, " (*)   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+4,x+1, "[( )   None          ]");
         	break;

      	case ESTABLISHED:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " (*)   Established    ");
         	mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " ( )   Fragmented     ");
         	wattrset(w, check_selected_attr);
				mvwprintw(w,y+4,x+1, "[( )   None          ]");
         	break;
  
      	case FRAG:
         	wattrset(w, dialog_attr);
				mvwprintw(w,y+1,x+1, " ( )   Established    ");
         	mvwprintw(w,y+2,x+1, " ( )   Setup (SYN)    ");
         	mvwprintw(w,y+3,x+1, " (*)   Fragmented     ");
         	wattrset(w, check_selected_attr); 
				mvwprintw(w,y+4,x+1, "[( )   None          ]");
         	break;
      	}
      	break;
		break;
		} // proto switch	
	}	
	
	wmove(w, 0, 0);	
	wrefresh(w);
	return 0;
}

int draw_iface(WINDOW *w, int y, int x, int selected, struct ruleset r)
{
	draw_box(w, y, x, 5, 24, dialog_attr, border_attr);
	wattrset(w, title_attr);
	mvwprintw(w, y, (x + 12) - 5, "Interface:"); 
	wattrset(w, dialog_attr);
	
	switch(selected) {
	case 0:
		switch(r.ifs) {
		case INCOMING:
			mvwprintw(w,y+1,x+1, " (*)   Incoming     ");
			mvwprintw(w,y+2,x+1, " ( )   Outgoing     ");
			mvwprintw(w,y+3,x+1, " ( )   Any          "); 
			break;
		
		case OUTGOING:
			mvwprintw(w,y+1,x+1, " ( )   Incoming     ");
         mvwprintw(w,y+2,x+1, " (*)   Outgoing     ");
         mvwprintw(w,y+3,x+1, " ( )   Any          ");
			break;
		
		case ANY:
			mvwprintw(w,y+1,x+1, " ( )   Incoming     ");
         mvwprintw(w,y+2,x+1, " ( )   Outgoing     ");
         mvwprintw(w,y+3,x+1, " (*)   Any          ");
			break;
		}
		break;

	case 1:
		switch(r.ifs) {
		case INCOMING:
			wattrset(w, check_selected_attr); 
			mvwprintw(w,y+1,x+1, "[(*)   Incoming      ]");
			wattrset(w, dialog_attr);
			mvwprintw(w,y+2,x+1, " ( )   Outgoing       ");
			mvwprintw(w,y+3,x+1, " ( )   Any            ");
			break;
		
		case OUTGOING:
			wattrset(w, check_selected_attr);
         mvwprintw(w,y+1,x+1, "[( )   Incoming      ]");
         wattrset(w, dialog_attr);
         mvwprintw(w,y+2,x+1, " (*)   Outgoing       ");
         mvwprintw(w,y+3,x+1, " ( )   Any            ");
			break;
		
		case ANY: 
			wattrset(w, check_selected_attr);
         mvwprintw(w,y+1,x+1, "[( )   Incoming      ]");
         wattrset(w, dialog_attr);
         mvwprintw(w,y+2,x+1, " ( )   Outgoing       ");
         mvwprintw(w,y+3,x+1, " (*)   Any            ");
			break;
		}
		break;

	case 2:
		switch(r.ifs) {
		case INCOMING:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " (*)   Incoming       ");
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+2,x+1, "[( )   Outgoing      ]");
			wattrset(w, dialog_attr);	
			mvwprintw(w,y+3,x+1, " ( )   Any            ");
			break;	
		
		case OUTGOING:
		 	wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1, " ( )   Incoming       ");
         wattrset(w, check_selected_attr);
         mvwprintw(w,y+2,x+1, "[(*)   Outgoing      ]");
         wattrset(w, dialog_attr);
         mvwprintw(w,y+3,x+1, " ( )   Any            ");	
			break;
		
		case ANY:
			wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1, " ( )   Incoming       ");
         wattrset(w, check_selected_attr);
         mvwprintw(w,y+2,x+1, "[( )   Outgoing      ]");
         wattrset(w, dialog_attr);
         mvwprintw(w,y+3,x+1, " (*)   Any            ");	
			break;
		}
		break;

	case 3:
		switch(r.ifs) {
		case INCOMING:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " (*)   Incoming       ");
			mvwprintw(w,y+2,x+1, " ( )   Outgoing       ");
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+3,x+1, "[( )   Any           ]");
			break;
	
		case OUTGOING:
			wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1, " ( )   Incoming       ");
         mvwprintw(w,y+2,x+1, " (*)   Outgoing       ");
         wattrset(w, check_selected_attr);
         mvwprintw(w,y+3,x+1, "[( )   Any           ]");
         break;
	
		case ANY:
			wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1, " ( )   Incoming       ");
         mvwprintw(w,y+2,x+1, " ( )   Outgoing       ");
         wattrset(w, check_selected_attr);
         mvwprintw(w,y+3,x+1, "[(*)   Any           ]");
         break;
		}
		break;
	}
	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}

int draw_host_inputbox(WINDOW *w, int y, int x)
{
	draw_box(w, y, x, 3, 35, dialog_attr, border_attr);
	wattrset(w, title_attr);
	mvwprintw(w, y, (x + 15) - 6, "address/mask");
	wrefresh(w);
	return 0;
}

int draw_nat_option(WINDOW *w, int y, int x, int selected, int enabled)
{
	draw_box(w, y, x, 4, 50, dialog_attr, border_attr);
	wattrset(w, title_attr);

	switch(selected) {
	case 0:
		switch(enabled) {
		case 0:
			wattrset(w, dialog_attr);
			mvwprintw(w, y+1, x+1, "[ ]  Enable network address translation         ");
			mvwprintw(w, y+2, x+1, "[*]  Disable network address translation        ");
			break;
	
		case 1:
			wattrset(w, dialog_attr);
			mvwprintw(w, y+1, x+1, "[*]  Enable network address translation         ");
			mvwprintw(w, y+2, x+1, "[ ]  Disable network address translation        ");
			break;
		}
		break;
	
	case 1:
		switch(enabled) {
		case 0:
			wattrset(w, check_selected_attr);
			mvwprintw(w, y+1, x+1, "[ ]  Enable network address translation         ");
			wattrset(w, dialog_attr);
			mvwprintw(w, y+2, x+1, "[*]  Disable network address translation        ");
			break;

		case 1:
			wattrset(w, check_selected_attr);
			mvwprintw(w, y+1, x+1, "[*]  Enable network address translation         ");
			wattrset(w, dialog_attr);
			mvwprintw(w, y+2, x+1, "[ ]  Disable network address translation        ");
			break;
		}
		break;

	case 2:
		switch(enabled) {
		case 0:
			wattrset(w, dialog_attr);
			mvwprintw(w, y+1, x+1, "[ ]  Enable network address translation         ");
			wattrset(w, check_selected_attr);
			mvwprintw(w, y+2, x+1, "[*]  Disable network address translation        ");
			break;

		case 1:
			wattrset(w, dialog_attr);
			mvwprintw(w, y+1, x+1, "[*]  Enable network address translation         "); 
			wattrset(w, check_selected_attr);
			mvwprintw(w, y+2, x+1, "[ ]  Disable network address translation        ");
			break;
		}
		break;
	}
	
	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}


int draw_port_inputbox(WINDOW *w, int y, int x)
{
	draw_box(w, y, x, 3, 8, dialog_attr, border_attr);
	wattrset(w, title_attr);
	mvwprintw(w, y, (x + 3) - 1, "port"); 
	wrefresh(w);
	return 0;
}

int draw_action_box(WINDOW *w, int y, int x, int selected, struct ruleset r)
{
	draw_box(w, y, x, 5, 50, dialog_attr, border_attr);

	wattrset(w, title_attr);
	mvwprintw(w, y, x + 25 - 5, " Action: ");

	switch(selected) {
	default:
		switch(r.action) {
		case ALLOW:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " (*) Allow   Allow packets matching this rule   ");
			mvwprintw(w,y+2,x+1, " ( ) Deny    Deny packets matching this rule    ");
			mvwprintw(w,y+3,x+1, " ( ) Reset   Discard packet and send RST (TCP)  ");
			break; 
		
		case DENY:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " ( ) Allow   Allow packets matching this rule   ");
			mvwprintw(w,y+2,x+1, " (*) Deny    Deny packets matching this rule    ");
			mvwprintw(w,y+3,x+1, " ( ) Reset   Discard packet and send RST (TCP)  ");
			break;
		
		case RESET:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " ( ) Allow   Allow packets matching this rule   ");
			mvwprintw(w,y+2,x+1, " ( ) Deny    Deny packets matching this rule    ");
			mvwprintw(w,y+3,x+1, " (*) Reset   Discard packet and send RST (TCP)  ");
			break;
		}
		break;

	case 1:
		switch(r.action) {
		case ALLOW:	
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+1,x+1, "[(*) Allow   Allow packets matching this rule  ]");
			wattrset(w, dialog_attr);
			mvwprintw(w,y+2,x+1, " ( ) Deny    Deny packets matching this rule    "); 
			mvwprintw(w,y+3,x+1, " ( ) Reset   Discard packet and send RST (TCP)  ");
			break;

		case DENY:
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+1,x+1, "[( ) Allow   Allow packets matching this rule  ]");
			wattrset(w, dialog_attr);
			mvwprintw(w,y+2,x+1, " (*) Deny    Deny packets matching this rule    ");
			mvwprintw(w,y+3,x+1, " ( ) Reset   Discard packet and send RST (TCP)  ");
			break;
	
		case RESET:
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+1,x+1, "[( ) Allow   Allow packets matching this rule  ]");
			wattrset(w, dialog_attr);
			mvwprintw(w,y+2,x+1, " ( ) Deny    Deny packets matching this rule    ");
			mvwprintw(w,y+3,x+1, " (*) Reset   Discard packet and send RST (TCP)  ");	
			break;
		}
		break;

	case 2:
		switch(r.action) {
		case ALLOW:	
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " (*) Allow   Allow packets matching this rule   ");
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+2,x+1, "[( ) Deny    Deny packets matching this rule   ]");
			wattrset(w, dialog_attr);
			mvwprintw(w,y+3,x+1, " ( ) Reset   Discard packet and send RST (TCP)  ");
			break;
		
		case DENY:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " ( ) Allow   Allow packets matching this rule   ");
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+2,x+1, "[(*) Deny    Deny packets matching this rule   ]");
			wattrset(w, dialog_attr);
			mvwprintw(w,y+3,x+1, " ( ) Reset   Discard packet and send RST (TCP)  ");
			break;
 
		case RESET:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " ( ) Allow   Allow packets matching this rule   ");
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+2,x+1, "[( ) Deny    Deny packets matching this rule   ]");
			wattrset(w, dialog_attr);
			mvwprintw(w,y+3,x+1, " (*) Reset   Discard packet and send RST (TCP)  ");
   		break;	
		}	
		break;
	
	case 3:
		switch(r.action) {
		case ALLOW:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " (*) Allow   Allow packets matching this rule   ");
			mvwprintw(w,y+2,x+1, " ( ) Deny    Deny packets matching this rule    ");
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+3,x+1, "[( ) Reset   Discard packet and send RST (TCP) ]");
			break; 
		
		case DENY:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " ( ) Allow   Allow packets matching this rule   ");
			mvwprintw(w,y+2,x+1, " (*) Deny    Deny packets matching this rule    ");
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+3,x+1, "[( ) Reset   Discard packet and send RST (TCP) ]"); 
			break;
		
		case RESET:
			wattrset(w, dialog_attr);
			mvwprintw(w,y+1,x+1, " ( ) Allow   Allow packets matching this rule   ");
			mvwprintw(w,y+2,x+1, " ( ) Deny    Deny packets matching this rule    ");
			wattrset(w, check_selected_attr);
			mvwprintw(w,y+3,x+1, "[(*) Reset   Discard packet and send RST (TCP) ]");
			break; 
		}
		break;
	}
	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}

int draw_protocol_forward_box(WINDOW *w, int y, int x, int selected, struct ruleset r)
{
	draw_box(w, y, x, 4, 50, dialog_attr, border_attr);
	
	wattrset(w, title_attr);
	mvwprintw(w, y, x + 25 - 6 , " Protocol: ");
 	
	switch(selected) {
	default:
		switch(r.proto) { 
		case TCP:
			wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1," (*) TCP     Transmission Control Protocol      ");
			mvwprintw(w,y+2,x+1," ( ) UDP     User Datagram Protocol             ");
			break;
		case UDP:
			wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
         mvwprintw(w,y+2,x+1," (*) UDP     User Datagram Protocol             ");
			break; 
		}
		break;
	
	case 1:
		switch(r.proto) {
		case TCP:
			wattrset(w, check_selected_attr);
         mvwprintw(w,y+1,x+1,"[(*) TCP     Transmission Control Protocol     ]");
        	wattrset(w, dialog_attr); 
			mvwprintw(w,y+2,x+1," ( ) UDP     User Datagram Protocol             ");
         break;
		case UDP:
			wattrset(w, check_selected_attr);
         mvwprintw(w,y+1,x+1,"[( ) TCP     Transmission Control Protocol     ]");
         wattrset(w, dialog_attr);
			mvwprintw(w,y+2,x+1," (*) UDP     User Datagram Protocol             ");
         break;
		}
		break;
	
	case 2:
		switch(r.proto) {
		case TCP:
			wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1," (*) TCP     Transmission Control Protocol      ");
         wattrset(w, check_selected_attr);
			mvwprintw(w,y+2,x+1,"[( ) UDP     User Datagram Protocol            ]");
         break;

		case UDP:
			wattrset(w, dialog_attr);
         mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
         wattrset(w, check_selected_attr);
			mvwprintw(w,y+2,x+1,"[(*) UDP     User Datagram Protocol            ]");
         break;
		}
		break;
	}

	wattrset(w, dialog_attr);
	wmove(w, 0, 0);	
	wrefresh(w);
	return selected;
}
 
int draw_protocol_box(WINDOW *w, int y, int x, int selected, struct ruleset r)
{
	draw_box(w, y, x, 6, 50, dialog_attr, border_attr);
	
	wattrset(w, title_attr);
	mvwprintw(w, y, x + 25 - 6 , " Protocol: "); 
	
	switch(selected) {
	default:
		switch(r.proto) {
		case IP:
			wattrset(w, dialog_attr);
	      mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
   	   mvwprintw(w,y+2,x+1," (*) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;
		
		case TCP:
			wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," (*) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;
		
		case UDP:
			wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," (*) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;
		
		case ICMP:
	 		wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," (*) ICMP    Internet Control Message Protocol  ");
      	break;

		}
		break;
	
	case 1:
		switch(r.proto) {
		case IP:
		 	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+1,x+1,"[( ) TCP     Transmission Control Protocol     ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+2,x+1," (*) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;

		case TCP:
	 		wattrset(w, check_selected_attr);
      	mvwprintw(w,y+1,x+1,"[(*) TCP     Transmission Control Protocol     ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;
	
		case UDP:
	 		wattrset(w, check_selected_attr);
      	mvwprintw(w,y+1,x+1,"[( ) TCP     Transmission Control Protocol     ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," (*) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;
	
		case ICMP:
	 		wattrset(w, check_selected_attr);
      	mvwprintw(w,y+1,x+1,"[( ) TCP     Transmission Control Protocol     ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," (*) ICMP    Internet Control Message Protocol  ");
      	break;
		}
		break;	
	
	case 2:
		switch(r.proto) {
		case IP:
			wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+2,x+1,"[(*) IP      Internet Protocol                 ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;

		case TCP:
			wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," (*) TCP     Transmission Control Protocol      ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+2,x+1,"[( ) IP      Internet Protocol                 ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;
	
		case UDP:
			wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+2,x+1,"[( ) IP      Internet Protocol                 ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+3,x+1," (*) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;
	
		case ICMP:	
			wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+2,x+1,"[( ) IP      Internet Protocol                 ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	mvwprintw(w,y+4,x+1," (*) ICMP    Internet Control Message Protocol  ");
      	break;
		}
		break;
	
	case 3: 	
		switch(r.proto) {
		case IP:
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," (*) IP      Internet Protocol                  ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+3,x+1,"[( ) UDP     User Datagram Protocol            ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;

		case TCP:
	      wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," (*) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+3,x+1,"[( ) UDP     User Datagram Protocol            ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;

		case UDP:
	      wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+3,x+1,"[(*) UDP     User Datagram Protocol            ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+4,x+1," ( ) ICMP    Internet Control Message Protocol  ");
      	break;

		case ICMP:
	      wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+3,x+1,"[( ) UDP     User Datagram Protocol            ]");
      	wattrset(w, dialog_attr);
      	mvwprintw(w,y+4,x+1," (*) ICMP    Internet Control Message Protocol  ");
      	break;
		}
		break;
	
	case 4:
		switch(r.proto) {
		case IP:
 			wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," (*) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+4,x+1,"[( ) ICMP    Internet Control Message Protocol ]");
      	break;

		case TCP:
	 		wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," (*) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+4,x+1,"[( ) ICMP    Internet Control Message Protocol ]");
      	break;

		case UDP:
	 		wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," (*) UDP     User Datagram Protocol             ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+4,x+1,"[( ) ICMP    Internet Control Message Protocol ]");
      	break;

		case ICMP: 
	 		wattrset(w, dialog_attr);
      	mvwprintw(w,y+1,x+1," ( ) TCP     Transmission Control Protocol      ");
      	mvwprintw(w,y+2,x+1," ( ) IP      Internet Protocol                  ");
      	mvwprintw(w,y+3,x+1," ( ) UDP     User Datagram Protocol             ");
      	wattrset(w, check_selected_attr);
      	mvwprintw(w,y+4,x+1,"[(*) ICMP    Internet Control Message Protocol ]");
      	break;
		}
		break;
	}

	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}
 
