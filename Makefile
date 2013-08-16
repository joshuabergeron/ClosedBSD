#
# The following build tools are required, in addition to the FreeBSD
# base install, on the host system:-
# mkisofs and the md driver.
#
# Many of the targets which appear in Makefile.inc1 in the core FreeBSD
# build process will actually wind up appearing here, because we only
# build using ELF support (not a.out).

# ----------------------------------------------------------------------------
# User-driven targets:
# iso		- Build the CDROM image of ClosedBSD. 
# floppy	- Build the floppy image of ClosedBSD.
# flash		- Build a CompactFlash image (including partitions).
#
# Implied targets:
# depend	- Recursive depend for userland world.
# buildworld	- Build the ClosedBSD world (binaries).
#
# Convenience targets:
# update 	- Convenience target to update ClosedBSD via CVS
# checkout	- Checkout ClosedBSD from $CVSROOT; overwrites this file.
# ----------------------------------------------------------------------------

# Set the default make target.
.MAIN:  all

# ----------------------------------------------------------------------------
# Pull in user-configurable build defaults.
# ----------------------------------------------------------------------------

TGTS=	all burnflash burnfloppy burniso buildworld buildkernel checkout \
	clean cleandepend cleandir depend flash floppy hierarchy iso kernel \
	libraries mk obj objlink tags update 

# XXX: Currently assumes that the BSD make includes are available on
# the host system in the path ${SHAREMKDIR}, which defaults to /usr/share/mk.
# XXX: Also assumes that the 'md' memory disk driver is available, as is
# the mkisofs(8) utility; also relies on /usr/bin/crunchgen.

PATH=   /sbin:/bin:/usr/sbin:/usr/bin:/usr/local/bin:/usr/local/sbin
MAKE=   PATH=${PATH} make -m ${.CURDIR}/share/mk -f Makefile.inc1

STARTTIME!= LC_ALL=C date

# ----------------------------------------------------------------------------

#
# A simple test target used as part of the test to see if make supports
# the -m argument.
#
test:

#
# Handle the user-driven targets, using the source relative mk files.
#
${TGTS}:
	@cd ${.CURDIR}; \
		${MAKE} ${.TARGET}
