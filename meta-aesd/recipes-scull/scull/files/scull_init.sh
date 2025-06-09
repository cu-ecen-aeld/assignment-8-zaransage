#!/bin/sh
# Source for load: https://github.com/cu-ecen-aeld/ldd3/blob/master/scull/scull_load
# Source for unload: https://github.com/cu-ecen-aeld/ldd3/blob/master/scull/scull_unload

# $Id: scull_load,v 1.4 2004/11/03 06:19:49 rubini Exp $
module="scull"
device="scull"
mode="664"

# Group: since distributions do it differently, look for wheel or use staff
if grep -q '^staff:' /etc/group; then
    group="staff"
else
    group="wheel"
fi

# Regular command handling

case "$1" in
  start)
    # invoke insmod with all arguments we got
    # and use a pathname, as insmod doesn't look in . by default
    insmod /lib/modules/5.15.164-yocto-standard/extra/$module.ko $* || exit 1
    
    # retrieve major number
    major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
  
    rm -f /dev/${device}[0-3]
    mknod /dev/${device}0 c $major 0
    mknod /dev/${device}1 c $major 1
    mknod /dev/${device}2 c $major 2
    mknod /dev/${device}3 c $major 3
    ln -sf ${device}0 /dev/${device}
    chgrp $group /dev/${device}[0-3] 
    chmod $mode  /dev/${device}[0-3]
    ;;
  stop)
    rmmod $module || exit 1
    rm -f /dev/${device} /dev/${device}[0-3] 
    ;;
    *)
      echo "Use $0 {start|stop}"
      exit 1
esac

exit 0