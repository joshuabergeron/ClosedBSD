/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "mon.h"

int fddipad;

/* some global variables (ugh!) */
pcap_t *ip_socket;
int zdlt_len = 4;
WINDOW *w;
int y = 10;
unsigned long int packs = 0;


/* main function: parses arguments, sets up pcap, and goes into loop */
int 
mon()
{
	char *interface = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct bpf_program prog;
	bpf_u_int32 network, netmask;
	int promisc = 0;
	char *clog_filter = DEFAULT_FILTER;
	int n;
	int k;
	int key;

	init_dialog();
	dialog_clear();

	nodelay(w, TRUE);
	while ((key = wgetch(w) != ERR)) {
		switch (key) {
		default:
			dialog_clear();
			return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
		}
	}

	/* if we didnt specify a interface, have pcap find a suitable one */
	interface = pcap_lookupdev(errbuf);
	if (interface == NULL) {
		dialog_msgbox("ClosedBSD", "No suitable devices found", -1, -1, 1);
		dialog_clear();
		return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
	}
	if (pcap_lookupnet(interface, &network, &netmask, errbuf) < 0) {
		endwin();
		return -1;
	}
	dialog_clear();
	w = newwin(22, 74, 2, 2);
	draw_box(w, 0, 0, 22, 74, dialog_attr, border_attr);
	draw_shadow(w, 0, 0, 23, 75);

	wattrset(w, title_attr);
	mvwprintw(w, 0, 22, "[ ClosedBSD: Network Monitor ]");

	wattrset(w, check_selected_attr);
	for (k = 1; k <= 72; k++)
		mvwprintw(w, 3, k, " ");

	mvwprintw(w, 3, 1, "Protocol");
	mvwprintw(w, 3, 20, "Packets in");
	mvwprintw(w, 3, 39, "Packets out");
	mvwprintw(w, 3, 57, "Packets total");

	wattrset(w, dialog_attr);
	mvwprintw(w, 4, 1, "     TCP");
	mvwprintw(w, 5, 1, "     UDP");
	mvwprintw(w, 6, 1, "    ICMP");
	wrefresh(w);

	ip_socket = pcap_open_live(interface, 1024, promisc, 1024, errbuf);
	if (ip_socket == NULL) {
		/* fprintf(stderr, "pcap_open_live: %s\n", errbuf); */
		return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
	}
	switch (pcap_datalink(ip_socket)) {
	case DLT_EN10MB:
		zdlt_len = 14;
		break;
	default:
		zdlt_len = 4;
		break;
	}

	if (pcap_compile(ip_socket, &prog, clog_filter, 1, netmask) < 0) {
		/* fprintf(stderr, "pcap_compile: %s\n", errbuf); */
	}
	if (pcap_setfilter(ip_socket, &prog) < 0) {
		/* fprintf(stderr, "pcap_setfilter: %s\n", errbuf); */
	}
	/*
	 * display some useful information: interface, filter, promisc
	 * status
	 */
	wattrset(w, dialog_attr);
	mvwprintw(w, 1, 1, "Using interface: %s", interface);
	wattrset(w, check_selected_attr);
	for (n = 1; n <= 72; n++)
		mvwprintw(w, 9, n, " ");
	mvwprintw(w, 9, 22, "[ 10 most recent connections ]");
	wmove(w, 0, 0);
	wrefresh(w);

	log_packet();

	/* go into loop processing packets */
	while (1) {
		log_packet();
		nodelay(w, TRUE);
		while ((key = wgetch(w) != ERR)) {
			switch (key) {
			default:
				return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
			}
		}

		pcap_dispatch(ip_socket, 1, (pcap_handler) filter_packet, NULL);
		wrefresh(w);
	}

	/* never reached */
	end_dialog();
	endwin();

	return 0;
}

/* this is the function that's called when pcap reads a packet */
int 
filter_packet(u_char *u, struct pcap_pkthdr * p, u_char *packet)
{
	/* some defines we need - this only works with ethernet devices */
#define IP_SIZE	20
#define TCP_SIZE	20

	unsigned short ip_options = 0;
	struct ip *ip;
	struct tcphdr *tcp;
	static u_char *align_buf = NULL;
	int key;

	log_packet();

	nodelay(w, TRUE);
	while ((key = wgetch(w) != ERR)) {
		switch (key) {
		default:
			dialog_clear();
			return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
		}
	}

	/* p->len should never be smaller than the smallest possible packet */
	if (p->len < (zdlt_len + IP_SIZE + TCP_SIZE))
		return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;

	/* cast a ip pointer */
	ip = (struct ip *) (packet + zdlt_len);

	/* align packet if needed */
	if (align_buf == NULL)
		align_buf = (u_char *)malloc(4096);

	bcopy((char *)ip, (char *)align_buf, p->len);
	packet = align_buf;
	ip = (struct ip *) align_buf;

	/* lame hack needed for PPP */
	if (ip->ip_p != IPPROTO_TCP)
		return 0;

	/* determine length of ip options (usually 0) */
	ip_options = ip->ip_hl;
	ip_options -= 5;
	ip_options *= 4;

	/* cast tcp pointer */
	tcp = (struct tcphdr *) (packet + IP_SIZE + ip_options);

	/* nuke any flags in the offset field */
	ip->ip_off &= 0xFF9F;

	/* toss packets where the fragmentation offset is not 0 */
	if (ip->ip_off != 0)
		return 0;


	/* if a syn was recieved, but a ack was not, log the packet */
	if ((tcp->th_flags & TH_SYN) && !(tcp->th_flags & TH_ACK)) {
		log_syn(ip, tcp);

		if (y < 19)
			y++;
		else
			y = 10;
	} else {
		log_packet();
	}

	bzero(ip, sizeof(ip));
	bzero(tcp, sizeof(tcp));

	return 0;
}

int 
log_packet()
{
	int mib[4];
	struct tcpstat t;
	struct udpstat u;
	struct icmpstat ic;
	int len;
	int n;
	int ii = 0, io = 0;

	wattrset(w, dialog_attr);

	/* tcp */
	mib[0] = CTL_NET;
	mib[1] = PF_INET;
	mib[2] = IPPROTO_TCP;
	mib[3] = TCPCTL_STATS;
	len = sizeof(struct tcpstat);
	bzero(&t, sizeof(t));
	if (sysctl(mib, 4, &t, &len, NULL, 0) < 0) {
		return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
	}
	mvwprintw(w, 4, 20, "%10lu", t.tcps_rcvtotal);
	mvwprintw(w, 4, 40, "%10lu", t.tcps_sndtotal);
	mvwprintw(w, 4, 60, "%10lu", t.tcps_sndtotal + t.tcps_rcvtotal);

	/* udp */
	mib[0] = CTL_NET;
	mib[1] = PF_INET;
	mib[2] = IPPROTO_UDP;
	mib[3] = UDPCTL_STATS;
	len = sizeof(struct udpstat);
	bzero(&u, sizeof(u));
	if (sysctl(mib, 4, &u, &len, NULL, 0) < 0) {
		return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
	}
	mvwprintw(w, 5, 20, "%10lu", u.udps_ipackets);
	mvwprintw(w, 5, 40, "%10lu", u.udps_opackets);
	mvwprintw(w, 5, 60, "%10lu", u.udps_ipackets + u.udps_opackets);

	/* icmp */
	mib[0] = CTL_NET;
	mib[1] = PF_INET;
	mib[2] = IPPROTO_ICMP;
	mib[3] = ICMPCTL_STATS;
	len = sizeof(struct icmpstat);
	bzero(&ic, sizeof(ic));
	if (sysctl(mib, 4, &ic, &len, NULL, 0) < 0) {
		return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
	}
	for (n = 0; n < (ICMP_MAXTYPE + 1); n++) {
		if (ic.icps_outhist[n] > 0)
			io += ic.icps_outhist[n];
		if (ic.icps_inhist[n] > 0)
			ii += ic.icps_inhist[n];
	}

	wattrset(w, dialog_attr);
	mvwprintw(w, 6, 20, "%10d", ii);
	mvwprintw(w, 6, 40, "%10d", io);
	mvwprintw(w, 6, 60, "%10d", ii + io);

	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}

/* this function logs the interesting packets */
int 
log_syn(struct ip * ip, struct tcphdr * tcp)
{
	char buf[80];

	wattrset(w, dialog_attr);
	snprintf(buf, sizeof(buf), "%s : %.15s:%u -> ", get_date(), hostlookup(ip->ip_src.s_addr), ntohs(tcp->th_sport));

	mvwprintw(w, y, 2, "%s", buf);
	mvwprintw(w, y, 2 + strlen(buf), "%.25s:%u", hostlookup(ip->ip_dst.s_addr), ntohs(tcp->th_dport));

	wattrset(w, dialog_attr);
	wmove(w, 0, 0);
	wrefresh(w);
	return 0;
}

/* return either a hostname, or a ip address */
char *
hostlookup(unsigned int in)
{
	static char blah[4096];
	struct in_addr i;
	struct hostent *he = NULL;
	int resolve = 0;

	i.s_addr = in;

	if (resolve == 1) {
		he = gethostbyaddr((char *)&i, sizeof(struct in_addr), AF_INET);
	}
	if (he == NULL)
		strncpy(blah, inet_ntoa(i), 4095);
	else
		strncpy(blah, he->h_name, 4095);

	return (char *)blah;
}

/* print the time in a readable way, sorta like asctime() cept different */
char *
get_date(void)
{
	struct tm *timeptr;
	time_t timer;
	static char buffer[40];
	char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	time(&timer);
	timeptr = localtime(&timer);
	sprintf(buffer, "%s %02d %02d:%02d", months[timeptr->tm_mon], timeptr->tm_mday, timeptr->tm_hour, timeptr->tm_min);

	return buffer;
}
