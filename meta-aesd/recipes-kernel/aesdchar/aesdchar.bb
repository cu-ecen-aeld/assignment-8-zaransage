                                                                                                                                                                          
DESCRIPTION = "Build and set the aesd char driver."                                                                                                                    
LICENSE = "MIT"                                                                                                                                                                                         
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"                                                                                                              

# I do not like this right now but if I eventually move this to another repo as its own driver this would make sense.                                                                                                                                                                                                        
SRC_URI = "git://git@github.com/cu-ecen-aeld/assignments-3-and-later-zaransage.git;protocol=ssh;branch=master"
SRC_URI += "file://aesdchar_init.sh"                                                                                                                                                                    
                
PV = "1.0+git${SRCPV}"                                                                                                                                                                                  
SRCREV = "e6600c5ec0a7b538558a45850e5253b2c6d3f6a9"
                                                                                                                                                                                                        
S = "${WORKDIR}/git"
inherit module update-rc.d                                                                                                                                                                              
 
# Mostly boiler plate code from the other driver examples.
MODULES_INSTALL_TARGET = "install"     

EXTRA_OEMAKE += " -C ${STAGING_KERNEL_DIR} M=${S}/aesd-char-driver"
INITSCRIPT_NAME = "aesdchar_init.sh"                                                                                                                                                                    
INITSCRIPT_PARAMS = "start 97 5 . stop 03 0 1 6 ."
                                                                                                                                                                                                        
do_compile() {  
    cd ${S}/aesd-char-driver
    oe_runmake aesdchar.ko
}
                                                                                                                                                                                                        
do_install() {
    oe_runmake KERNEL_SRC=${STAGING_KERNEL_DIR} \                                                                                                                                                       
        M=${S}/aesd-char-driver \
        INSTALL_MOD_PATH=${D} \
        modules_install
}
do_install:append() {                                                                                                                                                                                   
    install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
    install -m 0644 ${S}/aesd-char-driver/aesdchar.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/                                                                                       
                                                                                                                                                                                                        
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/aesdchar_init.sh ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}                                                                                                             
}                                                                                                                                                                                                       
 
FILES:${PN} += "${sysconfdir}/init.d"                                                                                                                                                                   
FILES:${PN} += "${sysconfdir}/init.d/aesdchar_init.sh"
PACKAGES =+ "kernel-module-aesdchar"        

FILES:kernel-module-aesdchar = "${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/aesdchar.ko"