#
# This is the kernel subdirectory build Makefile. This relies on appropriate
# FreeBSD kernel source being installed under $SRC/sys.
#

KERNCONF=	CLOSED_${TARGET_MEDIA}
CLEANDIRS=	${REALOBJDIR}

# Nasty hack to handle the fact that building the kernel in the current
# directory will clobber this Makefile.
#
.if ${.OBJDIR} == ${.CURDIR}
REALOBJDIR=${.CURDIR}/obj
.else
REALOBJDIR=${.OBJDIR}
.endif

all: ${REALOBJDIR}/kernel.${KERNCONF}

${REALOBJDIR}/kernel.${KERNCONF}: kernel

kernel: ${.CURDIR}/CLOSED_CDROM
.if !exists(${SRC}/sys/Makefile)
	echo "The FreeBSD kernel source does not appear to be installed" \
	     "under the specified path ${SRC}." ; exit 20
.endif
	( cd ${SRC}/sys/${TARGET_ARCH}/conf ; \
	  config -d ${REALOBJDIR} ${.CURDIR}/CLOSED_CDROM )
	( cd ${REALOBJDIR} ; make -DNO_MODULES depend ; make -DNO_MODULES )

.include <bsd.prog.mk>
