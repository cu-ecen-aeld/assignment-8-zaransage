# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)

# WARNING: the following LICENSE and LIC_FILES_CHKSUM values are best guesses - it is
# your responsibility to verify that the values are complete and correct.
#
# The following license files were not able to be identified and are
# represented as "Unknown" below, you will need to check them yourself:
#   LICENSE
LICENSE = "Unknown"
LIC_FILES_CHKSUM = "file://LICENSE;md5=f098732a73b5f6f3430472f5b094ffdb"

SRC_URI = "git://github.com/cu-ecen-aeld/assignment-7-zaransage;protocol=https;branch=master"
SRC_URI += "file://scull_init.sh"
SRC_URI += "file://access_ok_version.h"
SRC_URI += "file://proc_ops_version.h"

# Modify these as desired
PV = "1.0+git${SRCPV}"
SRCREV = "f02fbee1bb44c17ad8491d0c4c9c10f650b821c1"

S = "${WORKDIR}/git"

inherit module update-rc.d

MODULES_INSTALL_TARGET = "install"
#EXTRA_OEMAKE += "KERNELDIR=${STAGING_KERNEL_DIR}"
EXTRA_OEMAKE += " -C ${STAGING_KERNEL_DIR} M=${S}/scull"

INITSCRIPT_NAME = "scull_init.sh"
INITSCRIPT_PARAMS = "start 98 5 . stop 02 0 1 6 ."

do_configure:append() {
    install -Dm0644 ${WORKDIR}/access_ok_version.h ${S}/scull/access_ok_version.h
    install -Dm0644 ${WORKDIR}/proc_ops_version.h ${S}/scull/proc_ops_version.h
}

do_install:append(){
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/scull-init ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}
}