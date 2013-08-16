PROG=	config

CFLAGS=	-Wall -O2
LDADD=	-lpcap -lncurses -ldialog -lmytinfo
NOMAN=

SRCS=	config.c
SRCS+=	menu.c devices.c gauge.c
SRCS+=	ifcon.c fwmenu.c db.c rules.c
SRCS+=	mon.c forward.c pipe.c filter.c
SRCS+=	misc.c sum.c nat.c

.include <bsd.prog.mk>
