SUMMARY = "AESD Socket Server"
DESCRIPTION = "Yocto recipe for building and installing the aesdsocket application."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

#SRC_URI = "git://git@github.com/cu-ecen-aeld/assignments-3-and-later-zaransage.git;protocol=ssh;branch=master"
#SRCREV = "87af7ecf5b156122a91d81632a16793c6a49eaf3"

SRC_URI = "git://github.com/cu-ecen-aeld/assignments-3-and-later-zaransage.git;protocol=https;branch=4.17.2025"
SRCREV  = "fdcf15b185b2f7748390c0aa091d32c94fc829b8"
PV = "1.0+git${SRCPV}"

S = "${WORKDIR}/git/server"

INSANE_SKIP_${PN} += "debugsplit"

#inherit autotools

# NOTE: this is a Makefile-only piece of software, so we cannot generate much of the
# recipe automatically - you will need to examine the Makefile yourself and ensure
# that the appropriate arguments are passed in.

do_configure () {
	# Specify any needed configure commands here
	:
}

do_compile () {
	oe_runmake
}

STARTUP_NAME="aesdsocket-start-stop.sh"

do_install() {
    install -d ${D}${bindir}
    echo "Installing binary from ${B}:"
    ls -l ${B}/aesdsocket
    file ${B}/aesdsocket
    install -m 0755 ${B}/aesdsocket ${D}${bindir}/aesdsocket
    
    echo "Installing Startup and init scripts:"
    ls -l ${B}/${STARTUP_NAME}
    install -d ${D}${sysconfdir}/init.d/
    file ${B}/${STARTUP_NAME}
    install -m 0755 ${B}/${STARTUP_NAME} ${D}${sysconfdir}/init.d/${STARTUP_NAME}
}
