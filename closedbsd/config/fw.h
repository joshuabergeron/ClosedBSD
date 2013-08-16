/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

/* XXX: nested includes very bad! */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dialog.h>
#include <sys/stat.h>
#include <ncurses.h>
#include <db.h>

#define RULES "/etc/rules.db"
#define RULESCRIPT "/etc/rules"
#define NATSCRIPT "/etc/nat"

/* IPFW commands */
enum { ADD, FLUSH, RAW };

/* actions */
enum { ALLOW, DENY, DIVERT, FWD, RESET };

/* protocols */
enum { IP, TCP, UDP, ICMP };

/* flow cardinality */
enum { ANY, INCOMING, OUTGOING };

/* TCP state flags */
enum { NONE, SETUP, ESTABLISHED, FRAG };

/* IP options */
enum { IP_NONE, SSRR, LSRR, TS };

/* ICMP options */
enum { ICMP_NONE, ECHO_REPLY, DEST_UNREACH, ECHO_REQUEST };

struct ruleset {
	int id;
	int cmd;
	int action;
	int proto;
	int ifs;
	int option;
	unsigned char src[128];
	unsigned char dst[128];
	unsigned char src_port[5];
	unsigned char dst_port[5];
	unsigned char other_a[256];
	unsigned char other_b[256];
};

/*
 * menu actions
 */
int _afm_flush(dialogMenuItem * self);
int _afm_list(dialogMenuItem * self);
int _afm_delete(dialogMenuItem * self);
int _afm_save(dialogMenuItem * self);
int _afi_menu(dialogMenuItem * self);
int _afi_filter(dialogMenuItem * self);
int _afi_forward(dialogMenuItem * self);
int _afi_limit(dialogMenuItem * self);
int _afi_manual(dialogMenuItem * self);

/*
 * menu functions
 */
int enable_nat(char *device);
int help(WINDOW * w, int selected, struct ruleset r);

/*
 * curses helper functions from misc.c
 */
int ip_list(WINDOW * w, char *title, int y, int x);
int _std_window(WINDOW * w, char *title);
int draw_host_inputbox(WINDOW * w, int y, int x);
int draw_port_inputbox(WINDOW * w, int y, int x);
int draw_protocol_box(WINDOW * w, int y, int x, int selected, struct ruleset r);
int draw_action_box(WINDOW * w, int y, int x, int selected, struct ruleset r);
int draw_options(WINDOW * w, int y, int x, int selected, struct ruleset r);
int draw_iface(WINDOW * w, int y, int x, int selected, struct ruleset r);
int inputs(WINDOW * w, int selected, char src[128], char dst[128], char src_port[5], char dst_por[5]);
int buttons(WINDOW * w, int selected);
int draw_protocol_forward_box(WINDOW * w, int y, int x, int selected, struct ruleset r);
int forw_input(WINDOW * w, int selected, char src[128], char dst[128], char forw[128], char src_port[5], char dst_port[5], char forw_port[5]);
int draw_nat_option(WINDOW * w, int y, int x, int selected, int enabled);

/* filter.c */
int proto_filter();

/* forward.c */
int proto_forward();
int insert_rule(struct ruleset r);
int __insert_rule(struct ruleset r);
int next_rule();
int delete_rule();
int drop_rule(dialogMenuItem * self);
int raw_add_rule(char *rule);

/* curses.c */
void print_button(WINDOW * w, unsigned char *label, int y, int x, int selected);
