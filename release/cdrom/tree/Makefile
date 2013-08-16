# ----------------------------------------------------------------------------
#
# This is the staging area for the final preparation of the ClosedBSD
# filesystem image.  The directory staticroot contains non-binary files
# which will be overlaid during the creation process.
#
# Note that this final phase of the build requires root privileges, and
# write access to a filesystem which is not mounted with the 'nodev' option.
#
# If you need to test this from within this directory:

# make cleandir
# make obj
# make TARGET_ARCH=i386 TARGET_MEDIA=CDROM CDROM_ISO=closedbsd.iso SRC=/usr/src
#
# ----------------------------------------------------------------------------

# Force everything to happen in a subdirectory if ${.OBJDIR} would evaluate
# to ${.CURDIR}, to avoid clobbering the build tree. We do this in such
# a way that if MAKEOBJDIR[PREFIX] is not set, the obj target won't get
# called which would cause an error, so that autonomous builds can take
# place with the objects dirtying the source tree if we so choose.

.if ${.OBJDIR} != ${.CURDIR}
RD=${.OBJDIR}
RDOBJDIRTARGET=obj
.else
RD=${.CURDIR}/root
CLEANFILES=""
CLEANDIRS=root
RDOBJDIRTARGET=objdirtarget

objdirtarget:
	mkdir -p ${RD}
.endif

BOOT_OPTIONS=	"-C" 

LABEL=		CLOSEDBSD_CDROM

CLEANDIR= ${RD}

MONOLITH=	monolith1
SBIN_LINKS=	cat dd dhclient config dev_mkdb grep gzip init ifconfig \
		ipfw kget  kill ln umount ls mount natd ping chmod killall \
		ps pwd pwd_mkdb reboot rm route sysctl tar mount_mfs \
		newfs
BIN_LINKS=	cp hostname test \[

# ----------------------------------------------------------------------------

all: ${.OBJDIR}/${CDROM_ISO}

# Final ISO build pass.
${.OBJDIR}/${CDROM_ISO}:	tree installboot \
				installkernel installbins installmonolith
	( cd ${RD} ; mkisofs -V ${LABEL} -J -l -r -L -b boot/cdboot \
			     -no-emul-boot -c boot.catalog \
			     -o ${.OBJDIR}/${CDROM_ISO} ${RD} )

#
# Create the ClosedBSD tree and populate it.
#
# XXX: would it be nicer to use INSTALL_SCRIPT?
#
SCRIPT_MODE=755
STATIC_SCRIPTS="sbin/dhclient-script sbin/ifs.sh sbin/restoretc sbin/sysctl.sh sbin/writetc sbin/writetcnp defaults/etc/rc "

tree: ${RDOBJDIRTARGET}
	@mkdir -p ${RD}
	( cd ${RD} ; mtree -deU -f ${.CURDIR}/closedbsd.mtree )
	( cd ${.CURDIR}/staticroot ; tar cf - . ) | ( cd ${RD} ; tar xf - )
.for __script in ${STATIC_SCRIPTS}
	chmod ${SCRIPT_MODE} ${RD}/${__script}
.endfor
	( cd ${RD}/dev ; sh MAKEDEV std ; dev_mkdb -f ${RD}/var/run/dev.db . )
	touch ${RD}/etc/rules.db
	#pwd_mkdb -d ${RD}/etc ${RD}/etc/master.passwd	#XXX not using defaults
	#pwd_mkdb -d ${RD}/etc ${RD}/defaults/etc/master.passwd
	# XXX this process is munged. could this be causing a hang?
	pwd_mkdb -d ${RD}/defaults/etc ${RD}/defaults/etc/master.passwd
	( cd ${RD}/usr/share/misc ; ln -s /etc/termcap termcap )

# ----------------------------------------------------------------------------
#
# The following targets pull in binaries from the object tree and
# install them within the temporary tree.
#

# Install the /boot binaries. Install the kernel within the ISO tree.
# N.B: the kernel is now compressed with kgzip(8).

installboot: buildboot tree installkernel
	( cd ${SRC}/sys/boot ; \
	  make MACHINE=${TARGET_ARCH} DESTDIR=${RD} NOMAN= install)
	echo "boot ${BOOT_OPTIONS}" >> ${RD}/boot/loader.rc

installkernel:	${.OBJDIR}/../kernel/kernel.CLOSEDBSD_${TARGET_MEDIA} tree
	kgzip -v -o ${RD}/kernel \
		${.OBJDIR}/../kernel/kernel.CLOSEDBSD_${TARGET_MEDIA}

# Install the monolithic command image. Create symlinks within the tree
# to the commands encapsulated by this image.

installmonolith: tree
	( cd ${.CURDIR}/../monolith ; make install DESTDIR=${RD} )
.for __link in ${BIN_LINKS}
	( cd ${RD}/bin ; ln -s ../sbin/${MONOLITH} ${__link} )
.endfor
.for __link in ${SBIN_LINKS}
	( cd ${RD}/sbin ; ln -s ${MONOLITH} ${__link} )
.endfor

# Install additional binaries.
installbins: buildsh tree
	-( cd ${SRC}/bin/sh ; make -DNOMAN install DESTDIR=${RD} )

# ----------------------------------------------------------------------------
#
# The following targets build additional binaries which are pulled in
# during the build process by the above targets.
#

# Build the stage 2 bootstrap.
buildboot:
	( cd ${SRC}/sys/boot ; make MACHINE=${TARGET_ARCH} obj; \
	  make MACHINE=${TARGET_ARCH} )

# Build the ash shell.
buildsh:
	-( cd ${SRC}/bin/sh ; make obj ; make )

# ----------------------------------------------------------------------------
.include <bsd.prog.mk>
