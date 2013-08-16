# $FreeBSD: src/sbin/dhclient/Makefile,v 1.6.2.5 2001/04/25 14:04:15 ru Exp $

DIST_DIR= /usr/src/contrib/isc-dhcp

.PATH: ${DIST_DIR}/client ${DIST_DIR}/common

PROG=	dhclient
SRCS=	clparse.c dhclient.c
SRCS+=	alloc.c bpf.c conflex.c convert.c dispatch.c errwarn.c ethernet.c \
	hash.c icmp.c inet.c inet_addr.c memory.c nit.c options.c packet.c \
	parse.c print.c raw.c socket.c tables.c tree.c upf.c

CFLAGS+= -I${DIST_DIR}/includes -I${DIST_DIR}
CFLAGS+= -DCLIENT_PATH='"PATH=/sbin:/bin:/usr/sbin:/usr/bin"'

SCRIPTS=	scripts/freebsd
SCRIPTSNAME=	dhclient-script

NOMAN=YES

.include <bsd.prog.mk>
