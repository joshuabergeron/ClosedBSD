/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

/* XXX: !!!! */

#include <sys/types.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/sysctl.h>
#include <sys/un.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp_var.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <pcap.h>
#include <err.h>
#include <errno.h>
#include <osreldate.h>
#include <stdio.h>
#include <unistd.h>
#include <dialog.h>
#include <curses.h>
#include <string.h>
#include <fcntl.h>


/* function declarations */
int filter_packet(u_char *, struct pcap_pkthdr *, u_char *);
int log_syn(struct ip *, struct tcphdr *);
int mon();
int log_packet();
char *hostlookup(unsigned int);
char *get_date(void);

/* default packet filter - what could be easier :-) */
#define DEFAULT_FILTER  "tcp"
