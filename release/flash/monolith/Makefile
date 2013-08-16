#
# This is the subdirectory makefile for the crunched filesystem image
# used by the CDROM install. A variable called PROG is used to
# name the resultant hunk of code.
#
# Initially, the behaviour of crunchgen(1) posed us with a problem:
# how to make sure that the PROG gets built in the MAKEOBJDIRPREFIX
# source tree. crunchgen ONLY operates in the current directory. Let's
# fix this nonsense once and for all by hacking crunchgen to DTRT.
#

PROG=		monolith1
BINDIR=		/sbin
NOMAN=

# Extract an absolute build path for the current source location.
CSRCROOT= $(.CURDIR)/../../..

all: monolith

monolith: monolith1

monolith1:
	@cat ${.CURDIR}/monolith.conf | sed -e \
	"s@/usr/local/closedbsd/@${CSRCROOT}/@" > ${.OBJDIR}/monolith1.conf
	cd ${.OBJDIR}
	@crunchgen monolith1.conf
	@${MAKE} -f monolith1.mk -DNOPAM all \
	"CFLAGS=${CFLAGS} -DRELEASE_CRUNCH -DCRUNCHED_BINARY -DNOSECURE -DNOCRYPT" #2>&1 >/dev/null

CLEANFILES=	*.o *.stub *.lo *_stub.c *.mk \
		monolith.cache \
		monolith.mk \
		monolith.c \
		monolith1* \
		monolith \
		.tmp_* \
		*~ \
		*.gz

.include <bsd.prog.mk>
