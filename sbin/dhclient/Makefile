#
# $ClosedBSD$
#
# This is a kludge makefile which wraps the actual makefile in ${DIST_DIR}.
# 

SRC=		/usr/src
DIST_DIR=	${SRC}/sbin/dhclient

.PATH: ${DIST_DIR}/minires ${DIST_DIR}/dst ${DIST_DIR}/omapip ${DIST_DIR}/common ${DIST_DIR}/client

.MAIN: all

all:
	( cd ${DIST_DIR} ; make )

obj:
	( cd ${DIST_DIR} ; make obj )

clean:
	( cd ${DIST_DIR} ; make clean )

cleandir:
	( cd ${DIST_DIR} ; make cleandir )

install:
	( cd ${DIST_DIR} ; make install )

depend:
	( cd ${DIST_DIR} ; make depend )

.include <bsd.prog.mk>
